// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

typedef struct
{
    int errCode;
    char *desc;
} errcodedesc_t;

static errcodedesc_t ErrCodeDescArr[] =
{
    { INVALID_PERSON_REC, "INVALID_PERSON_REC" },
    { INVALID_PMTTXN_REC, "INVALID_PMTTXN_REC" },
    { INVALID_RECPRM_REC, "INVALID_RECPRM_REC" },
    { INVALID_RECPRM_ARCHIVE, "INVALID_RECPRM_ARCHIVE" },
    { INVALID_RECPRM_EMAIL, "INVALID_RECPRM_EMAIL" },
    { RECEIPT_ANOTHER_USER, "RECEIPT_ANOTHER_USER" },
    { SYSCMD_FAILED, "SYSCMD_FAILED" },
    { STR_SIZE_EXCEEDED, "STR_SIZE_EXCEEDED" },
    { STR_QUOTES_INVALID, "STR_QUOTES_INVALID" },
    { LENGTH_INVALID, "LENGTH_INVALID" },
    { MSGLEN_IND_INVALID, "MSGLEN_IND_INVALID" },
    { INVALID_CLIENT_FD, "INVALID_CLIENT_FD" },
    { INVALID_LOGPKG, "INVALID_LOGPKG" },
    { INVALID_HEX_CHAR, "INVALID_HEX_CHAR" },
    { INVALID_DB_MSG, "INVALID_DB_MSG" },
    { INVALID_BANK_MSG, "INVALID_BANK_MSG" },
    { SYSCALL_POPEN, "SYSCALL_POPEN" },
    { SYSCALL_FGETS, "SYSCALL_FGETS" },
    { SYSCALL_PCLOSE, "SYSCALL_PCLOSE" },
    { SYSCALL_STAT, "SYSCALL_STAT" },
    { SYSCALL_VSNPRINTF, "SYSCALL_VSNPRINTF" },
    { SYSCALL_FOPEN, "SYSCALL_FOPEN" },
    { SYSCALL_FPRINTF, "SYSCALL_FPRINTF" },
    { SYSCALL_VFPRINTF, "SYSCALL_VFPRINTF" },
    { SYSCALL_FPUTC, "SYSCALL_FPUTC" },
    { SYSCALL_FPUTS, "SYSCALL_FPUTS" },
    { SYSCALL_FCLOSE, "SYSCALL_FCLOSE" },
    { SYSCALL_MALLOC, "SYSCALL_MALLOC" },
    { SYSCALL_REALLOC, "SYSCALL_REALLOC" },
    { SYSCALL_FORK, "SYSCALL_FORK" },
    { SYSCALL_SETSID, "SYSCALL_SETSID" },
    { SYSCALL_CLOSE, "SYSCALL_CLOSE" },
    { SYSCALL_GETRLIMIT, "SYSCALL_GETRLIMIT" },
    { SYSCALL_SETRLIMIT, "SYSCALL_SETRLIMIT" },
    { SYSCALL_GETCWD, "SYSCALL_GETCWD" },
    { SYSCALL_OPEN, "SYSCALL_OPEN" },
    { SYSCALL_DUP2, "SYSCALL_DUP2" },
    { SYSCALL_UNLINK, "SYSCALL_UNLINK" },
    { SYSCALL_SOCKET, "SYSCALL_SOCKET" },
    { SYSCALL_BIND, "SYSCALL_BIND" },
    { SYSCALL_LISTEN, "SYSCALL_LISTEN" },
    { SYSCALL_ACCEPT, "SYSCALL_ACCEPT" },
    { SYSCALL_INET_PTON, "SYSCALL_INET_PTON" },
    { SYSCALL_SEND, "SYSCALL_SEND" },
    { SYSCALL_RECV, "SYSCALL_RECV" },
    { SYSCALL_POLL, "SYSCALL_POLL" },
    { SYSCALL_CALLOC, "SYSCALL_CALLOC" },
    { SYSCALL_SHUTDOWN, "SYSCALL_SHUTDOWN" },
    { SYSCALL_EVENTFD, "SYSCALL_EVENTFD" },
    { SYSCALL_READ, "SYSCALL_READ" },
    { SYSCALL_WRITE, "SYSCALL_WRITE" },
    { SYSCALL_GETADDRINFO, "SYSCALL_GETADDRINFO" },
    { SYSCALL_CONNECT, "SYSCALL_CONNECT" },
    { SYSCALL_FCNTL, "SYSCALL_FCNTL" },
    { SYSCALL_GETSOCKOPT, "SYSCALL_GETSOCKOPT" },
    { SYSCALL_CRYPT_GENSALT_RA, "SYSCALL_CRYPT_GENSALT_RA" },
    { SYSCALL_CRYPT_RA, "SYSCALL_CRYPT_RA" },
    { SYSCALL_PT_CREATE, "SYSCALL_PT_CREATE" },
    { SYSCALL_PT_MUTEX_INIT, "SYSCALL_PT_MUTEX_INIT" },
    { SYSCALL_PT_MUTEX_DESTROY, "SYSCALL_PT_MUTEX_DESTROY" },
    { SYSCALL_PT_MUTEX_LOCK, "SYSCALL_PT_MUTEX_LOCK" },
    { SYSCALL_PT_MUTEX_UNLOCK, "SYSCALL_PT_MUTEX_UNLOCK" },
    { SYSCALL_PT_COND_INIT, "SYSCALL_PT_COND_INIT" },
    { SYSCALL_PT_COND_WAIT, "SYSCALL_PT_COND_WAIT" },
    { SYSCALL_PT_COND_SIGNAL, "SYSCALL_PT_COND_SIGNAL" },
    { SYSCALL_PT_BARRIER_INIT, "SYSCALL_PT_BARRIER_INIT" },
    { SYSCALL_PT_BARRIER_WAIT, "SYSCALL_PT_BARRIER_WAIT" },
    { SYSCALL_PT_SIGMASK, "SYSCALL_PT_SIGMASK" },
    { SYSCALL_PT_KILL, "SYSCALL_PT_KILL" },
    { SYSCALL_SIGACTION, "SYSCALL_SIGACTION" },
    { SYSCALL_SIGFILLSET, "SYSCALL_SIGFILLSET" },
    { SYSCALL_SIGEMPTYSET, "SYSCALL_SIGEMPTYSET" },
    { SYSCALL_SIGADDSET, "SYSCALL_SIGADDSET" },
    { SYSCALL_SIGWAIT, "SYSCALL_SIGWAIT" },
    { SYSCALL_SIGWAITINFO, "SYSCALL_SIGWAITINFO" },
    { 0, NULL }
};

