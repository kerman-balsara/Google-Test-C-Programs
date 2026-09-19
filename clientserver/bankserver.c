#include <errno.h>

#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/eventfd.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <bankmsgdefs.h>
#include <errcodes.h>
#include <loglvldefs.h>
#include <logpkgdefs.h>
#include <msglenind.h>
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
#include <lnklstrtns.h>
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

#define SIGEMPTYSET(set) ({ \
    if (sigemptyset(set)) \
        SysExit(__func__, errno, SYSCALL_SIGEMPTYSET); })
#define SIGADDSET(set, signum) ({ \
    if (sigaddset(set, signum)) \
        SysExit(__func__, errno, SYSCALL_SIGADDSET); })

typedef struct
{
    bool_t disconnect;
    char *request;
    int requestLen;
    char *prevMsg;
    int prevMsgLen;
} clientinfo_t;

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

typedef struct procthr_s
{
    struct procthr_s *next;

    unsigned int id;

    int fd;
    int mpx;
    bool_t closeClient;
    string_t msglenInd[SZ_MSGLEN_IND + 1];
    char * request;
    int requestLen;
} procthrlst_t;

static procthrlst_t * ProcThrLstHead;
static procthrlst_t * ProcThrLstTail;

static struct pollfd *MainPoll;
static clientinfo_t *ClientInfo;

static pthread_t MainThreadId;
static pthread_t SignalHandlerId;

static pthread_mutex_t ThrInfoMutex;
static pthread_mutex_t SendMutex;
static pthread_mutex_t ProcThrLstMutex;

static startInfo_t StartInfo;

static atomic_int OrderlyShutdown;

static atomic_int NextId;

static constr_t ProgramName = "bankserver";
static constr_t LockfileName = "/home/kerman/projects/GoogleTestC/bankserver.lock";
static constr_t LogfileNamePrefix = "/home/kerman/projects/GoogleTestC/bankserver.log";
static string_t LogfileName[SZ_FULL_PATH_NAME];
static int LogfileFd = -1;

static void validateInput(const int argc, const char **argv,
                            unsigned long * const maxUsers,
                            string_t msglenInd[SZ_MSGLEN_IND + 1]);
static void showUsage(void);

static void setCurrentDir(string_t workingDir[SZ_FULL_PATH_NAME + 1]);
static void setFileLimit(const int limit);
static void writeLogfileHeaders(void);

static void initThreads(void);

static void *SignalHandlerThread(void *arg);
static bool_t ignoreSignal(int signo);

static void procNewClient(const int listenFd, struct sockaddr_in * sockaddr, const int * const sockaddr_len);
static void *ProcessorThread(void *arg);
static void *endProcessorThread(const pthread_t threadId, procthrlst_t * const elem);
static void procCtrlCmd(const char * const msg, const int msgLen, const procthrlst_t * const elem);
static void setupResponse(constr_t caller, const char * const msg, const int msgLen, const procthrlst_t * const elem);

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
    validateInput(argc, argv, &StartInfo.maxUsers, StartInfo.msglenInd);

    strlcpy(StartInfo.svcName, ProgramName, sizeof(StartInfo.svcName));

    // Check if the process is already running
    LockfileCheck(LockfileName);

    fclose(stdin);
    fclose(stdout);

    setCurrentDir(StartInfo.workingDir);

    // We will be polling the ListenFd, EventFd, and the user fds
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
    if (eventFd == -1) SysExit(__func__, errno, SYSCALL_EVENTFD);

    MainPoll = calloc(StartInfo.maxPoll, sizeof(struct pollfd));
    if (MainPoll == NULL) SysExit(__func__, errno, SYSCALL_CALLOC);
    ClientInfo = calloc(StartInfo.maxPoll, sizeof(clientinfo_t));
    if (ClientInfo == NULL) SysExit(__func__, errno, SYSCALL_CALLOC);

    for (int i = ListenFdMpx; i < StartInfo.maxPoll; i ++)
        initClientAtIdx(i);

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

            LogPkgTrace(LOGPKG_WIRE, "--------------------------");
            LogPkgTraceF(LOGPKG_WIRE, "Processing mpx: %d, fd: %d", mpx, MainPoll[mpx].fd);

            if (MainPoll[mpx].revents & POLLIN)
            {
                ret = SocketRecvByServer(ProgramName, MainPoll[mpx].fd, &ClientInfo[mpx].prevMsg, &ClientInfo[mpx].prevMsgLen);
                if (ret == 1)
                {
                    LogPkgTrace(LOGPKG_WIRE, "  Disconnect received");
                    closeClientAtIdx(mpx);
                    continue;
                }

                for (;;)
                {
                    bool_t msgComplete;
                    MsgGetComplete(ProgramName, StartInfo.msglenInd, &ClientInfo[mpx].request, &ClientInfo[mpx].requestLen,
                                   &ClientInfo[mpx].prevMsg, &ClientInfo[mpx].prevMsgLen, &msgComplete);

                    if (!msgComplete)
                        break;

                    procthrlst_t * elem = MemAlloc(ProgramName, sizeof(procthrlst_t));
                    elem->id = ++NextId;

                    elem->fd = MainPoll[mpx].fd;
                    elem->mpx = mpx;
                    strlcpy(elem->msglenInd, StartInfo.msglenInd, sizeof(elem->msglenInd));
                    elem->request = ClientInfo[mpx].request;
                    elem->requestLen = ClientInfo[mpx].requestLen;

                    ClientInfo[mpx].request = NULL;
                    ClientInfo[mpx].requestLen = 0;

                    PTHREAD_MUTEX_LOCK(&ProcThrLstMutex);
                    LnkLstAppend(elem, &ProcThrLstHead, &ProcThrLstTail);
                    PTHREAD_MUTEX_UNLOCK(&ProcThrLstMutex);

                    pthread_t processorId;            
                    pthread_attr_t attr;
                    pthread_attr_init(&attr);
                    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                    PTHREAD_CREATE(&processorId, &attr, ProcessorThread, elem);
                    pthread_attr_destroy(&attr);
                }
            }
        }

        if (OrderlyShutdown)
            break;
    }

    for (int mpx = ClientFdMpx; mpx < StartInfo.maxPoll; mpx++)
    {
        if (MainPoll[mpx].fd == -1)
            continue;
        closeClientAtIdx(mpx);
    }

    endServer();
    return(0);
}

