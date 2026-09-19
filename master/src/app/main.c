// build/mysys 10 svcname &
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/resource.h>

#include <errno.h>

#include <person.h>

#include <errcodes.h>
#include <errmsgdefs.h>
#include <loglvldefs.h>
#include <strlstdefs.h>
#include <syssizes.h>

#include <csvrtns.h>
#include <daemonrtns.h>
#include <datmutils.h>
#include <errmsgrtns.h>
#include <filertns.h>
#include <fmtstr.h>
#include <lnklstrtns.h>
#include <lockfilertns.h>
#include <logcmds.h>
#include <logrtns.h>
#include <memrtns.h>
#include <memrtns.h>
#include <pthreadrtns.h>
#include <strutils.h>
#include <syscmd.h>
#include <sysexit.h>
#include <thrinfo.h>

// Threads involved in log rotation
#define NUM_BARRIER_THREADS 4

#define SIGEMPTYSET(set) ({ \
    if (sigemptyset(set)) \
        SysExit(__func__, errno, SYSCALL_SIGEMPTYSET); })
#define SIGADDSET(set, signum) ({ \
    if (sigaddset(set, signum)) \
        SysExit(__func__, errno, SYSCALL_SIGADDSET); })

typedef struct cmdqueue_s
{
    struct cmdqueue_s *next;
    pthread_mutex_t mutex;
    strlst_t *response;
    errmsg_t errmsg;
    int status;

    string_t cmd[];
} cmdqueue_t;

typedef struct
{
    pid_t pid;
    unsigned long maxUsers;
    string_t svcName[SZ_SERVICE_NAME + 1];
    string_t workingDir[SZ_FULL_PATH_NAME + 1];
    int rl;
} startInfo_t;

static void validateInput(const int argc, const char **argv, unsigned long *const maxUsers, string_t svcName[SZ_SERVICE_NAME + 1]);
static void showUsage(void);
static void setCurrentDir(string_t workingDir[SZ_FULL_PATH_NAME + 1]);
static void setFileLimit(const int limit);

static void initThreads(void);

static void *SignalHandlerThread(void *arg);
static void *CmdInputThread(void *arg);
static void *CmdOutputThread(void *arg);
static void *CmdProcessThread(void *arg);
static void *LogRotateThread(void *arg);

static bool_t ignoreSignal(int signo);

static void getCmd(void);
static void addCmdQueue(constr_t cmd, strlst_t **strlstHead, strlst_t **strlstTail);

static bool_t processCmdQueue(cmdqueue_t * const elem);
static void setCmdProcessWorkPending(const bool_t sendSignal);

static void showOutput(void);
static void setCmdOutputWorkPending(const bool_t sendSignal);

static void waitLogfileBarrierStart(void);
static void waitLogfileBarrierEnd(void);

static void openLogfile();
static void endServer(void);

static startInfo_t StartInfo;

static constr_t LockfileName = "/home/kerman/projects/GoogleTestC/cmdserver.lock";
static constr_t LogfileNamePrefix = "/home/kerman/projects/GoogleTestC/cmdserver.log";
static constr_t CmdfileName = "/home/kerman/projects/GoogleTestC/servercfg.txt";

static string_t LogfileName[SZ_FULL_PATH_NAME + 1];

static int LogfileFd = -1;

static atomic_int PendingOrderlyShutdown;
static atomic_int Shutdown;
static atomic_int LogfileRotate;

static pthread_t MainThreadId;

static pthread_t SignalHandlerId;

static pthread_t CmdInputId;
static pthread_cond_t CmdInputCond;
static pthread_mutex_t CmdInputMutex;
static atomic_int CmdInputWorkPending;

static pthread_t CmdOutputId;
static pthread_cond_t CmdOutputCond;
static pthread_mutex_t CmdOutputMutex;
static atomic_int CmdOutputWorkPending;

static pthread_t CmdProcessId;
static pthread_cond_t CmdProcessCond;
static pthread_mutex_t CmdProcessMutex;
static atomic_int CmdProcessWorkPending;

static pthread_t LogRotateId;
static pthread_mutex_t LogRotateMutex;

static pthread_mutex_t CmdQueueMutex;
static cmdqueue_t *CmdQueueHead = NULL;
static cmdqueue_t *CmdQueueTail = NULL;

static pthread_barrier_t LogfileBarrierStart;
static pthread_barrier_t LogfileBarrierEnd;

