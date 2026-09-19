// Startup
// myserver maxUsers serviceName msglenInd
// The msglenInd is used for communications with the bank. Messages to the bank can be null-terminated (N),
// include the total length (T), or include the message length (M).
//
// Clients
// Two types of clients are handled
// - A client that wants to process bank messages
// - A client that wants to process control commands. A maximum of 1 client can process control commands.
//
// Error processing
// For the following errors, an error message is sent to the client and the client is disconnected
// - Maximum users exceeded
// - Invalid request
// For all other errors, the client is disconnected without a message.
//
// Log file
// The server writes a log file. Control commands can be used to control the log level and packages to be
// logged. Messages are formatted so that grep can be used. For e.g.
// grep ": fd 7:" myserver.log.YYYY-MM-DD_HHMMSS
//
// Lock file
// The server writes a lock file to prevent multiple servers from being started. In case of a successful
// shutdown, the file is deleted. In all other cases, the lock file must be manually deleted after
// checking the log file. The server cannot be restarted until the lock file is deleted.
//
// Core dumps
// - Check the log file for a core dump or
// - On the command line, specify
//   coredumpctl debug
//   info threads
//
// Other servers
// The server communicates with a database server to acquire an audit number.
// The server communicates with a bak server to validate the credit card number.
//
// Messages
// Messages between client and server are null-terminated.
// Messages between server and database server are null-terminated.
// Messages between server and bank server are as specified by the msglenInd.
//
// Timeouts
// For bank messages, the wait time between phases is indicated by BANKMSG_WAIT_TIME.
// For control commands, the wait time between requests is indicated by IACTIVEMSG_WAIT_TIME.
// In case of a timeout, the client is disconnected without a message. For bank messages, the server
// status indicates the phase that timed out. The server status can be:
// Blank - The database server is down
// DS - The message was sent to the database server but no response was received
// DR - The message was received from the database server but not sent to the bank server
// BS - The message was sent to the bank server but no response was received
// BR - The message was received from the bank server
// See section Flow (bank messages) for further details
//
// Disconnecting clients
// This must be handled by the Main thread only. All other threads must set a disconnnect flag and
// trigger an event in the Main thread.
//
// Flow (bank messages)
// Client sends an initial message to the server (Main thread).
// Server obtains an audit number for the message from the database server (DbSendThread and DbRecvThread).
// Server obtains an approval/decline from the bank server (BankSendThread and BankRecvThread).
// Server sends a response to the client and waits for a completion message (Main thread).
// Client sends a completion message to the server (Main thread).
// The server cleans up and disconnects the client (Main thread).
//
// Flow (control commands)
// Client sends a control command to the server (Main thread).
// Server sends a response to the client (Main thread).
// When the client is finished, it sends a disconnect to the server. Alternatively, if the session is inactive
// for a period of time (IACTIVEMSG_WAIT_TIME), the server disconnects the client (Main thread).
//
// Timer Thread
// This thread constantly monitors the activity for all clients and disconnects them as required.
//
// SignalHandler Thread
// This thread handles signal processing for the server. There are two types of shutdown signals handled:
// - INT abortive shutdown; the server shuts down immediately leaving the lock file. On the command-line,
//   specify kill -INT pidOfServer
// - TERM orderly shutdown; the server shuts down only if there are no clients connected. A successful
//   shutdown deletes the lock file. On the command-line, specify kill -TERM pidOfServer

#include <errno.h>

#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/eventfd.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <bankmsgdefs.h>
#include <dbmsgdefs.h>
#include <errcodes.h>
#include <loglvldefs.h>
#include <logpkgdefs.h>
#include <msglenind.h>
#include <servermsgdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <ccrtns.h>
#include <cmdrtns.h>
#include <daemonrtns.h>
#include <datmutils.h>
#include <filertns.h>
#include <fmtstr.h>
#include <llsocket.h>
#include <llunistd.h>
#include <lockfilertns.h>
#include <logcmds.h>
#include <logfilertns.h>
#include <logpkgcmds.h>
#include <logpkgrtns.h>
#include <logrtns.h>
#include <memrtns.h>
#include <msglenrtns.h>
#include <pthreadrtns.h>
#include <socketrecv.h>
#include <socketsend.h>
#include <strutils.h>
#include <sysexit.h>
#include <tcprtns.h>
#include <thrinfo.h>

#define LISTEN_BACKLOG 5

#define ListenFdMpx 0
#define EventFdMpx 1
#define ClientFdMpx 2

#define BANKMSG_WAIT_TIME 20
#define IACTIVEMSG_WAIT_TIME 120

#define SIGEMPTYSET(set) ({ \
    if (sigemptyset(set)) \
        SysExit(__func__, errno, SYSCALL_SIGEMPTYSET); })
#define SIGADDSET(set, signum) ({ \
    if (sigaddset(set, signum)) \
        SysExit(__func__, errno, SYSCALL_SIGADDSET); })

typedef enum
{
    PROCSTAT_INIT = 0,
    PROCSTAT_CT_REQ_RECV,
    PROCSTAT_DB_SEND,
    PROCSTAT_DB_RECV,
    PROCSTAT_BANK_SEND,
    PROCSTAT_BANK_RECV,
    PROCSTAT_CT_RESP_SEND,
} procstat_t;

typedef enum
{
    PROCTYPE_INIT = 0,
    PROCTYPE_IACTIVE,
    PROCTYPE_BANK
} proctype_t;

typedef enum
{
    DISCON_INIT = 0,
    DISCON_AFTER_MSG,
    DISCON_NOW
} disconstat_t;

typedef struct
{
    pthread_mutex_t mutex;

    time_t activityTime;

    disconstat_t disconStat;
    proctype_t procType;
    procstat_t procStat;

    string_t disconMsg[100];

    char *request;
    int requestLen;

    servermsg_t servermsg;

    char *response;
    int responseLen;
    int prevBytesSent;
} clientinfo_t;

