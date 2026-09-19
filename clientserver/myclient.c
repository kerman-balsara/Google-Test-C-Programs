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
#include <servermsgdefs.h>
#include <typedefs.h>

#include <llpoll.h>
#include <llsocket.h>
#include <llunistd.h>
#include <memrtns.h>
#include <msglenrtns.h>
#include <tcprtns.h>
#include <socketrecv.h>
#include <socketsend.h>
#include <strutils.h>
#include <sysexit.h>

#define SZ_MAX_CLIENTS 1
#define SZ_USER_INPUT 50

typedef struct
{
    unsigned long maxClients;
    string_t svcName[SZ_SERVICE_NAME + 1];
    string_t msglenInd[SZ_MSGLEN_IND + 1];
} startInfo_t;

static startInfo_t StartInfo;

static struct pollfd *MainPoll;

static constr_t ProgramName = "myclient";

typedef struct
{
    bool_t disconnect;
    char *request;
    int requestLen;
    char *response;
    int responseLen;
} clientinfo_t;

static clientinfo_t *ClientInfo;

static void validateInput(const int argc, const char **argv,
                            unsigned long * const maxClients,
                            string_t svcName[SZ_SERVICE_NAME + 1]);
static void showUsage(void);
static void doCalloc(void);
static void doConnect(const int i, int * const sockFd);
static int xmitMsg(const int i, const int fd, const char *const msg, const int msgLen);
static void closeFd(int * const sockFd);

int main(const int argc, const char **argv)
{
    int ret;

    // Validate input
    validateInput(argc, argv,  &StartInfo.maxClients, StartInfo.svcName);

    strlcpy(StartInfo.msglenInd, "N", sizeof(StartInfo.msglenInd));

    int sockFd[StartInfo.maxClients];

    doCalloc();

    for (int i = 0; i < StartInfo.maxClients; i++)
    {
        doConnect(i, &sockFd[i]);
    }

    fprintf(stderr, "-------------------------------\n\n");

    servermsg_t servermsg[StartInfo.maxClients];
    bool_t servermsgInd[StartInfo.maxClients];
    for (int i = 0; i < StartInfo.maxClients; i++)
    {
        servermsgInd[i] = 0;
    }
    // TODO (malloc/free request)

    // Send initial message to the server
    for (int i = 0; i < StartInfo.maxClients; i++)
    {
        if (MainPoll[i].fd == -1)
            continue;
        memset(&servermsg[i], ' ', sizeof(servermsg_t));
        servermsg[i].initialMsg = '1';
        memcpy(servermsg[i].bankmsg.ccNo, "411111111111111", 15);
        servermsg[i].bankmsg.ccNo[15] = i + '0';
        ret = xmitMsg(i, sockFd[i], (char *) &servermsg[i], sizeof(servermsg_t));
        if (ret == 1)
        {
            closeFd(&sockFd[i]);
            MainPoll[i].fd = -1;
            continue;
        }
        servermsgInd[i] = 1;
    }

    fprintf(stderr, "-------------------------------\n\n");

    // Wait until all responses are received from the server
    for (;;)
    {
        bool_t outstanding = 0;
        for (int i = 0; i < StartInfo.maxClients; i++)
        {
            MainPoll[i].revents = 0;
            if (MainPoll[i].fd != -1)
            {
                outstanding = 1;
                break;
            }
        }

        if (!outstanding)
        {
            break;
        }

        ret = llpoll(MainPoll, StartInfo.maxClients, -1);
        if (ret < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            SysExit(__func__, errno, SYSCALL_POLL);
        }

        for (int mpx = 0; mpx < StartInfo.maxClients; mpx++)
        {
            if (MainPoll[mpx].fd == -1 || !MainPoll[mpx].revents)
                continue;

            if (MainPoll[mpx].revents & POLLIN)
            {
                bool_t msgComplete = 0;
                ret = SocketRecv(ProgramName, MainPoll[mpx].fd, StartInfo.msglenInd, &ClientInfo[mpx].response,
                                    &ClientInfo[mpx].responseLen, &msgComplete);
                if (ret == 1)
                {
                    fprintf(stderr, "  Disconnect received\n");
                    closeFd(&sockFd[mpx]);
                    MainPoll[mpx].fd = -1;
                    servermsgInd[mpx] = 0;
                    continue;
                }
                if (!msgComplete)
                    continue;

                int msgLen = MsgLenFromXmitLen(StartInfo.msglenInd, ClientInfo[mpx].responseLen);
                int startPos = MsgStartPos(StartInfo.msglenInd);

                fprintf(stderr, "CLIENT %d Message %d bytes (receiving %d bytes)\n", mpx, msgLen, ClientInfo[mpx].responseLen);
                fprintf(stderr, "  %.*s\n", msgLen, ClientInfo[mpx].response + startPos);

                if (!memcmp(ClientInfo[mpx].response + startPos, "ERROR:", 6))
                {
                    closeFd(&sockFd[mpx]);
                    servermsgInd[mpx] = 0;
                }
                else
                {
                    memcpy((char *) &servermsg[mpx], ClientInfo[mpx].response + startPos, msgLen);
                }

                MemFree(ProgramName, &ClientInfo[mpx].response);
                ClientInfo[mpx].responseLen = 0;
                MainPoll[mpx].fd = -1;    
            }
        }
    }

    fprintf(stderr, "-------------------------------\n\n");

    // TODO (malloc/free request)

    // Send completion message to the server so that the server can clean up 
    // and close the connection
    for (int i = 0; i < StartInfo.maxClients; i++)
    {
        if (sockFd[i] != -1 && servermsgInd[i])
        {
            // Send completion message so that myserver can cleanup.
            servermsg[i].initialMsg = '0';
            ret = xmitMsg(i, sockFd[i], (char *) &servermsg[i], sizeof(servermsg_t));
            if (ret == 1)
            {
                closeFd(&sockFd[i]);
                MainPoll[i].fd = -1;
                servermsgInd[i] = 0;
            }
        }
    }

    fprintf(stderr, "-------------------------------\n\n");

    // Wait for close from server
    for (int i = 0; i < StartInfo.maxClients; i++)
    {
        if (sockFd[i] != -1 && servermsgInd[i])
        {
            struct pollfd PollFd;
            PollFd.fd = sockFd[i];
            PollFd.events = POLLIN;

            for (;;)
            {
                ret = llpoll(&PollFd, 1, 2000);
                if (ret < 0)
                {
                    if (errno == EINTR || errno == EAGAIN)
                        continue;
                    SysExit(__func__, errno, SYSCALL_POLL);
                }
                break;
            }
            if (PollFd.revents & POLLIN)
            {
                char readBuf[SZ_SOCKETRW];

                int bytesRead;
                do
                {
                    bytesRead = llrecv(sockFd[i], readBuf, sizeof(readBuf), 0);

                } while (bytesRead > 0);
            }

            closeFd(&sockFd[i]);
            MainPoll[i].fd = -1;
            servermsgInd[i] = 0;
        }
    }

    free(MainPoll);
    free(ClientInfo);

    StartInfo.maxClients = 1;

    doCalloc();

    printf("Entering interactive mode\n\n");

    for (;;)
    {
        if (sockFd[0] == -1)
        {
            doConnect(0, &sockFd[0]);

            if (sockFd[0] == -1)
            {
                fprintf(stderr, "Cannot connect to service %s\n", StartInfo.svcName);
                break;
            }
        }

        printf("-------------------------------\n\n");
        printf("Myserver message: YAAAAAABBBBBBBBBBBBBBBBBBBCCZZ\n");
        printf("Y: Message Flag, Z: Server Status\n");
        printf("A: Audit Number, B: CC Number, C: Bank Status\n");
        printf("Y,A,B are the request parameters, C,Z are the response\n");
        printf("Y is set to '1' for initial message, '0' for final message\n");
        printf("The response to a final message is a disconnect\n\n");

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

        ret = xmitMsg(0, sockFd[0], cmd, strlen(cmd));
        if (ret == 1)
        {
            break;
        }

        bool_t disconnect = 0;

        for (;;)
        {
            ret = llpoll(MainPoll, StartInfo.maxClients, -1);
            if (ret < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                SysExit(__func__, errno, SYSCALL_POLL);
            }

            int mpx = 0;

            bool_t msgComplete = 0;
            ret = SocketRecv(ProgramName, MainPoll[mpx].fd, StartInfo.msglenInd, &ClientInfo[mpx].response,
                                &ClientInfo[mpx].responseLen, &msgComplete);
            if (ret == 1)
            {
                fprintf(stderr, "  Disconnect received\n");
                disconnect = 1;
                break;
            }

            if (!msgComplete)
                continue;
                
            int msgLen = MsgLenFromXmitLen(StartInfo.msglenInd, ClientInfo[mpx].responseLen);
            int startPos = MsgStartPos(StartInfo.msglenInd);

            fprintf(stderr, "CLIENT %d Message %d bytes (receiving %d bytes)\n", mpx, msgLen, ClientInfo[mpx].responseLen);
            fprintf(stderr, "  %.*s\n", msgLen, ClientInfo[mpx].response + startPos);

            if (!memcmp(ClientInfo[mpx].response + startPos, "ERROR:", 6))
            {
                fprintf(stderr, "  Disconnecting due to error\n");
                disconnect = 1;
            }

            MemFree(ProgramName, &ClientInfo[mpx].response);
            ClientInfo[mpx].responseLen = 0;
            break;
        }

        if (disconnect)
        {
            closeFd(&sockFd[0]);
        }
    }

    free(MainPoll);
    free(ClientInfo);
    if (sockFd[0] != -1)
        closeFd(&sockFd[0]);
    return(0);
}

