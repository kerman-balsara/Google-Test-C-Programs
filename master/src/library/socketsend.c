#include <errno.h>

#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <llsocket.h>
#include <logrtns.h>
#include <memrtns.h>
#include <socketsend.h>
#include <sysexit.h>

int SocketSendNoYield(const int fd, const char * const buf, const int bufLen)
{
    char * bufPtr = (char *) buf;
    int totalBytesSent = 0;
    int currBytesSent;

    while (totalBytesSent < bufLen)
    {
        for (;;)
        {
            currBytesSent = llsend(fd, bufPtr, bufLen - totalBytesSent, 0);
            if (currBytesSent > 0) break;
            if (currBytesSent < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                    continue;

                if (errno == ECONNRESET)
                    return(1);

                LogErrorF("fd: %d, buf: [%.*s], totalBytesSent: %d", fd, bufLen, buf, totalBytesSent);
                SysExit(__func__, errno, SYSCALL_SEND);    
            }
            LogErrorF("fd: %d, buf: [%.*s], totalBytesSent: %d", fd, bufLen, buf, totalBytesSent);
            SysExit(__func__, 0, SYSCALL_SEND);
        }

        totalBytesSent += currBytesSent;
        bufPtr += currBytesSent;
    }

    return(0);
}

int SocketSendYield(const int fd,
                    const char * const buf,
                    const int bufLen,
                    int * prevBytesSent)
{
    char * bufPtr = (char *) buf + *prevBytesSent;
    int currBytesSent;

    for (;;)
    {
        currBytesSent = llsend(fd, bufPtr, bufLen - *prevBytesSent, 0);
        if (currBytesSent > 0) break;
        if (currBytesSent < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            LogErrorF("fd: %d, buf: [%.*s], prevBytesSent: %d", fd, bufLen, buf, *prevBytesSent);
            SysExit(__func__, errno, SYSCALL_SEND);    
        }
        LogErrorF("fd: %d, buf: [%.*s], prevBytesSent: %d", fd, bufLen, buf, *prevBytesSent);
        SysExit(__func__, 0, SYSCALL_SEND);
    }

    *prevBytesSent += currBytesSent;

    bool_t msgComplete = (*prevBytesSent == bufLen);

    return(msgComplete ? 0 : 1);
}