typedef enum
{
    DISCONNECTED = 0,
    CONNECTED
} connstat_t;

typedef struct
{
    pid_t pid;
    unsigned long maxUsers;
    unsigned long maxPoll;
    string_t svcName[SZ_SERVICE_NAME + 1];
    string_t msglenInd[SZ_MSGLEN_IND + 1];
    string_t workingDir[SZ_FULL_PATH_NAME + 1];
    int rl;
} startInfo_t;

static struct pollfd *MainPoll;
static clientinfo_t *ClientInfo;

static pthread_t MainThreadId;
static pthread_t SignalHandlerId;

static pthread_t DbSendId;
static pthread_cond_t DbSendCond;
static pthread_mutex_t DbSendMutex;
static atomic_int DbSendWorkPending;
static connstat_t DbStatus;
static int DbFd;
static pthread_t DbRecvId;

static pthread_t BankSendId;
static pthread_cond_t BankSendCond;
static pthread_mutex_t BankSendMutex;
static atomic_int BankSendWorkPending;
static connstat_t BankStatus;
static int BankFd;
static pthread_t BankRecvId;

static pthread_t TimerThreadId;
static pthread_cond_t TimerThreadCond;
static atomic_int ServerWorkPending;
static pthread_mutex_t TimerThreadMutex;

static pthread_mutex_t ThrInfoMutex;

static startInfo_t StartInfo;

static atomic_int OrderlyShutdown;

static constr_t ProgramName = "myserver";
static constr_t LockfileName = "/home/kerman/projects/GoogleTestC/myserver.lock";
static constr_t LogfileNamePrefix = "/home/kerman/projects/GoogleTestC/myserver.log";
static string_t LogfileName[SZ_FULL_PATH_NAME];
static int LogfileFd = -1;

static void validateInput(const int argc, const char **argv, unsigned long *const maxUsers, string_t svcName[SZ_SERVICE_NAME + 1],
                             string_t msglenInd[SZ_MSGLEN_IND + 1]);
static void showUsage(void);

static void setCurrentDir(string_t workingDir[SZ_FULL_PATH_NAME + 1]);
static void setFileLimit(const int limit);
static void writeLogfileHeaders(void);

static void initThreads(void);

static void recvRequest(const int mpx);
static void sendResponse(const int mpx);

static void *SignalHandlerThread(void *arg);
static bool_t ignoreSignal(int signo);

static void *TimerThread(void *arg);
static void setClientDisconnect(const int i, constr_t disconMsg);
static void setServerWorkPending(const bool_t sendSignal);

static void procNewClient(const int listenFd, struct sockaddr_in * sockaddr, const int * const sockaddr_len);
static void procCtrlCmd(const int mpx, const char * const request, const int msgLen);

static void *DbSendThread(void *arg);
static void *DbRecvThread(void *arg);
static void setDbSendWorkPending(const bool_t sendSignal);

static void *BankSendThread(void *arg);
static void *BankRecvThread(void *arg);
static void setBankSendWorkPending(const bool_t sendSignal);
static int xmitBankMsg(const int fd, const char * const msg, const int msgLen);

static void setupClientResponse(constr_t caller, const int i, const char * const msg, const int msgLen);

static void disconnectClients(void);
static void closeClientAtIdx(const int i);
static void initClientAtIdx(const int i);

static void endServer(void);

int main(const int argc, const char **argv)
{
    SysSetPrintStack(1);
    LogLvlEnable(LOGLVL_INFO | LOGLVL_TRACE);
    LogPkgEnable(LOGPKG_WIRE);

    // Validate input
    validateInput(argc, argv,  &StartInfo.maxUsers, StartInfo.svcName, StartInfo.msglenInd);

    // Check if the process is already running
    LockfileCheck(LockfileName);

    fclose(stdin);
    fclose(stdout);

    setCurrentDir(StartInfo.workingDir);

    StartInfo.maxPoll = StartInfo.maxUsers + 10;

    // 200 includes gcda files that are opened
    StartInfo.rl = StartInfo.maxPoll + 200;
    setFileLimit(StartInfo.rl);

    DaemonizeServer();

    StartInfo.pid = getpid();

    LockfileWritePid(LockfileName, StartInfo.pid);

    // Open log file
    LogfileOpen(LogfileNamePrefix, LogfileName, &LogfileFd);
    writeLogfileHeaders();

    struct sockaddr_in sockaddr;
    int listenFd;
    int sockaddr_len = sizeof(sockaddr);

    TcpSetListenSocket("localhost", StartInfo.svcName, LISTEN_BACKLOG, &listenFd, &sockaddr, sockaddr_len);

    int eventFd = eventfd(0, 0);
    if (eventFd == -1)
        SysExit(__func__, errno, SYSCALL_EVENTFD);

    MainPoll = calloc(StartInfo.maxPoll, sizeof(struct pollfd));
    if (MainPoll == NULL) SysExit(__func__, errno, SYSCALL_CALLOC);
    ClientInfo = calloc(StartInfo.maxPoll, sizeof(clientinfo_t));
    if (ClientInfo == NULL) SysExit(__func__, errno, SYSCALL_CALLOC);

    for (int i = ListenFdMpx; i < StartInfo.maxPoll; i++)
        initClientAtIdx(i);
    for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
        PTHREAD_MUTEX_INIT(&ClientInfo[i].mutex, NULL);

    MainPoll[ListenFdMpx].fd = listenFd;
    MainPoll[ListenFdMpx].events = POLLIN;
    MainPoll[EventFdMpx].fd = eventFd;
    MainPoll[EventFdMpx].events = POLLIN;

    MainThreadId = pthread_self();
    LogInfoF("Main Thread Id %p", MainThreadId);

    // Block all signals
    sigset_t sigSet;
    if (sigfillset(&sigSet))
        SysExit(__func__, errno, SYSCALL_SIGFILLSET);
    if (pthread_sigmask(SIG_BLOCK, &sigSet, NULL))
        SysExit(__func__, errno, SYSCALL_PT_SIGMASK);

    initThreads();

    for (;;)
    {
        int ret;

        for (int i = ListenFdMpx; i < StartInfo.maxPoll; i++)
        {
            MainPoll[i].revents = 0;
        }

        ret = poll(MainPoll, StartInfo.maxPoll, -1);
        if (ret < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            SysExit(__func__, errno, SYSCALL_POLL);
        }

        for (int mpx = ListenFdMpx; mpx < StartInfo.maxPoll; mpx++)
        {
            if (MainPoll[mpx].fd == -1 || !MainPoll[mpx].revents)
                continue;

            if (MainPoll[mpx].fd == listenFd)
            {
                procNewClient(listenFd, &sockaddr, &sockaddr_len);
                continue;                
            }

            if (MainPoll[mpx].fd == eventFd)
            {
                uint64_t u;
                ret = read(MainPoll[EventFdMpx].fd, &u, sizeof(uint64_t));
                if (ret < 0) SysExit(__func__, errno, SYSCALL_READ);

                if (OrderlyShutdown)
                    break;

                disconnectClients();    
                continue;                
            }

            PTHREAD_MUTEX_LOCK(&ClientInfo[mpx].mutex);

            if (MainPoll[mpx].fd != -1 && MainPoll[mpx].revents & POLLIN)
            {
                recvRequest(mpx);
                continue;
            }

            if (MainPoll[mpx].fd != -1 && MainPoll[mpx].revents & POLLOUT)
            {
                sendResponse(mpx);
                continue;
            }

            PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        }

        if (OrderlyShutdown)
            break;
    }

    for (int mpx = ClientFdMpx; mpx < StartInfo.maxPoll; mpx++)
        PTHREAD_MUTEX_DESTROY(&ClientInfo[mpx].mutex);

    for (int mpx = ClientFdMpx; mpx < StartInfo.maxPoll; mpx++)
    {
        if (MainPoll[mpx].fd == -1)
            continue;
        closeClientAtIdx(mpx);
    }

    endServer();
    return(0);
}

