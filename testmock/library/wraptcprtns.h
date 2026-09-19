#ifndef WRAPTCPRTNS_H
#define WRAPTCPRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#include <tcprtns.h>

#ifdef __cplusplus
extern "C" {
#endif

// Test static function using mocks
int WRAPsocketBlock(const int socketFd);
int WRAPretryLater(int *socketFd);

#ifdef __cplusplus
}
#endif

#endif // WRAPTCPRTNS_H