static void validateInput(const int argc, const char **argv,
                            unsigned long * const maxUsers,
                            string_t msglenInd[SZ_MSGLEN_IND + 1])
{
    if (argc != 3)
    {
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

    if (strnlen(argv[2], SZ_MSGLEN_IND + 1) == SZ_MSGLEN_IND + 1)
    {
        showUsage();
        fprintf(stderr, "Message length indicator too long (Maximum 1 character)\n");
        exit(1);
    }
    strlcpy(msglenInd, argv[2], SZ_MSGLEN_IND + 1);
    if (*msglenInd != MSG_WITH_NULL && *msglenInd != MSG_WITH_TOTLEN && *msglenInd != MSG_WITH_MSGLEN)
    {
        showUsage();
        fprintf(stderr, "Invalid message length indicator (N, T or M)\n");
        exit(1);
    }
}

static void showUsage(void)
{
    fprintf(stderr, "Usage: bankserver maxUsers msglenInd\n");
    fprintf(stderr, "       maxUsers must be between 1-99\n");
    fprintf(stderr, "       msglenInd must not exceed 1 character (N, T or M)\n");
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
    LogInfoF("Message length Indicator: %s",
        *StartInfo.msglenInd == MSG_WITH_NULL ? "Messages must be null-terminated" :
        *StartInfo.msglenInd == MSG_WITH_TOTLEN ? "Messages must include total length" :
            "Messages must include the message length");
    LogInfoF("Current working directory: %s", StartInfo.workingDir);
    LogInfoF("Resource limit: %d", StartInfo.rl);
    LogInfoF("Log file: %s", LogfileName);
}

static void initThreads(void)
{
    PTHREAD_MUTEX_INIT(&ThrInfoMutex, NULL);
    PTHREAD_MUTEX_INIT(&SendMutex, NULL);
    PTHREAD_MUTEX_INIT(&ProcThrLstMutex, NULL);
    PTHREAD_CREATE(&SignalHandlerId, NULL, SignalHandlerThread, NULL);
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
            // If we connect to the server using gdb and then quit gdb, an
            // interrupt is generated. Ignore it.
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
            LogPkgTraceF(LOGPKG_WIRE, "New client mpx: %d, fd: %d", j, clientFd);

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
        closeClientAtIdx(j);
}

static void *ProcessorThread(void *arg)
{
    pthread_t threadId = pthread_self();

    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd(ProgramName, threadId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, threadId);

    procthrlst_t * elem = (procthrlst_t *) arg;

    MsgPrint(ProgramName, elem->fd, elem->msglenInd, elem->request, elem->requestLen, "receiving");

    int msgLen = MsgLenFromXmitLen(elem->msglenInd, elem->requestLen);
    int startPos = MsgStartPos(elem->msglenInd);

    const char *msg = elem->request + startPos;

    if (*msg == '?')
    {
        procCtrlCmd(msg, msgLen, elem);
        return(endProcessorThread(threadId, elem));
    }
        
    if (msgLen != sizeof(bankmsg_t))
    {
        elem->closeClient = 1;
        return(endProcessorThread(threadId, elem));
    }

    // Set up response
    bankmsg_t bankmsg;
    memcpy(&bankmsg, msg, msgLen);

    string_t ccNo[SZ_CC_NO_MAX_LEN + 1];
    BufToStr(bankmsg.ccNo, SZ_CC_NO_MAX_LEN, ccNo);
    if (IsValidCreditCardNo(ccNo))
        memcpy(bankmsg.status, "00", sizeof(bankmsg.status));
    else
        memcpy(bankmsg.status, "02", sizeof(bankmsg.status));

    setupResponse(ProgramName, (char *) &bankmsg, sizeof(bankmsg_t), elem);
    return(endProcessorThread(threadId, elem));
}

static void procCtrlCmd(const char * const msg, const int msgLen, const procthrlst_t * const elem)
{
    string_t * response = NULL;
    int responseLen = CmdProc(msg, msgLen, &response);
    setupResponse(ProgramName, response, responseLen, elem);
    MemFree(ProgramName, &response);
}

static void setupResponse(constr_t caller, const char * const msg, const int msgLen, const procthrlst_t * const elem)
{
    int responseLen = MsgLenToXmitLen(elem->msglenInd, msgLen);
    char response[responseLen];
    if (*elem->msglenInd == MSG_WITH_NULL)
    {
        strlcpy(response, msg, responseLen);    
    }
    else
    {
        MsgLenToBuf(elem->msglenInd, msgLen, response);
        memcpy(response + SZ_MSGLEN_IN_MSG, msg, msgLen);
    }

    MsgPrint(ProgramName, elem->fd, StartInfo.msglenInd, response, responseLen, "sending");
    PTHREAD_MUTEX_LOCK(&SendMutex);
    SocketSendNoYield(elem->fd, response, responseLen);
    PTHREAD_MUTEX_UNLOCK(&SendMutex);
}

static void *endProcessorThread(const pthread_t threadId, procthrlst_t * const elem)
{
    bool_t closeClient = elem->closeClient;
    int mpx = elem->mpx;

    MemFree(ProgramName, &elem->request);
    elem->requestLen= 0;

    PTHREAD_MUTEX_LOCK(&ProcThrLstMutex);
    procthrlst_t * findElem;
    for (findElem = ProcThrLstHead; findElem; findElem = findElem->next)
    {
        if (elem->id == findElem->id)
        {
            LnkLstRemove(ProgramName, &ProcThrLstHead, &findElem, &ProcThrLstTail);
            break;
        }
    }
    PTHREAD_MUTEX_UNLOCK(&ProcThrLstMutex);

    LogInfoF("Thread %p ended", threadId);

    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoRemove(ProgramName, threadId);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    if (closeClient)
    {
        int ret;
        ClientInfo[mpx].disconnect = 1;

        uint64_t u = 1;
        ret = write(MainPoll[EventFdMpx].fd, &u, sizeof(uint64_t));
        if (ret < 0) SysExit(__func__, errno, SYSCALL_WRITE);
    }

    return(NULL);
}

static void disconnectClients(void)
{
    for (int i = ClientFdMpx; i < StartInfo.maxPoll; i++)
    {
        if (MainPoll[i].fd != -1 && ClientInfo[i].disconnect)
        {
            LogPkgTrace(LOGPKG_WIRE, "  Disconnecting client");
            closeClientAtIdx(i);
        }
    }
}

static void closeClientAtIdx(const int i)
{
    LogPkgTraceF(LOGPKG_WIRE, "  Closing mpx: %d, fd: %d", i, MainPoll[i].fd);
    (void) llclose(MainPoll[i].fd);
    initClientAtIdx(i);
}

static void initClientAtIdx(const int i)
{
    MainPoll[i].fd = -1;
    MainPoll[i].events = 0;
    MainPoll[i].revents = 0;

    if (ClientInfo[i].requestLen > 0)
        MemFree("initClientAtIdx", &ClientInfo[i].request);
    if (ClientInfo[i].prevMsgLen > 0)
        MemFree("initClientAtIdx", &ClientInfo[i].prevMsg);

    ClientInfo[i].request = NULL;
    ClientInfo[i].requestLen = 0;
    ClientInfo[i].prevMsg = NULL;
    ClientInfo[i].prevMsgLen = 0;
}

static void endServer(void)
{
    (void) pthread_kill(SignalHandlerId, SIGQUIT);
    (void) pthread_join(SignalHandlerId, NULL);

    (void) pthread_mutex_destroy(&ThrInfoMutex);
    (void) pthread_mutex_destroy(&SendMutex);
    (void) pthread_mutex_destroy(&ProcThrLstMutex);

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