static void recvRequest(const int mpx)
{
    int ret;

    bool_t msgComplete = 0;
    ret = SocketRecv(ProgramName, MainPoll[mpx].fd, "N",
                        &ClientInfo[mpx].request,
                        &ClientInfo[mpx].requestLen, &msgComplete);
    if (ret == 1)
    {
        PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: Disconnect received from client", ProgramName, MainPoll[mpx].fd);
        closeClientAtIdx(mpx);
        return;
    }

    if (!msgComplete)
    {
        PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        return;
    }

    MsgPrint(ProgramName, MainPoll[mpx].fd, "N", ClientInfo[mpx].request, ClientInfo[mpx].requestLen, "receiving");

    ClientInfo[mpx].procStat = PROCSTAT_CT_REQ_RECV;
    ClientInfo[mpx].activityTime = time(NULL);

    if (ClientInfo[mpx].disconStat == DISCON_AFTER_MSG)
    {
        setupClientResponse(ProgramName, mpx, ClientInfo[mpx].disconMsg, strlen(ClientInfo[mpx].disconMsg));

        PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        return;
    }

    int msgLen = MsgLenFromXmitLen("N", ClientInfo[mpx].requestLen);
    int startPos = MsgStartPos("N");

    const char *request = ClientInfo[mpx].request + startPos;
    
    if (*request == '?')
    {
        for (int j = ClientFdMpx; j < StartInfo.maxPoll; j++)
        {
            if (MainPoll[mpx].fd != MainPoll[j].fd && ClientInfo[j].procType == PROCTYPE_IACTIVE)
            {
                setClientDisconnect(mpx, "Too many interactive sessions");
                PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
                return;
            }
        }

        ClientInfo[mpx].procType = PROCTYPE_IACTIVE;
        procCtrlCmd(mpx, request, msgLen);
        PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        return;
    }

    if (msgLen != sizeof(servermsg_t))
    {
        char sendline[100];
        strlcpy(sendline, "ERROR: Invalid request", sizeof(sendline));
        setupClientResponse(ProgramName, mpx, sendline, strlen(sendline));

        ClientInfo[mpx].disconStat = DISCON_AFTER_MSG;

        PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        return;
    }

    ClientInfo[mpx].procType = PROCTYPE_BANK;
    servermsg_t * servermsg = &ClientInfo[mpx].servermsg;
    memcpy(servermsg, request, msgLen);

    if (servermsg->initialMsg == '0')
    {
        MemFree(ProgramName, &ClientInfo[mpx].request);
        ClientInfo[mpx].requestLen = 0;
        PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: Disconnecting client (after completion message)", ProgramName, MainPoll[mpx].fd);
        closeClientAtIdx(mpx);
        return;
    }
    // TODO Validate bank message
    
    ClientInfo[mpx].procStat = PROCSTAT_DB_SEND;
    ClientInfo[mpx].activityTime = time(NULL);
    PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
    setDbSendWorkPending(1);
}

static void sendResponse(const int mpx)
{
    int ret;
    MsgPrint(ProgramName, MainPoll[mpx].fd, "N", ClientInfo[mpx].response, ClientInfo[mpx].responseLen, "sending");
    ret = SocketSendYield(MainPoll[mpx].fd, ClientInfo[mpx].response, ClientInfo[mpx].responseLen,
                                &ClientInfo[mpx].prevBytesSent);
    if (ret == 1)
    {
        PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);
        return;
    }

    MemFree(ProgramName, &ClientInfo[mpx].response);
    ClientInfo[mpx].responseLen = 0;
    ClientInfo[mpx].prevBytesSent = 0;

    PTHREAD_MUTEX_UNLOCK(&ClientInfo[mpx].mutex);

    if (ClientInfo[mpx].disconStat == DISCON_AFTER_MSG)
    {
        LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: Disconnecting client (after error message)", ProgramName, MainPoll[mpx].fd);
        closeClientAtIdx(mpx);
    }
    else
    {
        MainPoll[mpx].events = POLLIN;
    }
}

