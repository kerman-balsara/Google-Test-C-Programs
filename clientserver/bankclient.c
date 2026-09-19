#include <errno.h>

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <bankmsgdefs.h>
#include <errcodes.h>
#include <msglenind.h>
#include <typedefs.h>

#include <llunistd.h>
#include <memrtns.h>
#include <msglenrtns.h>
#include <tcprtns.h>
#include <socketrecv.h>
#include <socketsend.h>
#include <strutils.h>
#include <sysexit.h>

// Fire off multiple automatic requests to the server
#define MAX_MESSAGES 9
// Accept a single request from the user to be sent to the server
#define SZ_USER_INPUT 50

typedef struct
{
    unsigned long maxClients;
    string_t svcName[SZ_SERVICE_NAME + 1];
    string_t msglenInd[SZ_MSGLEN_IND + 1];
} startInfo_t;

static startInfo_t StartInfo;

static struct pollfd *MainPoll;

static constr_t ProgramName = "bankclient";

typedef struct
{
    char *response;
    int responseLen;
} clientinfo_t;

static clientinfo_t *ClientInfo;

static void showUsage(void);
static void validateInput(const int argc, const char **argv,
                            string_t msglenInd[SZ_MSGLEN_IND + 1]);

static void doCalloc(void);
static void doConnect(const int i, int * const sockFd);
static int xmitMsg(const int i, const int fd, const char *const msg, const int msgLen);
static int endClient(int * const sockFd);

int main(const int argc, const char **argv)
{
    int ret;

    // Validate input
    validateInput(argc, argv, StartInfo.msglenInd);

    StartInfo.maxClients = 1;
    strlcpy(StartInfo.svcName, "bankserver", sizeof(StartInfo.svcName));

    doCalloc();

    int sockFd;
    doConnect(0, &sockFd);

    // Fire off multiple automatic requests to the server
    for (int i = 0; i <= MAX_MESSAGES; i++)
    {
        bankmsg_t bankmsg;
        memset(&bankmsg, ' ', sizeof(bankmsg_t));
        memset(bankmsg.auditNo, '0', sizeof(bankmsg.auditNo));
        memcpy(bankmsg.ccNo, "411111111111111", 15);
        bankmsg.ccNo[15] = i + '0';
        ret = xmitMsg(0, sockFd, (char *) &bankmsg, sizeof(bankmsg_t));
        if (ret == 1)
        {
            return(endClient(&sockFd));
        }
    }

    // Wait (done by SocketRecvByClient()) and show responses from the server
    bool_t disconnect = 0;

    char *prevMsg = NULL;
    int prevMsgLen = 0;

    int i = 0;
    for (;;)
    {
        char *response = NULL;
        int responseLen = 0;

        ret = SocketRecvByClient(ProgramName, sockFd, StartInfo.msglenInd, &response,
                            &responseLen, &prevMsg, &prevMsgLen);
        if (ret == 1)
        {
            disconnect = 1;
            break;
        }

        int msgLen = MsgLenFromXmitLen(StartInfo.msglenInd, responseLen);
        int startPos = MsgStartPos(StartInfo.msglenInd);

        fprintf(stderr, "CLIENT %d Message %d bytes (receiving %d bytes)\n", sockFd, msgLen, responseLen);
        fprintf(stderr, "  %.*s\n", msgLen, response + startPos);

        MemFree(ProgramName, &response);
        responseLen = 0;
        i++;

        if (i > MAX_MESSAGES) break;
    }

    MemFree(ProgramName, &prevMsg);
    prevMsgLen = 0;

    if (disconnect)
        return(endClient(&sockFd));

    printf("Entering interactive mode\n\n");

    for (;;)
    {
        printf("-------------------------------\n\n");
        printf("Bank Server message: AAAAAABBBBBBBBBBBBBBBBBBBCC\n");
        printf("A: Audit Number, B: CC Number, C: Status Code\n");
        printf("A,B are the request parameters, C is the response\n");

        printf("Enter command (quit or leave empty to exit)\n> ");
        char cmd[SZ_USER_INPUT];
        const char *result = fgets(cmd, sizeof(cmd), stdin);
        if (result == NULL || *result == '\n')
            break;

        if (cmd[strlen(cmd) - 1] != '\n')
        {
            // Ignore input larger than SZ_USER_INPUT. Empty the stream so that next call to fgets
            // can process a new command.  
            int ch;
            while((ch = getc(stdin)) != EOF && ch != '\n');
        }    
        StrRmTrailNewLine(cmd);
        if (!strcmp(cmd, "quit"))
            break;

        ret = xmitMsg(0, sockFd, cmd, strlen(cmd));
        if (ret == 1)
            break;

        disconnect = 0;

        for (;;)
        {
            MainPoll[0].revents = 0;

            ret = poll(MainPoll, StartInfo.maxClients, -1);
            if (ret < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                SysExit(__func__, errno, SYSCALL_POLL);
            }

            int mpx = 0;

            bool_t msgComplete;
            ret = SocketRecv(ProgramName, MainPoll[mpx].fd, StartInfo.msglenInd, &ClientInfo[mpx].response,
                                &ClientInfo[mpx].responseLen, &msgComplete);
            if (ret == 1)
            {
                fprintf(stderr, "  Disconnect received\n");
                disconnect = 1;
                break;
            }

            if (!msgComplete)
            {
                continue;
            }

            int msgLen = MsgLenFromXmitLen(StartInfo.msglenInd, ClientInfo[mpx].responseLen);
            int startPos = MsgStartPos(StartInfo.msglenInd);

            fprintf(stderr, "CLIENT %d Message %d bytes (receiving %d bytes)\n", mpx, msgLen, ClientInfo[mpx].responseLen);
            fprintf(stderr, "  %.*s\n", msgLen, ClientInfo[mpx].response + startPos);

            MemFree(ProgramName, &ClientInfo[mpx].response);
            ClientInfo[mpx].responseLen = 0;
            break;
        }

        if (disconnect)
            break;
    }

    return(endClient(&sockFd));
}

