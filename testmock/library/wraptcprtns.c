// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

// System under test
#include <tcprtns.c>

int WRAPsocketBlock(const int socketFd)
{
    return(socketBlock(socketFd));
}

int WRAPretryLater(int *socketFd)
{
    return(retryLater(socketFd));
}