static void validateInput(const int argc, const char **argv, unsigned long * const maxUsers, string_t svcName[SZ_SERVICE_NAME + 1],
                             string_t msglenInd[SZ_MSGLEN_IND + 1])
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        showUsage();
        exit(1);
    }

    if (strnlen(argv[1], SZ_MAX_USERS + 1) == SZ_MAX_USERS + 1)
    {
        fprintf(stderr, "Number of users too long (Maximum 2 digits)\n");
        showUsage();
        exit(1);
    }
    *maxUsers = strtoul(argv[1], NULL, 0);
    if (errno == EINVAL || errno == ERANGE || *maxUsers == 0)
    {
        fprintf(stderr, "Invalid maximum number of users (1-99)\n");
        showUsage();
        exit(1);
    }

    if (strnlen(argv[2], SZ_SERVICE_NAME + 1) == SZ_SERVICE_NAME + 1)
    {
        fprintf(stderr, "Service name too long (Maximum 10 characters)\n");
        showUsage();
        exit(1);
    }
    strlcpy(svcName, argv[2], SZ_SERVICE_NAME + 1);

    if (strnlen(argv[3], SZ_MSGLEN_IND + 1) == SZ_MSGLEN_IND + 1)
    {
        fprintf(stderr, "Message length indicator too long (Maximum 1 character)\n");
        showUsage();
        exit(1);
    }
    strlcpy(msglenInd, argv[3], SZ_MSGLEN_IND + 1);
    if (*msglenInd != MSG_WITH_NULL && *msglenInd != MSG_WITH_TOTLEN && *msglenInd != MSG_WITH_MSGLEN)
    {
        fprintf(stderr, "Invalid message length indicator (N, T or M)\n");
        showUsage();
        exit(1);
    }

}

static void showUsage(void)
{
    fprintf(stderr, "Usage: myserver maxUsers serviceName msglenInd\n");
    fprintf(stderr, "       maxUsers must be between 1-99\n");
    fprintf(stderr, "       serviceName must not exceed 10 characters\n");
    fprintf(stderr, "       msglenInd for bank messages must not exceed 1 character (N, T or M)\n");
}

static void setCurrentDir(string_t workingDir[SZ_FULL_PATH_NAME + 1])
{
    char *cwd = getcwd(NULL, SZ_FULL_PATH_NAME + 1);
    if (cwd == NULL)
        SysExit(__func__, errno, SYSCALL_GETCWD);

    strlcpy(workingDir, cwd, SZ_FULL_PATH_NAME + 1);
    free(cwd);
}

static void setFileLimit(const int limit)
{
    struct rlimit rLimit;
    memset(&rLimit, 0, sizeof(struct rlimit));
    if (getrlimit(RLIMIT_NOFILE, &rLimit))
        SysExit(__func__, errno, SYSCALL_GETRLIMIT);

    rLimit.rlim_cur = limit;

    if (setrlimit(RLIMIT_NOFILE, &rLimit))
        SysExit(__func__, errno, SYSCALL_SETRLIMIT);
}

static void writeLogfileHeaders(void)
{
    LogInfoF("Server pid %d", StartInfo.pid);
    LogInfoF("Max Users: %d", StartInfo.maxUsers);
    LogInfoF("Service Name: %s", StartInfo.svcName);
    LogInfoF("Bank Message Length Indicator: %s",
        *StartInfo.msglenInd == MSG_WITH_NULL ? "Bank messages will be null-terminated" :
        *StartInfo.msglenInd == MSG_WITH_TOTLEN ? "Bank messages will include total length" :
            "Bank messages will include the message length");
    LogInfo("Server messages will be null-terminated");
    LogInfoF("Current working directory: %s", StartInfo.workingDir);
    LogInfoF("Resource limit: %d", StartInfo.rl);
    LogInfoF("Log file: %s", LogfileName);
}

static void initThreads(void)
{
    PTHREAD_MUTEX_INIT(&ThrInfoMutex, NULL);
    PTHREAD_MUTEX_INIT(&DbSendMutex, NULL);
    PTHREAD_MUTEX_INIT(&BankSendMutex, NULL);
    PTHREAD_MUTEX_INIT(&TimerThreadMutex, NULL);

    PTHREAD_COND_INIT(&DbSendCond, NULL);
    PTHREAD_COND_INIT(&BankSendCond, NULL);
    PTHREAD_COND_INIT(&TimerThreadCond, NULL);

    PTHREAD_CREATE(&SignalHandlerId, NULL, SignalHandlerThread, NULL);
    PTHREAD_CREATE(&TimerThreadId, NULL, TimerThread, NULL);
    PTHREAD_CREATE(&DbSendId, NULL, DbSendThread, NULL);
    PTHREAD_CREATE(&BankSendId, NULL, BankSendThread, NULL);
}

