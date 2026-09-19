#ifndef TCPRTNS_H
#define TCPRTNS_H

// ---------------------
// External definitions.
// ---------------------

#include <sys/socket.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void TcpSetListenSocket(constr_t hostName, constr_t svcName, const int listenBacklog, int * listenFd, struct sockaddr_in * sockaddr, const int sockaddr_len);
void TcpSetSockAddr(constr_t hostName, constr_t svcName, struct sockaddr_in * const sockaddr, const int sockaddr_len);
int TcpSetConnectSocketNoBlock(constr_t hostName, constr_t svcName, int *socketFd);
int TcpSetConnectSocket(constr_t hostName, constr_t svcName, int *socketFd);

// Uses poll to wait for close at the other end so must not be used by functions that already use poll.
void TcpShutdown(int * fd);

#ifdef __cplusplus
}
#endif

#endif // TCPRTNS_H