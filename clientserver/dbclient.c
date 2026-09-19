#include <errno.h>

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <dbmsgdefs.h>
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

// Fire off multiple automatic requests to the dbserver
#define MAX_MESSAGES 1
// Accept a single request from the user to be sent to the dbserver
#define SZ_USER_INPUT 50

typedef struct
{
    unsigned long maxClients;
    string_t svcName[SZ_SERVICE_NAME + 1];
    string_t msglenInd[SZ_MSGLEN_IND + 1];
} startInfo_t;

static startInfo_t StartInfo;

static struct pollfd *MainPoll;

static constr_t ProgramName = "dbclient";

typedef struct
{
    char *response;
    int responseLen;
} clientinfo_t;

static clientinfo_t *ClientInfo;

static void doCalloc(void);
static void doConnect(const int i, int * const sockFd);
static int xmitMsg(const int i, const int fd, const char *const msg, const int msgLen);
static int endClient(int * const sockFd);

int main(const int argc, const char **argv)
{
    int ret;

    if (argc != 1)
    {
        fprintf(stderr, "Usage: dbclient\n");
        return(1);
    }

    StartInfo.maxClients = 1;
    strlcpy(StartInfo.svcName, "dbserver", sizeof(StartInfo.svcName));
    strlcpy(StartInfo.msglenInd, "N", sizeof(StartInfo.msglenInd));

    doCalloc();

    int sockFd;
    doConnect(0, &sockFd);

    // Fire off multiple automatic requests to the dbserver
    for (int i = 0; i <= MAX_MESSAGES; i++)
    {
        dbmsg_t dbmsg;
        memset(dbmsg.id, '0', SZ_DB_ID);
        memset(dbmsg.auditNo, '0', SZ_BANK_AUDIT_NO);
        ret = xmitMsg(0, sockFd, (char *) &dbmsg, sizeof(dbmsg));
        if (ret == 1)
            return(endClient(&sockFd));
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
        printf("Db Server message: 000000000000\n");
        printf("The first six zeros are for the id (sent in the request)\n");
        printf("The next six zeros are for the audit number (received in the response)\n\n");

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
                fprintf(stderr, "Disconnect received\n");
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

static int xmitMsg(const int i, const int fd, const char * const msg, const int msgLen)
{
    int sendLen = MsgLenToXmitLen(StartInfo.msglenInd, msgLen);
    char sendline[sendLen];

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