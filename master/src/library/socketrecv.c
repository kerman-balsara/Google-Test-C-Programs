#include <errno.h>

#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include <errcodes.h>
#include <msglenind.h>
#include <syssizes.h>
#include <typedefs.h>

#include <llpoll.h>
#include <llsocket.h>
#include <logpkgrtns.h>
#include <logrtns.h>
#include <memrtns.h>
#include <msglenrtns.h>
#include <socketrecv.h>
#include <strutils.h>
#include <sysexit.h>

int SocketRecvByClient(constr_t caller,
                       const int fd,
                       constr_t msglenInd,
                       char ** msg, int * const msgLen,
                       char ** prevMsg, int * const prevMsgLen)
{
    char readBuf[SZ_SOCKETRW];

    for (;;)
    {
        bool_t msgComplete;
        MsgGetComplete(caller, msglenInd, msg, msgLen, prevMsg, prevMsgLen, &msgComplete);
        if (msgComplete)
            break;

        struct pollfd PollFd;
        PollFd.fd = fd;
        PollFd.events = POLLIN;

        for (;;)
        {
            int ret = llpoll(&PollFd, 1, -1);
            if (ret < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                    continue;

                SysExit(__func__, errno, SYSCALL_POLL);
            }

            if (PollFd.revents & POLLHUP)
            {
                LogInfoF("%s: fd %d: POLLHUP received", caller, fd);
                break;
            }

            if (PollFd.revents & POLLIN)
                break;

            LogErrorF("Poll failed: revents %d", PollFd.revents);    
            SysExit(__func__, 0, SYSCALL_POLL);
        }

        int bytesRead = llrecv(fd, readBuf, sizeof(readBuf), 0);
        if (bytesRead < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            if (errno == ECONNRESET)
            {
                LogInfoF("%s: fd %d: Reset received", caller, fd);
                bytesRead = 0;
            }
            else
            {    
                SysExit(__func__, errno, SYSCALL_RECV);
            }
        }

        // Cater for disconnect
        if (bytesRead == 0)
        {
            LogInfoF("%s: fd %d: Disconnect received", caller, fd);

            MemFree(caller, msg);
            MemFree(caller, prevMsg);
            *msgLen = 0;
            *prevMsgLen = 0;

            return(1);
        }

        MemRealloc(caller, prevMsg, *prevMsgLen + bytesRead);
        memcpy(*prevMsg + *prevMsgLen, readBuf, bytesRead);
        *prevMsgLen += bytesRead;
    }

    return(0);
}

int SocketRecv(constr_t caller,
               const int fd,
               constr_t msglenInd,
               char ** prevMsg, int * prevMsgLen,
               bool_t * const msgComplete)
{
    char readBuf[SZ_SOCKETRW];

    int bytesRead;

    for (;;)
    {
        bytesRead = llrecv(fd, readBuf, sizeof(readBuf), 0);
        if (bytesRead > 0) break;
        if (bytesRead < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            if (errno != ECONNRESET)
                SysExit(__func__, errno, SYSCALL_RECV);

            LogInfoF("%s: fd %d: Reset received", caller, fd);
        }

        LogInfoF("%s: fd %d: Disconnect received", caller, fd);
        MemFree(caller, prevMsg);
        *prevMsgLen = 0;
        return(1);
    }

    MemRealloc(caller, prevMsg, *prevMsgLen + bytesRead);
    memcpy(*prevMsg + *prevMsgLen, readBuf, bytesRead);
    *prevMsgLen += bytesRead;

    *msgComplete = IsMsgComplete(msglenInd, *prevMsg, *prevMsgLen, NULL);
    return(0);
}

int SocketRecvByServer(constr_t caller,
                       const int fd,
                       char ** msg, int * const msgLen)
{
    char readBuf[SZ_SOCKETRW];

    int bytesRead;

    for (;;)
    {
        bytesRead = llrecv(fd, readBuf, sizeof(readBuf), 0);
        if (bytesRead > 0) break;
        if (bytesRead == 0)
        {
            LogInfoF("%s: fd %d: Disconnect received", caller, fd);
            MemFree(caller, msg);
            *msgLen = 0;
            return(1);
        }

        if (errno == EINTR || errno == EAGAIN)
            continue;

        if (errno == ECONNRESET)
        {
            LogInfoF("%s: fd %d: Reset received", caller, fd);
            MemFree(caller, msg);
            *msgLen = 0;
            return(1);
        }

        SysExit(__func__, errno, SYSCALL_RECV);
    }

    MemRealloc(caller, msg, *msgLen + bytesRead);
    memcpy(*msg + *msgLen, readBuf, bytesRead);
    *msgLen += bytesRead;

    return(0);
}