static pthread_mutex_t ThrInfoMutex;

int main(const int argc, const char **argv)
{
    LogLvlEnable(LOGLVL_INFO);
    SysSetPrintStack(1);

    // Validate input
    validateInput(argc, argv,  &StartInfo.maxUsers, StartInfo.svcName);

    // Check if the process is already running
    LockfileCheck(LockfileName);

    fclose(stdin);
    fclose(stdout);

    setCurrentDir(StartInfo.workingDir);

    int maxPoll = StartInfo.maxUsers + 10;

    StartInfo.rl = maxPoll + 10;
    setFileLimit(StartInfo.rl);

    DaemonizeServer();
    
    StartInfo.pid = getpid();

    LockfileWritePid(LockfileName, StartInfo.pid);

    // Open log file
    openLogfile();

    MainThreadId = pthread_self();
    LogInfoF("Main Thread Id %p", MainThreadId);

    // Block all signals
    sigset_t sigMask;
    if (sigfillset(&sigMask))
        SysExit(__func__, errno, SYSCALL_SIGFILLSET);
    if (pthread_sigmask(SIG_BLOCK, &sigMask, NULL))
        SysExit(__func__, errno, SYSCALL_PT_SIGMASK);

    // All threads will be created blocking all signals
    initThreads();

    // Allow SignalHandlerThread to send SIGPOLL
    sigset_t sigMaskUsr2;
    SIGEMPTYSET(&sigMaskUsr2);
    SIGADDSET(&sigMaskUsr2, SIGPOLL);

    for (;;)
    {
        int signum;
        int ret = sigwait(&sigMaskUsr2, &signum);
        if (ret)
            SysExit(__func__, errno, SYSCALL_SIGWAIT);

        if (PendingOrderlyShutdown)
        {
            Shutdown = 1;
            break;
        }

    }

    endServer();
    
    return(0);
}

static void validateInput(const int argc, const char **argv, unsigned long * const maxUsers, string_t svcName[SZ_SERVICE_NAME + 1])
{
    if (argc != 3)
    {
        showUsage();
        fprintf(stderr, "Invalid number of arguments\n");
        exit(1);
    }

    if (strnlen(argv[1], SZ_MAX_USERS + 1) == SZ_MAX_USERS + 1)
    {
        showUsage();
        fprintf(stderr, "Number of users too long (Maximum 2 digits)\n");
        exit(1);
    }
    *maxUsers = strtoul(argv[1], NULL, 0);
    if (errno == EINVAL || errno == ERANGE || *maxUsers == 0)
    {
        showUsage();
        fprintf(stderr, "Invalid maximum number of users (1-99)\n");
        exit(1);
    }

    if (strnlen(argv[2], SZ_SERVICE_NAME + 1) == SZ_SERVICE_NAME + 1)
    {
        showUsage();
        fprintf(stderr, "Service name too long (Maximum 10 characters)\n");
        exit(1);
    }

    strlcpy(svcName, argv[2], SZ_SERVICE_NAME + 1);
}