// --------------------------------------------------------------------
// NOTE: Do not include any of the internal libraries. These could call
// back into SysExit() creating a loop.
// NOTE: None of the system calls made in this module should be mocked
// i.e. do no call any of the ll...() functions.
// --------------------------------------------------------------------

#include <sysexit.h>

static bool_t PrintStack;
static bool_t ExitInProgress;

static void printErrCode(const int errCode);

// --------------------------------------------------------------------
// Exit system with passed errors.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/sysexit.cpp
// --------------------------------------------------------------------

void SysExit(constr_t functionName, const int sysErrNo, const int errCode)
{
    if (ExitInProgress) return;

    ExitInProgress = 1;

    fprintf(stderr, "Function: %s\n", functionName);
    fprintf(stderr, "    Error code: %d\n", errCode);
    printErrCode(errCode);
    if (sysErrNo)
        fprintf(stderr, "    errno: %d [%s]\n", sysErrNo, strerror(sysErrNo));
    if (PrintStack) SysPstack();    
    exit(1);
}

static void printErrCode(const int errCode)
{
    for (errcodedesc_t *ecd = ErrCodeDescArr; ecd->desc; ecd++)
    {
        if (ecd->errCode == errCode)
        {
            fprintf(stderr, "    Error desc: %s\n", ecd->desc);
            return;
        }
    }

    fprintf(stderr, "    Error desc: No description for error code %d\n", errCode);
}

void SysSetPrintStack(const bool_t value)
{
    PrintStack = value;
}

void SysPstack(void)
{
    int pid = getpid();

    string_t cmd[SZ_SYSCMD_STR];
    snprintf(cmd, sizeof(cmd), "echo \"set pagination off\\nthread apply all backtrace\\nquit\\n\" | gdb --pid %d", pid);

    FILE * fPtr = popen(cmd, "r");
    if (fPtr == NULL)
        return;

    for (;;)
    {
        string_t message[SZ_SYSCMD_STR];
        const string_t * result = fgets(message, SZ_SYSCMD_STR, fPtr);
        if (result == NULL) break;
        fprintf(stderr, "%s", result);
    }
    (void) pclose(fPtr);
}

