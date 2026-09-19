#ifndef MSGLENRTNS_H
#define MSGLENRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void MsgPrint(constr_t caller, int fd, constr_t msglenInd, const char * msg, const int len, constr_t commType);
void MsgLenToBuf(constr_t msglenInd, const int msgLen, char buf[SZ_MSGLEN_IN_MSG]);
int MsgLenToXmitLen(constr_t msglenInd, const int msgLen);
int MsgLenFromXmitLen(constr_t msglenInd, const int xmitLen);
int MsgStartPos(constr_t msglenInd);
void MsgGetComplete(constr_t caller,
                    constr_t msglenInd,
                    char ** msg, int * msgLen,
                    char ** prevMsg, int * prevMsgLen,
                    bool_t * const msgComplete);
bool_t IsMsgComplete(constr_t msglenInd, const char * msg, const int msgLen, int * const lenInMsg);

#ifdef __cplusplus
}
#endif

#endif // MSGLENRTNS_H