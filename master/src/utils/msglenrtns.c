#include <string.h>

#include <errcodes.h>
#include <logpkgdefs.h>
#include <msglenind.h>
#include <syssizes.h>
#include <typedefs.h>

#include <logpkgrtns.h>
#include <logrtns.h>
#include <memrtns.h>
#include <strutils.h>
#include <msglenrtns.h>
#include <sysexit.h>

// --------------------------------------------------------------------
// Show passed message.
// --------------------------------------------------------------------

void MsgPrint(constr_t caller, const int fd, constr_t msglenInd, const char * msg, const int xmitLen, constr_t commType)
{
    int startPos = MsgStartPos(msglenInd);
    int msgLen = MsgLenFromXmitLen(msglenInd, xmitLen);

    LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: Message %d bytes (%s %d bytes)", caller, fd, msgLen, commType, xmitLen);
    LogPkgTraceF(LOGPKG_WIRE, "%s: fd %d: [%.*s]", caller, fd, msgLen, msg + startPos);
}

// --------------------------------------------------------------------
// Return the starting position of a message based on the passed
// msglenInd.
// For messages that include a terminating null character, the starting
// position returned is 0.
// For messages that include a length at the beginning of the message,
// the starting position returned is SZ_MSGLEN_IN_MSG.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/msglenrtns.cpp
// --------------------------------------------------------------------

int MsgStartPos(constr_t msglenInd)
{
    return(*msglenInd == MSG_WITH_NULL ? 0 : SZ_MSGLEN_IN_MSG);
}

// --------------------------------------------------------------------
// Return the message length based on the passed msglenInd and the 
// length of the message transmitted.
// For messages that include a terminating null character, the length
// returned excludes the null character.
// For messages that include a length in the message, the length
// returned excludes SZ_MSGLEN_IN_MSG.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/msglenrtns.cpp
// --------------------------------------------------------------------

int MsgLenFromXmitLen(constr_t msglenInd, const int xmitLen)
{
    if (*msglenInd == MSG_WITH_NULL) return(xmitLen - 1);
    return(xmitLen - SZ_MSGLEN_IN_MSG);
}

// --------------------------------------------------------------------
// Return the transmission length based on the passed msglenInd and the 
// length of the message.
// For messages that include a terminating null character, the length
// returned includes the null character.
// For messages that include a length in the message, the length
// returned includes SZ_MSGLEN_IN_MSG.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/msglenrtns.cpp
// --------------------------------------------------------------------

int MsgLenToXmitLen(constr_t msglenInd, const int msgLen)
{
    if (*msglenInd == MSG_WITH_NULL) return(msgLen + 1);
    return(msgLen + SZ_MSGLEN_IN_MSG);
}

// --------------------------------------------------------------------
// For messages that include a terminating null character, this function
// must not be called.
// For messages that include a length in the message, the passed buf is
// set based on the passed msglenInd. For messages that include the total
// length, buf contains the passed msgLen + SZ_MSGLEN_IN_MSG. For messages
// that include only the message length, buff contains the passed msgLen. 
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/msglenrtns.cpp
// --------------------------------------------------------------------

void MsgLenToBuf(constr_t msglenInd, const int msgLen, char buf[SZ_MSGLEN_IN_MSG])
{
    memset(buf, ' ', SZ_MSGLEN_IN_MSG);
    if (*msglenInd == MSG_WITH_NULL)
    {
        LogErrorF("Message length '%s' invalid for this function", msglenInd);
        SysExit(__func__, 0, MSGLEN_IND_INVALID);
    }
    int wkMsgLen = msgLen;
    if (*msglenInd == MSG_WITH_TOTLEN)
        wkMsgLen += SZ_MSGLEN_IN_MSG;
    ShortToBuf(wkMsgLen, buf);
}

// --------------------------------------------------------------------
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/msglenrtns.cpp
// --------------------------------------------------------------------

void MsgGetComplete(constr_t caller,
                    constr_t msglenInd,
                    char ** msg, int * msgLen,
                    char ** prevMsg, int * prevMsgLen,
                    bool_t * const msgComplete)
{
    int lenInMsg;

    *msgComplete = IsMsgComplete(msglenInd, *prevMsg, *prevMsgLen, &lenInMsg);

    if (*msgComplete)
    {
        MemRealloc(caller, msg, *msgLen + lenInMsg);
        memcpy(*msg + *msgLen, *prevMsg, lenInMsg);
        *msgLen += lenInMsg;

        memcpy(*prevMsg, *prevMsg + lenInMsg, *prevMsgLen - lenInMsg);
        *prevMsgLen -= lenInMsg;
    }
}

bool_t IsMsgComplete(constr_t msglenInd, const char * msg, const int msgLen, int * const lenInMsg)
{
    bool_t msgComplete = 0;

    int wklenInMsg = 0;

    if (*msglenInd == MSG_WITH_NULL && msgLen > 0)
    {
        char *dp;
        for (dp = (char *) msg; dp < msg + msgLen; dp++)
        {
            if (*dp == '\0')
            {
                msgComplete = 1;
                wklenInMsg = dp - msg + 1;
                break;
            }
        }
    }
    else if (msgLen >= SZ_MSGLEN_IN_MSG)
    {
        wklenInMsg = BufToShort(msg);

        if (*msglenInd == MSG_WITH_MSGLEN)
            wklenInMsg += SZ_MSGLEN_IN_MSG;

        msgComplete = (msgLen >= wklenInMsg);
    }

    if (msgComplete && lenInMsg)
    {
        *lenInMsg = wklenInMsg;
    }
    return(msgComplete);
}