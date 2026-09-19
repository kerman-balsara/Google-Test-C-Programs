#ifndef LLSOCKET_H
#define LLSOCKET_H

#include <stddef.h>
#include <sys/types.h>

// ---------------------
// Internal definitions.
// ---------------------

#ifdef __cplusplus
extern "C" {
#endif

int llsocket(int domain, int type, int protocol);
int llbind(int sockfd, const struct sockaddr * const addr, socklen_t addrlen);
int lllisten(int sockfd, int backlog);
int llconnect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t llsend(const int fd, const char * const buf, const size_t buflen, const int flags);
ssize_t llrecv(const int fd, char * const buf, const size_t buflen, const int flags);
int llgetsockoptSocketError(int sockfd, int * so_error, socklen_t * so_len);
int llshutdown(int sockfd, int how);

#ifdef __cplusplus
}
#endif

#endif // LLSOCKET_H