static void showUsage(void)
{
    fprintf(stderr, "Usage: mysys maxUsers serviceName\n");
    fprintf(stderr, "       maxUsers must be between 1-99\n");
    fprintf(stderr, "       serviceName must not exceed 10 characters\n");
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

static void setCurrentDir(string_t workingDir[SZ_FULL_PATH_NAME + 1])
{
    char *cwd = getcwd(NULL, SZ_FULL_PATH_NAME + 1);
    if (cwd == NULL)
        SysExit(__func__, errno, SYSCALL_GETCWD);

    strlcpy(workingDir, cwd, SZ_FULL_PATH_NAME + 1);
    free(cwd);
}

static void initThreads(void)
{
    PTHREAD_MUTEX_INIT(&ThrInfoMutex, NULL);
    PTHREAD_MUTEX_INIT(&CmdInputMutex, NULL);
    PTHREAD_MUTEX_INIT(&CmdOutputMutex, NULL);
    PTHREAD_MUTEX_INIT(&CmdProcessMutex, NULL);
    PTHREAD_MUTEX_INIT(&CmdQueueMutex, NULL);
    PTHREAD_MUTEX_INIT(&LogRotateMutex, NULL);

    PTHREAD_COND_INIT(&CmdInputCond, NULL);
    PTHREAD_COND_INIT(&CmdOutputCond, NULL);
    PTHREAD_COND_INIT(&CmdProcessCond, NULL);

    PTHREAD_CREATE(&SignalHandlerId, NULL, SignalHandlerThread, NULL);
    PTHREAD_CREATE(&CmdInputId, NULL, CmdInputThread, NULL);
    PTHREAD_CREATE(&CmdOutputId, NULL, CmdOutputThread, NULL);
    PTHREAD_CREATE(&CmdProcessId, NULL, CmdProcessThread, NULL);
}

static void *SignalHandlerThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd("CmdServer", SignalHandlerId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, SignalHandlerId);

    sigset_t sigMask;
    SIGEMPTYSET(&sigMask);
    SIGADDSET(&sigMask, SIGINT);
    SIGADDSET(&sigMask, SIGTERM);
    SIGADDSET(&sigMask, SIGHUP);
    SIGADDSET(&sigMask, SIGUSR1);
    SIGADDSET(&sigMask, SIGQUIT);

    for (;;)
    {
        siginfo_t sigInfo;
        int signo;
        signo = sigwaitinfo(&sigMask, &sigInfo);
        if (signo < 0)
            SysExit(__func__, errno, SYSCALL_SIGWAITINFO);

        if (signo == SIGINT)
            exit(1);

        if (signo == SIGTERM)
        {
            if (ignoreSignal(signo)) continue;
            PendingOrderlyShutdown = 1;
            if (pthread_kill(MainThreadId, SIGPOLL))
                SysExit(__func__, errno, SYSCALL_PT_KILL);
        }
        else if (signo == SIGUSR1)
        {
            if (ignoreSignal(signo)) continue;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            PTHREAD_CREATE(&LogRotateId, &attr, LogRotateThread, NULL);
            pthread_attr_destroy(&attr);
        }
        else if (signo == SIGHUP)
        {
            if (ignoreSignal(signo)) continue;
            PTHREAD_MUTEX_LOCK(&CmdInputMutex);
            CmdInputWorkPending = 1;
            PTHREAD_COND_SIGNAL(&CmdInputCond);
            PTHREAD_MUTEX_UNLOCK(&CmdInputMutex);
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
    if (CmdInputWorkPending || LnkLstCount(CmdQueueHead) > 0)
    {
        LogWarnF("'%s' signal ignored, commands being processed", strsignal(signo));
        return(1);                                        
    }
    if (LogfileRotate)
    {
        LogWarnF("'%s' signal ignored, log file being rotated", strsignal(signo));
        return(1);                                        
    }
    if (PendingOrderlyShutdown)
    {
        LogWarnF("'%s' signal ignored, shutdown requested", strsignal(signo));
        return(1);                                        
    }

    return(0);
}

static void *CmdInputThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd("CmdServer", CmdInputId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, CmdInputId);

    for (;;)
    {
        PTHREAD_MUTEX_LOCK(&CmdInputMutex);
        while (!CmdInputWorkPending && !Shutdown && !LogfileRotate)
        {
            PTHREAD_COND_WAIT(&CmdInputCond, &CmdInputMutex);
        }
        PTHREAD_MUTEX_UNLOCK(&CmdInputMutex);

        if (Shutdown)
            break;

        if (LogfileRotate)
        {
            waitLogfileBarrierStart();
            waitLogfileBarrierEnd();
        }    

        if (CmdInputWorkPending)
        {
            getCmd();
            CmdInputWorkPending = 0;
        }
    }

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static void getCmd(void)
{
    FILE *fPtr = fopen(CmdfileName, "r");
    if (fPtr == NULL)
    {
        LogErrorF("%s: %s open failed with errno %d (%s)", __func__, CmdfileName, errno, strerror(errno));
        return;
    }

    for (;;)
    {
        string_t cmd[SZ_SYSCMD_STR];
        if (!fgets(cmd, SZ_SYSCMD_STR - 1, fPtr))
        {
            if (ferror(fPtr))
                LogErrorF("%s: %s read failed", __func__, CmdfileName);
            (void) fclose(fPtr);
            break;
        }

        if (!*cmd || *cmd == '\n' || *cmd == '#') continue;

        StrRmTrailNewLine(cmd);

        if (!memcmp(cmd, "?log:", 5))
        {
            LogCmdsProc(cmd+5);
            LogInfoF("%s", LogLvlsInfo());
            continue;
        }
        if (!memcmp(cmd, "?log", 4))
        {
            LogInfoF("%s", LogLvlsInfo());
            continue;
        }

        strlst_t *strlstHead = NULL;
        strlst_t *strlstTail = NULL;
        addCmdQueue(cmd, &strlstHead, &strlstTail);
        LnkLstFree(__func__, &strlstHead);
    }
}

static void addCmdQueue(constr_t cmd, strlst_t **strlstHead, strlst_t **strlstTail)
{
    if (!memcmp(cmd, "?sys:", 5))
    {
        (void) CsvStrLst(cmd+5, strlstHead);
    }
    else
    {
        int len = strlen(cmd) + 1;
        strlst_t * strlstElem = MemAlloc(__func__, sizeof(strlst_t) + len);
        strlcpy(strlstElem->str, cmd, len);
        LnkLstAppend(strlstElem, strlstHead, strlstTail);
    }

    PTHREAD_MUTEX_LOCK(&CmdQueueMutex);

    for (strlst_t *strlstElem = *strlstHead; strlstElem; strlstElem = strlstElem->next)
    {
        int len = strlen(strlstElem->str) + 1;
        cmdqueue_t *elem = MemAlloc(__func__, sizeof(cmdqueue_t) + len);
        strlcpy(elem->cmd, strlstElem->str, len);
        PTHREAD_MUTEX_INIT(&elem->mutex, NULL);
        elem->response = NULL;
        elem->status = 0;
        ErrmsgInit(&elem->errmsg);
        LnkLstAppend(elem, &CmdQueueHead, &CmdQueueTail);

        setCmdProcessWorkPending(1);
    }

    PTHREAD_MUTEX_UNLOCK(&CmdQueueMutex);
}

static void *CmdProcessThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd("CmdServer", CmdProcessId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, CmdProcessId);

    for (;;)
    {
        PTHREAD_MUTEX_LOCK(&CmdProcessMutex);
        while (!CmdProcessWorkPending && !Shutdown && !LogfileRotate)
        {
            PTHREAD_COND_WAIT(&CmdProcessCond, &CmdProcessMutex);
        }
        PTHREAD_MUTEX_UNLOCK(&CmdProcessMutex);

        if (Shutdown)
            break;

        if (LogfileRotate)
        {
            waitLogfileBarrierStart();
            waitLogfileBarrierEnd();
        }    

        for (cmdqueue_t *elem = CmdQueueHead; elem; elem = elem->next)
        {
            if (elem->status == 0)
            {
                if (processCmdQueue(elem))
                    setCmdOutputWorkPending(1);
                break;    
            }
        }
        setCmdProcessWorkPending(0);
    }

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static bool_t processCmdQueue(cmdqueue_t * const elem)
{
    PTHREAD_MUTEX_LOCK(&elem->mutex);

    bool_t elemProcessed = 0;

    if (elem->status == 0)
    {
        elemProcessed = 1;    
        elem->status = 1;
        int ret = SysCmdStrLst(elem->cmd, &elem->response, &elem->errmsg);
        if (ret == 1)
            elem->status = 2;
        else
            elem->status = 3;
    }    
    PTHREAD_MUTEX_UNLOCK(&elem->mutex);
    return(elemProcessed);
}

static void setCmdProcessWorkPending(const bool_t sendSignal)
{
    PTHREAD_MUTEX_LOCK(&CmdProcessMutex);
    CmdProcessWorkPending = 0;
    for (cmdqueue_t *elem = CmdQueueHead; elem; elem = elem->next)
    {
        if (elem->status == 0)
        {
            CmdProcessWorkPending = 1;
            break;
        }
    }
    if (sendSignal)
        PTHREAD_COND_SIGNAL(&CmdProcessCond);

    PTHREAD_MUTEX_UNLOCK(&CmdProcessMutex);
}

static void *CmdOutputThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd("CmdServer", CmdOutputId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, CmdOutputId);    

    for (;;)
    {
        PTHREAD_MUTEX_LOCK(&CmdOutputMutex);
        while (!CmdOutputWorkPending && !Shutdown && !LogfileRotate)
        {
            PTHREAD_COND_WAIT(&CmdOutputCond, &CmdOutputMutex);
        }
        PTHREAD_MUTEX_UNLOCK(&CmdOutputMutex);

        if (Shutdown)
            break;

        if (LogfileRotate)
        {
            waitLogfileBarrierStart();
            waitLogfileBarrierEnd();
        }    

        showOutput();    

        setCmdOutputWorkPending(0);    
    }

    LogInfoF("%s ended", __func__);
    pthread_exit(NULL);
}

static void showOutput(void)
{
    PTHREAD_MUTEX_LOCK(&CmdQueueMutex);
        
    for (cmdqueue_t *elem = CmdQueueHead; elem; elem = elem->next)
    {
        bool_t elemProcessed = 0;
        if (elem->status == 2)
        {
            elemProcessed = 1;
            // Show error
            LogErrorF("Command %s failed", elem->cmd);
            LogErrorF("code: %d, msg: %s", elem->errmsg.code, elem->errmsg.message);
        }
        else if (elem->status == 3)
        {
            elemProcessed = 1;
            // Show output
            LogInfoF("Command %s succeeded", elem->cmd);
            for (strlst_t *resp = elem->response; resp; resp = resp->next)
            {
                LogInfo(StrRmTrailNewLine(resp->str));
            }
        }
        // Delete element
        if (elemProcessed)
        {
            PTHREAD_MUTEX_DESTROY(&elem->mutex);
            LnkLstFree(__func__, &elem->response);
            LnkLstRemove(__func__, &CmdQueueHead, &elem, &CmdQueueTail);
        }
    }

    PTHREAD_MUTEX_UNLOCK(&CmdQueueMutex);
}

static void setCmdOutputWorkPending(const bool_t sendSignal)
{
    PTHREAD_MUTEX_LOCK(&CmdOutputMutex);
    CmdOutputWorkPending = 0;
    for (cmdqueue_t *elem = CmdQueueHead; elem; elem = elem->next)
    {
        if (elem->status == 2 || elem->status == 3)
        {
            CmdOutputWorkPending = 1;
            break;
        }
    }

    if (sendSignal)
        PTHREAD_COND_SIGNAL(&CmdOutputCond);

    PTHREAD_MUTEX_UNLOCK(&CmdOutputMutex);
}

static void *LogRotateThread(void *arg)
{
    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoAdd("CmdServer", LogRotateId, __func__);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    LogInfoF("%s started Id %p", __func__, LogRotateId);

    PTHREAD_MUTEX_LOCK(&LogRotateMutex);

    PTHREAD_BARRIER_INIT(&LogfileBarrierStart, NULL, NUM_BARRIER_THREADS);
    PTHREAD_BARRIER_INIT(&LogfileBarrierEnd, NULL, NUM_BARRIER_THREADS);
    LogfileRotate = 1;

    PTHREAD_MUTEX_LOCK(&CmdInputMutex);
    PTHREAD_COND_SIGNAL(&CmdInputCond);
    PTHREAD_MUTEX_UNLOCK(&CmdInputMutex);

    PTHREAD_MUTEX_LOCK(&CmdProcessMutex);
    PTHREAD_COND_SIGNAL(&CmdProcessCond);
    PTHREAD_MUTEX_UNLOCK(&CmdProcessMutex);

    PTHREAD_MUTEX_LOCK(&CmdOutputMutex);
    PTHREAD_COND_SIGNAL(&CmdOutputCond);
    PTHREAD_MUTEX_UNLOCK(&CmdOutputMutex);

    waitLogfileBarrierStart();
    waitLogfileBarrierEnd();

    (void) pthread_barrier_destroy(&LogfileBarrierStart);
    (void) pthread_barrier_destroy(&LogfileBarrierEnd);

    PTHREAD_MUTEX_UNLOCK(&LogRotateMutex);

    LogInfoF("%s ended", __func__);

    PTHREAD_MUTEX_LOCK(&ThrInfoMutex);
    ThrInfoRemove("CmdServer", LogRotateId);
    PTHREAD_MUTEX_UNLOCK(&ThrInfoMutex);

    return(NULL);
}

static void waitLogfileBarrierStart(void)
{
    int ret = pthread_barrier_wait(&LogfileBarrierStart);
    if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD)
        SysExit(__func__, errno, SYSCALL_PT_BARRIER_WAIT);

    if (ret == PTHREAD_BARRIER_SERIAL_THREAD)
    {
        openLogfile();
        LogfileRotate = 0;
    }
}