void doConnect(const int i, int * const sockFd)
{
    int ret = TcpSetConnectSocket("localhost", StartInfo.svcName, sockFd);
    if (ret)
    {
        fprintf(stderr, "Cannot connect to service %s\n", StartInfo.svcName);
        exit(1);
    }
    MainPoll[i].fd = *sockFd;
    MainPoll[i].events = POLLIN;
    fprintf(stderr, "CLIENT %d FD %d\n", i, *sockFd);
}

static void doCalloc(void)
{
    MainPoll = calloc(StartInfo.maxClients, sizeof(struct pollfd));
    if (MainPoll == NULL)
        SysExit(__func__, errno, SYSCALL_CALLOC);
    ClientInfo = calloc(StartInfo.maxClients, sizeof(clientinfo_t));
    if (ClientInfo == NULL)
        SysExit(__func__, errno, SYSCALL_CALLOC);
}

static void validateInput(const int argc, const char **argv,
                            string_t msglenInd[SZ_MSGLEN_IND + 1])
{
    if (argc != 2)
    {
        showUsage();
        exit(1);
    }

    if (strnlen(argv[1], SZ_MSGLEN_IND + 1) == SZ_MSGLEN_IND + 1)
    {
        showUsage();
        fprintf(stderr, "Message length indicator too long (Maximum 1 character)\n");
        exit(1);
    }
    strlcpy(msglenInd, argv[1], SZ_MSGLEN_IND + 1);
    if (*msglenInd != MSG_WITH_NULL && *msglenInd != MSG_WITH_TOTLEN && *msglenInd != MSG_WITH_MSGLEN)
    {
        showUsage();
        fprintf(stderr, "Invalid message length indicator (N, T or M)\n");
        exit(1);
    }
}

static void showUsage(void)
{
    fprintf(stderr, "Usage: bankclient msglenInd\n");
    fprintf(stderr, "       msglenInd must not exceed 1 character (N, T or M)\n");
}

static int xmitMsg(const int i, const int fd, const char * const msg, const int msgLen)
{
    char sendline[1024];

    int sendLen = MsgLenToXmitLen(StartInfo.msglenInd, msgLen);;

    if (*StartInfo.msglenInd == MSG_WITH_NULL)
    {
        strlcpy(sendline, msg, sendLen);
    }
    else
    {
        MsgLenToBuf(StartInfo.msglenInd, msgLen, sendline);
        memcpy(sendline + SZ_MSGLEN_IN_MSG, msg, msgLen);
    }

    fprintf(stderr, "CLIENT %d Message %d bytes (sending %d bytes)\n", i, msgLen, sendLen);
    fprintf(stderr, "  %.*s\n", msgLen, msg);

    return(SocketSendNoYield(fd, sendline, sendLen));
}

static int endClient(int * const sockFd)
{
    (void) llclose(*sockFd);
    *sockFd = -1;
    free(MainPoll);
    free(ClientInfo);
    return(0);
}