static void *SignalHandlerThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd(ProgramName, SignalHandlerId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, SignalHandlerId);

    sigset_t sigSet;
    SIGEMPTYSET(&sigSet);
    SIGADDSET(&sigSet, SIGINT);
    SIGADDSET(&sigSet, SIGTERM);
    SIGADDSET(&sigSet, SIGQUIT);

    for (;;)
    {
        siginfo_t sigInfo;
        int signo;
        signo = sigwaitinfo(&sigSet, &sigInfo);
        if (signo < 0)
        {
            if (errno == EINTR)
                continue;
            SysExit(__func__, errno, SYSCALL_SIGWAITINFO);
        }

        if (signo == SIGINT)
            exit(1);

        if (signo == SIGTERM)
        {
            if (ignoreSignal(signo)) continue;

            OrderlyShutdown = 1;

            uint64_t u = 1;
            int ret = write(MainPoll[EventFdMpx].fd, &u, sizeof(uint64_t));
            if (ret < 0) SysExit(__func__, errno, SYSCALL_WRITE);
        }
        else if (signo == SIGQUIT)
        {
            if (sigInfo.si_pid != StartInfo.pid) continue;
            break;
        }
    }        

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static void *TimerThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd(ProgramName, TimerThreadId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, TimerThreadId);

    for (;;)
    {
        PTHREAD_MUTEX_LOCK(&TimerThreadMutex);
        while (!OrderlyShutdown && !ServerWorkPending)
        {
            PTHREAD_COND_WAIT(&TimerThreadCond, &TimerThreadMutex);
        }
        PTHREAD_MUTEX_UNLOCK(&TimerThreadMutex);

        if (OrderlyShutdown)
            break;

        for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
        {
            if (MainPoll[i].fd == -1)
                continue;
            if (ClientInfo[i].disconStat != DISCON_INIT)
                continue;

            time_t timeNow = time(NULL);
            int waitTime = (ClientInfo[i].procType == PROCTYPE_BANK) ? BANKMSG_WAIT_TIME : IACTIVEMSG_WAIT_TIME;
            time_t elapsedTime = timeNow - ClientInfo[i].activityTime;
            if (elapsedTime < waitTime)
                continue;

            PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
            if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT)
            {
                string_t * actionReqd;
                if (!memcmp(ClientInfo[i].servermsg.status, "BS", SZ_SERVER_STATUS))
                {
                    actionReqd = "CHECK WITH BANK";
                }
                else if (!memcmp(ClientInfo[i].servermsg.status, "BR", SZ_SERVER_STATUS))
                {
                    actionReqd = "CHECK UNPROCESSED";
                }
                else if (!memcmp(ClientInfo[i].servermsg.status, "DS", SZ_SERVER_STATUS))
                {
                    actionReqd = "CHECK POSSIBLE UNUSED AUDIT NO";
                }
                else if (!memcmp(ClientInfo[i].servermsg.status, "DR", SZ_SERVER_STATUS))
                {
                    actionReqd = "CHECK UNUSED AUDIT NO";
                }
                else
                {
                    actionReqd = "RETRY LATER";
                }
                LogWarnF("%s: fd %d: %s [%.*s]", ProgramName, MainPoll[i].fd, actionReqd, sizeof(servermsg_t), (char *) &ClientInfo[i].servermsg);
                setClientDisconnect(i, "Timed out");
            }
            PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
        }
        setServerWorkPending(0);

        if (ServerWorkPending)
            sleep(1);
    }

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static void setClientDisconnect(const int i, constr_t disconMsg)
{
    ClientInfo[i].disconStat = DISCON_NOW;
    strlcpy(ClientInfo[i].disconMsg, disconMsg, sizeof(ClientInfo[i].disconMsg));

    uint64_t u = 1;
    int ret = write(MainPoll[EventFdMpx].fd, &u, sizeof(uint64_t));
    if (ret < 0) SysExit(__func__, errno, SYSCALL_WRITE);
}

static void setServerWorkPending(const bool_t sendSignal)
{
    PTHREAD_MUTEX_LOCK(&TimerThreadMutex);
    ServerWorkPending = 0;
    for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
    {
        if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT)
        {
            PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
            ServerWorkPending = (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT);
            PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
            if (ServerWorkPending)
            {
                break;
            }
        }
    }
    if (ServerWorkPending && sendSignal)
        PTHREAD_COND_SIGNAL(&TimerThreadCond);

    PTHREAD_MUTEX_UNLOCK(&TimerThreadMutex);
}

static bool_t ignoreSignal(int signo)
{
    for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
    {
        if (MainPoll[i].fd != -1)
        {
            LogWarnF("'%s' signal ignored, clients being processed", strsignal(signo));
            return(1);
        }
    }
    return(0);
}

static void procNewClient(const int listenFd, struct sockaddr_in * sockaddr, const int * const sockaddr_len)
{
    int clientFd;
    
    for (;;)
    {
        clientFd = accept(listenFd, (struct sockaddr *) sockaddr, (socklen_t *) sockaddr_len);
        if (clientFd < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            SysExit(__func__, errno, SYSCALL_ACCEPT);
        }
        break;
    }
    
    int j;
    for (j = ClientFdMpx; j < StartInfo.maxPoll; j++)
    {
        if (MainPoll[j].fd == -1)
        {
            LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: Connected client mpx %d", ProgramName, clientFd, j);

            MainPoll[j].fd = clientFd;
            MainPoll[j].events = POLLIN;
            break;
        }
    }

    int maxUsers = 0;
    for (int k = ClientFdMpx; k < StartInfo.maxPoll; k++)
    {
        if (MainPoll[k].fd != -1)
            maxUsers++;
    }

    if (maxUsers > StartInfo.maxUsers)
    {
        strlcpy(ClientInfo[j].disconMsg, "ERROR: Maximum users exceeded", sizeof(ClientInfo[j].disconMsg));
        ClientInfo[j].disconStat = DISCON_AFTER_MSG;
    }
    else
    {
        ClientInfo[j].activityTime = time(NULL);
    }
    setServerWorkPending(1);
}

static void procCtrlCmd(const int mpx, const char * const request, const int msgLen)
{
    string_t * response = NULL;
    int responseLen = CmdProc(request, msgLen, &response);
    setupClientResponse(ProgramName, mpx, response, responseLen);
    MemFree(ProgramName, &response);
}

