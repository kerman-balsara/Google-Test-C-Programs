// --------------------------------------------------------------------
// Wrappers to allow mocks.
// --------------------------------------------------------------------

#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>

#include <llsocket.h>

int llsocket(int domain, int type, int protocol)
{
    return(socket(domain, type, protocol));
}

int llbind(int sockfd, const struct sockaddr * const addr, socklen_t addrlen)
{
    return(bind(sockfd, addr, addrlen));
}

int lllisten(int sockfd, int backlog)
{
    return(listen(sockfd, backlog));
}

int llconnect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return(connect(sockfd, addr, addrlen));
}

ssize_t llsend(const int fd, const char * const buf, const size_t buflen, const int flags)
{
    return(send(fd, buf, buflen, flags));
}

ssize_t llrecv(const int fd, char * const buf, const size_t buflen, const int flags)
{
    return(recv(fd, buf, buflen, flags));
}

int llgetsockoptSocketError(int sockfd, int * so_error, socklen_t * so_len)
{
    return(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, so_error, so_len));
}

int llshutdown(int sockfd, int how)
{
    return(shutdown(sockfd, how));
}