static void waitLogfileBarrierEnd(void)
{
    int ret = pthread_barrier_wait(&LogfileBarrierEnd);
    if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD)
        SysExit(__func__, errno, SYSCALL_PT_BARRIER_WAIT);
}

static void openLogfile(void)
{
    string_t OldLogfileName[SZ_FULL_PATH_NAME + 1];
    string_t NewLogfileName[SZ_FULL_PATH_NAME + 1];
    bool_t rotate = 0;

    strlcpy(NewLogfileName, FmtStr("%s.%s", LogfileNamePrefix, DatmNowHmsFilename()), sizeof(NewLogfileName));
    if (LogfileFd != -1)
    {
        rotate = 1;
        strlcpy(OldLogfileName, LogfileName, sizeof(OldLogfileName));
        LogInfoF("New log file: %s", NewLogfileName);
        if (close(LogfileFd))
            SysExit(__func__, errno, SYSCALL_CLOSE);
        LogfileFd = -1;
    }

    strlcpy(LogfileName, NewLogfileName, sizeof(LogfileName));

    LogfileFd = open(LogfileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (LogfileFd < 0)
        SysExit(__func__, errno, SYSCALL_OPEN);

    // Redirect stderr to log file
    if (dup2(LogfileFd, STDERR_FILENO) < 0)
        SysExit(__func__, errno, SYSCALL_DUP2);

    if (rotate)
        LogInfoF("Old log file: %s", OldLogfileName);

    LogInfoF("Server pid %d", StartInfo.pid);
    LogInfoF("Max Users: %d", StartInfo.maxUsers);
    LogInfoF("Service Name: %s", StartInfo.svcName);
    LogInfoF("Current working directory: %s", StartInfo.workingDir);
    LogInfoF("Resource limit: %d", StartInfo.rl);
    LogInfoF("Log file: %s", LogfileName);
    
    if (rotate)
    {
        LogInfoF("Main Thread Id %p", MainThreadId);
        LogInfoF("SignalHandlerThread Id %p", SignalHandlerId);
        LogInfoF("CmdInputThread Id %p", CmdInputId);
        LogInfoF("CmdProcessThread Id %p", CmdProcessId);
        LogInfoF("CmdOutputThread Id %p", CmdOutputId);
    }
}

static void endServer(void)
{
    // End threads and destroy thread related data
    (void) pthread_mutex_lock(&CmdInputMutex);
    (void) pthread_cond_signal(&CmdInputCond);
    (void) pthread_mutex_unlock(&CmdInputMutex);
    (void) pthread_join(CmdInputId, NULL);

    (void) pthread_mutex_lock(&CmdOutputMutex);
    (void) pthread_cond_signal(&CmdOutputCond);
    (void) pthread_mutex_unlock(&CmdOutputMutex);
    (void) pthread_join(CmdOutputId, NULL);

    (void) pthread_mutex_lock(&CmdProcessMutex);
    (void) pthread_cond_signal(&CmdProcessCond);
    (void) pthread_mutex_unlock(&CmdProcessMutex);
    (void) pthread_join(CmdProcessId, NULL);

    // Wait for log rotation (if any) to complete
    (void) pthread_mutex_lock(&LogRotateMutex);
    (void) pthread_mutex_unlock(&LogRotateMutex);

    (void) pthread_kill(SignalHandlerId, SIGQUIT);
    (void) pthread_join(SignalHandlerId, NULL);

    (void) pthread_mutex_destroy(&CmdInputMutex);
    (void) pthread_mutex_destroy(&CmdOutputMutex);
    (void) pthread_mutex_destroy(&CmdProcessMutex);
    (void) pthread_mutex_destroy(&CmdQueueMutex);
    (void) pthread_mutex_destroy(&ThrInfoMutex);
    (void) pthread_mutex_destroy(&LogRotateMutex);

    (void) pthread_cond_destroy(&CmdInputCond);
    (void) pthread_cond_destroy(&CmdOutputCond);
    (void) pthread_cond_destroy(&CmdProcessCond);

    // Free lists
    for (cmdqueue_t *elem = CmdQueueHead; elem; elem = elem->next)
    {
        (void) pthread_mutex_destroy(&elem->mutex);
        LnkLstFree(__func__, &elem->response);
    }
    LnkLstFree(__func__, &CmdQueueHead);
    ThrInfoFree(__func__);

    // Remove lock file
    LockfileDelete(LockfileName);

    // Final message to log file
    LogInfo("Server ended");
    (void) close(LogfileFd);
}