static void *DbSendThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd(ProgramName, DbSendId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, DbSendId);

    DbStatus = DISCONNECTED;

    int ret;

    for (;;)
    {
        LogInfoF("%s: Connecting to database ...", ProgramName);
        for (;;)
        {
            ret = TcpSetConnectSocketNoBlock("localhost", "dbserver", &DbFd);
            if (ret == 0)
            {
                LogInfoF("%s: fd %d: Connected to database", ProgramName, DbFd);
                DbStatus = CONNECTED;
            }

            if (OrderlyShutdown || DbStatus == CONNECTED)
                break;

            sleep(30);
            LogInfoF("%s: Retrying connection to database ...", ProgramName);
        }

        if (OrderlyShutdown)
            break;

        PTHREAD_CREATE(&DbRecvId, NULL, DbRecvThread, NULL);

        setDbSendWorkPending(0);

        for (;;)
        {
            PTHREAD_MUTEX_LOCK(&DbSendMutex);
            while (!OrderlyShutdown && !DbSendWorkPending && DbStatus == CONNECTED)
            {
                PTHREAD_COND_WAIT(&DbSendCond, &DbSendMutex);
            }
            PTHREAD_MUTEX_UNLOCK(&DbSendMutex);

            if (OrderlyShutdown || DbStatus == DISCONNECTED)
                break;

            for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
            {
                if (MainPoll[i].fd == -1)
                    continue;
                if (ClientInfo[i].disconStat != DISCON_INIT)
                    continue;
                if (ClientInfo[i].procStat != PROCSTAT_DB_SEND)
                    continue;

                PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
                if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT && ClientInfo[i].procStat == PROCSTAT_DB_SEND)
                {
                    dbmsg_t dbmsgSend;

                    BufPrintf(dbmsgSend.id, SZ_DB_ID, "%0*d", SZ_DB_ID, MainPoll[i].fd);
                    memset(dbmsgSend.auditNo, '0', sizeof(dbmsgSend.auditNo));

                    StrToBuf("DS", ClientInfo[i].servermsg.status, SZ_SERVER_STATUS);

                    char sendStr[sizeof(dbmsg_t) + 1];
                    strlcpy(sendStr, (char *) &dbmsgSend, sizeof(sendStr));

                    MsgPrint(ProgramName, MainPoll[i].fd, "N", sendStr, sizeof(sendStr), "sending");

                    ret = SocketSendNoYield(DbFd, sendStr, sizeof(sendStr));
                    if (ret == 1)
                    {
                        DbStatus = DISCONNECTED;
                        break;
                    }

                    ClientInfo[i].procStat = PROCSTAT_DB_RECV; 
                    ClientInfo[i].activityTime = time(NULL);
                }
                PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
                break;
            }

            if (DbStatus == DISCONNECTED)
                break;

            setDbSendWorkPending(0);
        }

        if (OrderlyShutdown)
            break;

        sleep(30);    
    }

    if (DbFd != -1)
        (void) llshutdown(DbFd, SHUT_WR);
    
    (void) pthread_join(DbRecvId, NULL);

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static void *DbRecvThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd(ProgramName, DbRecvId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, DbRecvId);

    char * prevMsg = NULL;
    int prevMsgLen = 0;

    for (;;)
    {
        int ret;

        char * msg = NULL;
        int msgLen = 0;    
        ret = SocketRecvByClient(__func__, DbFd, "N", &msg, &msgLen, &prevMsg, &prevMsgLen);
        if (ret == 1)
        {
            MemFree(__func__, &msg);
            msgLen = 0;
            break;
        }    

        if (msgLen != sizeof(dbmsg_t) + 1)
        {
            LogErrorF("Invalid message from dbserver %.*s", msgLen, msg);
            SysExit(__func__, 0, INVALID_DB_MSG);
        }

        dbmsg_t dbmsgRecv;
        memcpy((char *) &dbmsgRecv, msg, msgLen - 1);

        for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
        {
            if (MainPoll[i].fd == -1)
                continue;
            if (ClientInfo[i].disconStat != DISCON_INIT)
                continue;
            if (ClientInfo[i].procStat != PROCSTAT_DB_RECV)
                continue;

            string_t dbIdStr[SZ_DB_ID + 1];
            snprintf(dbIdStr, sizeof(dbIdStr), "%06d", MainPoll[i].fd);
            if (memcmp(dbIdStr, dbmsgRecv.id, SZ_DB_ID))
                continue;    

            PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
            if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT && ClientInfo[i].procStat == PROCSTAT_DB_RECV)
            {
                MsgPrint(ProgramName, MainPoll[i].fd, "N", msg, msgLen, "receiving");

                servermsg_t * servermsg = &ClientInfo[i].servermsg;
                memcpy(servermsg->bankmsg.auditNo, dbmsgRecv.auditNo, SZ_BANK_AUDIT_NO);
                StrToBuf("DR", servermsg->status, SZ_SERVER_STATUS);

                ClientInfo[i].procStat = PROCSTAT_BANK_SEND;
                ClientInfo[i].activityTime = time(NULL);
            }
            PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
            break;
        }

        MemFree(__func__, &msg);
        msgLen = 0;

        setBankSendWorkPending(1);
    }

    MemFree(__func__, &prevMsg);
    prevMsgLen = 0;

    (void) llclose(DbFd);
    DbFd = -1;
    DbStatus = DISCONNECTED;

    (void) pthread_mutex_lock(&DbSendMutex);
    (void) pthread_cond_signal(&DbSendCond);
    (void) pthread_mutex_unlock(&DbSendMutex);

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static void setDbSendWorkPending(const bool_t sendSignal)
{
    PTHREAD_MUTEX_LOCK(&DbSendMutex);
    DbSendWorkPending = 0;
    for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
    {
        if (MainPoll[i].fd == -1)
            continue;
        if (ClientInfo[i].disconStat != DISCON_INIT)
            continue;
        if (ClientInfo[i].procStat != PROCSTAT_DB_SEND)
            continue;

        PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
        if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT && ClientInfo[i].procStat == PROCSTAT_DB_SEND)
        {
            DbSendWorkPending = 1;
            PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
            break;
        }
        PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
    }
    if (DbSendWorkPending && sendSignal)
        PTHREAD_COND_SIGNAL(&DbSendCond);

    PTHREAD_MUTEX_UNLOCK(&DbSendMutex);
}

