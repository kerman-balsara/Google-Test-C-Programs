#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <llfcntl.h>
#include <llpoll.h>
#include <llsocket.h>
#include <llunistd.h>
#include <logpkgrtns.h>
#include <logrtns.h>
#include <tcprtns.h>
#include <sysexit.h>

static int socketBlock(const int socketFd);
static int retryLater(int *socketFd);

void TcpSetListenSocket(constr_t hostName, constr_t svcName, const int listenBacklog, int * listenFd, struct sockaddr_in * sockaddr, const int sockaddr_len)
{
    int ret;

    *listenFd = llsocket(AF_INET, SOCK_STREAM, 0);
    if (*listenFd < 0)
        SysExit(__func__, errno, SYSCALL_SOCKET);

    TcpSetSockAddr(hostName, svcName, sockaddr, sockaddr_len);

    sockaddr->sin_addr.s_addr = INADDR_ANY;

    ret = llbind(*listenFd, (struct sockaddr *) sockaddr, sockaddr_len);
    if (ret < 0)
        SysExit(__func__, errno, SYSCALL_BIND);

    ret = lllisten(*listenFd, listenBacklog);
    if (ret < 0)
        SysExit(__func__, errno, SYSCALL_LISTEN);
}

int TcpSetConnectSocketNoBlock(constr_t hostName, constr_t svcName, int *socketFd)
{
    int ret;

    struct sockaddr_in sockaddr;
    int sockaddr_len = sizeof(sockaddr);

    TcpSetSockAddr(hostName, svcName, &sockaddr, sockaddr_len);

    *socketFd = llsocket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (*socketFd < 0)
        SysExit(__func__, errno, SYSCALL_SOCKET);

    ret = llconnect(*socketFd, (struct sockaddr *) &sockaddr, sockaddr_len);
    if (ret == 0)
        return(socketBlock(*socketFd));
    if (errno != EINPROGRESS)
        SysExit(__func__, errno, SYSCALL_CONNECT);

    struct pollfd PollFd;
    PollFd.fd = *socketFd;
    PollFd.events = POLLOUT;
    PollFd.revents = 0;
    
    ret = llpoll(&PollFd, 1, 2000);
    if (ret < 0)
        SysExit(__func__, errno, SYSCALL_POLL);
    if (ret == 0)
        return(retryLater(socketFd));

    if (PollFd.revents & ~POLLOUT)
        return(retryLater(socketFd));
    
    int so_error = 0;
    socklen_t so_len = sizeof(int);
    ret = llgetsockoptSocketError(*socketFd, &so_error, &so_len);
    if (ret < 0)
        SysExit(__func__, errno, SYSCALL_GETSOCKOPT);

    return(so_error == 0 ? socketBlock(*socketFd) : retryLater(socketFd));
}

static int socketBlock(const int socketFd)
{
    int ret;
    int flags = llfcntl(socketFd, F_GETFL, 0);
    if (flags < 0)
        SysExit(__func__, errno, SYSCALL_FCNTL);
    flags &= ~O_NONBLOCK;
    ret = llfcntl(socketFd, F_SETFL, flags);
    if (ret < 0)
        SysExit(__func__, errno, SYSCALL_FCNTL);
    return(0);    
}

static int retryLater(int *socketFd)
{
    (void) llclose(*socketFd);
    *socketFd = -1;

    return(1);
}

int TcpSetConnectSocket(constr_t hostName, constr_t svcName, int *socketFd)
{
    int ret;

    struct sockaddr_in sockaddr;
    int sockaddr_len = sizeof(sockaddr);

    TcpSetSockAddr(hostName, svcName, &sockaddr, sockaddr_len);

    *socketFd = llsocket(AF_INET, SOCK_STREAM, 0);
    if (*socketFd < 0)
        SysExit(__func__, errno, SYSCALL_SOCKET);

    ret = llconnect(*socketFd, (struct sockaddr *) &sockaddr, sockaddr_len);
    if (ret < 0)
    {
        int saveErrno = errno;
        (void) llclose(*socketFd);
        *socketFd = -1;
        errno = saveErrno;
        if (errno == ECONNREFUSED)
            return(1);
        SysExit(__func__, errno, SYSCALL_CONNECT);
    }

    return(0);
}

void TcpSetSockAddr(constr_t hostName, constr_t svcName, struct sockaddr_in * const sockaddr, const int sockaddr_len)
{
    int ret;
    struct addrinfo *res, hints;

    memset(sockaddr, 0, sockaddr_len);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    ret = getaddrinfo(hostName, svcName, &hints, &res);
    if (ret)
    {
    	LogErrorF("Service %s Error: %s\n", svcName, gai_strerror(ret));
        SysExit(__func__, 0, SYSCALL_GETADDRINFO);
    }

    memcpy(sockaddr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

}

void TcpShutdown(int * fd)
{
    int ret;
    LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d", __func__, *fd);
    (void) llshutdown(*fd, SHUT_WR);
    
    struct pollfd PollFd;
    PollFd.fd = *fd;
    PollFd.events = POLLIN;
    PollFd.revents = 0;

    ret = llpoll(&PollFd, 1, 2000);
    LogPkgTraceF(LOGPKG_WIRE, "%s: poll ret %d, revents %d", __func__, ret, PollFd.revents);

    if (PollFd.revents & POLLIN)
    {
        char readBuf[SZ_SOCKETRW];

        int bytesRead;
        do
        {
            bytesRead = llrecv(*fd, readBuf, sizeof(readBuf), 0);
            LogPkgTraceF(LOGPKG_WIRE, "%s: received %d bytes", __func__, bytesRead);
            if (bytesRead < 0)
                LogPkgTraceF(LOGPKG_WIRE, "  errno %d [%s]", errno, strerror(errno));

        } while (bytesRead > 0);
    }

    (void) llshutdown(*fd, SHUT_RD);
    (void) llclose(*fd);
    *fd = -1;
}