void doConnect(const int i, int * const sockFd)
{
    int ret = TcpSetConnectSocket("localhost", StartInfo.svcName, sockFd);
    MainPoll[i].fd = *sockFd;
    if (ret == 0)
        MainPoll[i].events = POLLIN;
    fprintf(stderr, "CLIENT %d FD %d (%s)\n", i, *sockFd, *sockFd == -1 ? "Not Connected" : "Connected");
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
                            unsigned long * const maxClients,
                            string_t svcName[SZ_SERVICE_NAME + 1])
{
    if (argc != 3)
    {
        showUsage();
        fprintf(stderr, "Invalid number of arguments\n");
        exit(1);
    }

    if (strnlen(argv[1], SZ_MAX_CLIENTS + 1) == SZ_MAX_CLIENTS + 1)
    {
        showUsage();
        fprintf(stderr, "Number of clients too long (Maximum 1 digit)\n");
        exit(1);
    }
    *maxClients = strtoul(argv[1], NULL, 0);
    if (errno == EINVAL || errno == ERANGE || *maxClients == 0)
    {
        showUsage();
        fprintf(stderr, "Invalid maximum number of clients (1-9)\n");
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
    fprintf(stderr, "Usage: myclient maxClients serviceName\n");
    fprintf(stderr, "       maxClients must be between 1-9\n");
    fprintf(stderr, "       serviceName must not exceed 10 characters\n");
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

static void closeFd(int * const sockFd)
{
    (void) llclose(*sockFd);
    *sockFd = -1;
}