static void *BankSendThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd(ProgramName, BankSendId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, BankSendId);

    BankStatus = DISCONNECTED;

    int ret;

    for (;;)
    {
        LogInfoF("%s: Connecting to bank ...", ProgramName);
        for (;;)
        {
            ret = TcpSetConnectSocketNoBlock("localhost", "bankserver", &BankFd);
            if (ret == 0)
            {
                LogInfoF("%s: fd %d: Connected to bank", ProgramName, BankFd);
                BankStatus = CONNECTED;
            }

            if (OrderlyShutdown || BankStatus == CONNECTED)
                break;

            sleep(30);
            LogInfoF("%s: Retrying connection to bank ...", ProgramName);
        }

        if (OrderlyShutdown)
            break;

        PTHREAD_CREATE(&BankRecvId, NULL, BankRecvThread, NULL);

        setBankSendWorkPending(0);

        for (;;)
        {
            PTHREAD_MUTEX_LOCK(&BankSendMutex);
            while (!OrderlyShutdown && !BankSendWorkPending && BankStatus == CONNECTED)
            {
                PTHREAD_COND_WAIT(&BankSendCond, &BankSendMutex);
            }
            PTHREAD_MUTEX_UNLOCK(&BankSendMutex);

            if (OrderlyShutdown || BankStatus == DISCONNECTED)
                break;

            for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
            {
                if (MainPoll[i].fd == -1)
                    continue;
                if (ClientInfo[i].disconStat != DISCON_INIT)
                    continue;
                if (ClientInfo[i].procStat != PROCSTAT_BANK_SEND)
                    continue;

                PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
                if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT && ClientInfo[i].procStat == PROCSTAT_BANK_SEND)
                {
                    StrToBuf("BS", ClientInfo[i].servermsg.status, SZ_SERVER_STATUS);
                    ret = xmitBankMsg(MainPoll[i].fd, (char *) &ClientInfo[i].servermsg.bankmsg, sizeof(bankmsg_t));
                    if (ret == 1)
                    {
                        BankStatus = DISCONNECTED;
                        break;
                    }
                    ClientInfo[i].procStat = PROCSTAT_BANK_RECV; 
                    ClientInfo[i].activityTime = time(NULL);
                }
                PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
                break;
            }

            if (BankStatus == DISCONNECTED)
                break;

            setBankSendWorkPending(0);
        }

        if (OrderlyShutdown)
            break;

        sleep(30);    
    }

    if (BankFd != -1)
        (void) llshutdown(BankFd, SHUT_WR);
    
    (void) pthread_join(BankRecvId, NULL);

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static int xmitBankMsg(const int fd, const char * const msg, const int msgLen)
{
    char sendMsg[SZ_SOCKETRW];

    int sendLen = MsgLenToXmitLen(StartInfo.msglenInd, msgLen);

    if (*StartInfo.msglenInd == MSG_WITH_NULL)
    {
        strlcpy(sendMsg, msg, sendLen);
    }
    else
    {
        MsgLenToBuf(StartInfo.msglenInd, msgLen, sendMsg);
        memcpy(sendMsg + SZ_MSGLEN_IN_MSG, msg, msgLen);
    }

    MsgPrint(ProgramName, fd, StartInfo.msglenInd, sendMsg, sendLen, "sending");

    return(SocketSendNoYield(BankFd, sendMsg, sendLen));
}

static void *BankRecvThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd(ProgramName, BankRecvId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, BankRecvId);

    char * prevMsg = NULL;
    int prevMsgLen = 0;

    for (;;)
    {
        int ret;
        char * msg = NULL;
        int msgLen = 0;    
        ret = SocketRecvByClient(__func__, BankFd, StartInfo.msglenInd, &msg, &msgLen, &prevMsg, &prevMsgLen);
        if (ret == 1)
        {
            MemFree(__func__, &msg);
            msgLen = 0;
            break;
        }    

        int actMsgLen = MsgLenFromXmitLen(StartInfo.msglenInd, msgLen);
        int startPos = MsgStartPos(StartInfo.msglenInd);

        if (actMsgLen != sizeof(bankmsg_t))
        {
            LogErrorF("Invalid message from bankserver %.*s (Total Len: %d, Msg Len: %d)", actMsgLen, msg + startPos, msgLen, actMsgLen);
            SysExit(__func__, 0, INVALID_BANK_MSG);
        }

        bankmsg_t bankmsgRecv;
        memcpy((char *) &bankmsgRecv, msg + startPos, sizeof(bankmsg_t));

        for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
        {
            if (MainPoll[i].fd == -1)
                continue;
            if (ClientInfo[i].disconStat != DISCON_INIT)
                continue;
            if (ClientInfo[i].procStat != PROCSTAT_BANK_RECV)
                continue;

            servermsg_t * servermsg = &ClientInfo[i].servermsg;

            if (memcmp(servermsg->bankmsg.auditNo, bankmsgRecv.auditNo, SZ_BANK_AUDIT_NO))
                continue;    

            PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
            if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT && ClientInfo[i].procStat == PROCSTAT_BANK_RECV)
            {
                MsgPrint(ProgramName, MainPoll[i].fd, StartInfo.msglenInd, msg, msgLen, "receiving");

                memcpy(servermsg->bankmsg.status, bankmsgRecv.status, SZ_BANK_STATUS);
                StrToBuf("BR", servermsg->status, SZ_SERVER_STATUS);
                setupClientResponse(ProgramName, i, (char *) &ClientInfo[i].servermsg, sizeof(servermsg_t));
            }
            PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
            break;
        }

        MemFree(__func__, &msg);
        msgLen = 0;
    }

    MemFree(__func__, &prevMsg);
    prevMsgLen = 0;

    (void) llclose(BankFd);
    BankFd = -1;
    BankStatus = DISCONNECTED;

    (void) pthread_mutex_lock(&BankSendMutex);
    (void) pthread_cond_signal(&BankSendCond);
    (void) pthread_mutex_unlock(&BankSendMutex);

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static void setBankSendWorkPending(const bool_t sendSignal)
{
    PTHREAD_MUTEX_LOCK(&BankSendMutex);
    BankSendWorkPending = 0;
    for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
    {
        if (MainPoll[i].fd == -1)
            continue;
        if (ClientInfo[i].disconStat != DISCON_INIT)
            continue;

        PTHREAD_MUTEX_LOCK(&ClientInfo[i].mutex);
        if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_INIT && ClientInfo[i].procStat == PROCSTAT_BANK_SEND)
        {
            BankSendWorkPending = 1;
            PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
            break;
        }
        PTHREAD_MUTEX_UNLOCK(&ClientInfo[i].mutex);
    }
    if (BankSendWorkPending && sendSignal)
        PTHREAD_COND_SIGNAL(&BankSendCond);

    PTHREAD_MUTEX_UNLOCK(&BankSendMutex);
}

static void setupClientResponse(constr_t caller, const int i, const char * const msg, const int msgLen)
{
    ClientInfo[i].responseLen = MsgLenToXmitLen("N", msgLen);

    ClientInfo[i].response = MemAlloc(caller, ClientInfo[i].responseLen);
    strlcpy(ClientInfo[i].response, msg, ClientInfo[i].responseLen);    
    MemFree(caller, &ClientInfo[i].request);
    ClientInfo[i].requestLen = 0;
    ClientInfo[i].procStat = PROCSTAT_CT_RESP_SEND;
    ClientInfo[i].activityTime = time(NULL);
    MainPoll[i].events = POLLOUT;

    uint64_t u = 1;
    int ret = write(MainPoll[EventFdMpx].fd, &u, sizeof(uint64_t));
    if (ret < 0) SysExit(__func__, errno, SYSCALL_WRITE);
}

static void disconnectClients(void)
{
    for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
    {
        if (MainPoll[i].fd != -1 && ClientInfo[i].disconStat == DISCON_NOW)
        {
            LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: Disconnecting client now (%s)", ProgramName, MainPoll[i].fd, 
                                        ClientInfo[i].disconMsg);
            closeClientAtIdx(i);
        }
    }
}

static void closeClientAtIdx(const int i)
{
    (void) llshutdown(MainPoll[i].fd, SHUT_WR);
    (void) llclose(MainPoll[i].fd);
    LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: Disconnected client mpx %d", ProgramName, MainPoll[i].fd, i);
    initClientAtIdx(i);
}

static void initClientAtIdx(const int i)
{
    MainPoll[i].fd = -1;
    MainPoll[i].events = 0;
    MainPoll[i].revents = 0;

    if (ClientInfo[i].requestLen > 0)
        MemFree("initClientAtIdx", &ClientInfo[i].request);
    if (ClientInfo[i].responseLen > 0)
        MemFree("initClientAtIdx", &ClientInfo[i].response);

    ClientInfo[i].disconStat = DISCON_INIT;
    *ClientInfo[i].disconMsg = 0;
    ClientInfo[i].procType = PROCTYPE_INIT;
    ClientInfo[i].procStat = PROCSTAT_INIT;
    ClientInfo[i].activityTime = 0;    
    ClientInfo[i].request = NULL;
    ClientInfo[i].requestLen = 0;
    memset(&ClientInfo[i].servermsg, ' ', sizeof(servermsg_t));
    ClientInfo[i].response = NULL;
    ClientInfo[i].responseLen = 0;
    ClientInfo[i].prevBytesSent = 0;

    setServerWorkPending(1);
}

static void endServer(void)
{
    (void) pthread_mutex_lock(&DbSendMutex);
    (void) pthread_cond_signal(&DbSendCond);
    (void) pthread_mutex_unlock(&DbSendMutex);
    (void) pthread_join(DbSendId, NULL);

    (void) pthread_mutex_lock(&BankSendMutex);
    (void) pthread_cond_signal(&BankSendCond);
    (void) pthread_mutex_unlock(&BankSendMutex);
    (void) pthread_join(BankSendId, NULL);

    (void) pthread_mutex_lock(&TimerThreadMutex);
    (void) pthread_cond_signal(&TimerThreadCond);
    (void) pthread_mutex_unlock(&TimerThreadMutex);
    (void) pthread_join(TimerThreadId, NULL);

    (void) pthread_kill(SignalHandlerId, SIGQUIT);
    (void) pthread_join(SignalHandlerId, NULL);

    (void) pthread_mutex_destroy(&DbSendMutex);
    (void) pthread_mutex_destroy(&BankSendMutex);
    (void) pthread_mutex_destroy(&TimerThreadMutex);
    (void) pthread_mutex_destroy(&ThrInfoMutex);

    (void) pthread_cond_destroy(&DbSendCond);
    (void) pthread_cond_destroy(&BankSendCond);
    (void) pthread_cond_destroy(&TimerThreadCond);

    (void) llclose(MainPoll[ListenFdMpx].fd);
    (void) llclose(MainPoll[EventFdMpx].fd);

    // Free lists
    ThrInfoFree(__func__);
    free(MainPoll);
    free(ClientInfo);

    // Remove lock file
    LockfileDelete(LockfileName);

    // Final message to log file
    LogInfo("Server ended");
    LogfileClose(&LogfileFd);
}

