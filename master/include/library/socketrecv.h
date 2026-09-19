#ifndef SOCKETRECV_H
#define SOCKETRECV_H

// ---------------------
// External definitions.
// ---------------------

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

// caller - Info for memory allocation of msg and prevMsg
// fd - For poll/recv data
// msglenInd - For end-of-message
// msg/msgLen - Completed message to be processed by caller
// prevMsg/prevMsgLen - Remaining messages to be processed by subsequent
// calls to this function.
// 
// Function blocks on a call to poll for the specified fd. The function
// blocks (and does not return) until at least one complete message has
// been received or a disconnect is received.
//
// A complete message is returned in msg along with the msgLen. Any other
// messages (incomplete or complete) are retained in prevMsg along
// with the prevMsgLen. The caller must process the complete message,
// free msg, reset the msgLen to 0, and recall this function.
//
// If this function is called with an incomplete message in prevMsg, the
// function blocks until further messages are received. These messages are
// appended to prevMsg and prevMsgLen is updated.
//
// If a disconnect is received, msg and prevMsg are freed and length is
// reset to zero. In all other cases, the caller must free memory and
// reset length.
//
// This function should be used when multiple client messages are received
// from the one and only fd on a server. It is essential that client messages
// contain a unique identifier so that the client can be identified.
//
// Flow (assume each complete message is 25 bytes):
// - Poll blocks, poll wakes up, receives 50 bytes
//   msgLen = 25 bytes, prevMsgLen = 25 bytes
// - Function returns with a complete message of 25 bytes (msgLen)
// - Caller processes the complete message (resets msg/msgLen = 0) and recalls.
//   msgLen = 25 bytes, prevMsgLen = 0 bytes
// - Function returns with a complete message of 25 bytes (msgLen)
// - Caller processes the complete message (resets msg/msgLen = 0) and recalls.
// - Poll blocks (waiting for further messages)
// - Poll wakes up, receives 40 bytes
//   msgLen = 25 bytes, prevMsgLen = 15 bytes
// - Function returns with a complete message of 25 bytes (msgLen)
// - Caller processes the complete message (resets msg/msgLen = 0) and recalls
//   (note the unprocessed 15 bytes in prevMsgLen)
// - Poll blocks (waiting for further messages)
// - Poll wakes up, receives 10 bytes
//   msgLen = 25 bytes, prevMsgLen = 0 bytes
// - Function returns with a complete message of 25 bytes (msgLen)
// - Caller processes the complete message (resets msg/msgLen = 0) and recalls
// - Poll blocks (waiting for further messages)
//
// Returns:
// 0 if message in msg can be processed
// 1 if disconnect received
int SocketRecvByClient(constr_t caller,
                       const int fd,
                       constr_t msgLenInd,
                       char ** msg, int * const msgLen,
                       char ** prevMsg, int * const prevMsgLen);

// caller - Info for memory allocation of prevMsg
// fd - For recv data
// msglenInd - For end-of-message
// prevMsg/prevMsgLen - Remaining messages to be processed by subsequent
// calls to this function.
// msgComplete - Indicates to the caller whether there is a message to process
//
// Function assumes that caller has checked that a message is ready to be
// received from the fd.
//
// Returns:
// 0 caller must check msgComplete to determine whether the message in prevMsg can be
//   processed
// 1 if disconnect received
int SocketRecv(constr_t caller,
               const int fd,
               constr_t msglenInd,
               char ** prevMsg, int * const prevMsgLen,
               bool_t * const msgComplete);


// caller - Info for memory allocation of msg
// fd - For recv data
// msg/msgLen - Remaining messages to be processed by subsequent
// calls to this function.
// 
// Function assumes that data is ready to be received (caller blocks
// on a call to poll). Function returns with whatever data is received.
// The caller has to determine whether the message is complete or not
// and process appropriately.
//
// The caller must
// - process any complete messages
// - remove the complete messages from msg and adjust msgLen
// - if there is an incomplete message, retain msg and msgLen.
//   and recall this function when more data is ready to be received.
//
// If a disconnect is received, msg is freed and msgLen is reset to
// zero. In all other cases, the caller must manage the memory.
//
// This function should be used when multiple client messages are received
// on the one and only fd on a server.
//
// Flow (assume each complete message is 25 bytes):
// - Caller blocks, caller wakes up (passes msgLen = 0 bytes)
// - Function reads 50 bytes and returns with msgLen = 50 bytes
// - Caller processes the 2 messages (resets msg/msgLen = 0 bytes).
// - Caller blocks, caller wakes up (passes msgLen = 0)
// - Function reads 40 bytes and returns with msgLen = 40 bytes
// - Caller processes 1 message (resets msg/msgLen = 15 bytes).
// - Caller blocks, caller wakes up (passes msgLen = 15 bytes)
// - Function reads 10 bytes and returns with msgLen = 25 bytes
// - Caller processes 1 message (resets msg/msgLen = 0 bytes).
// - Caller blocks.
//
// Returns:
// 0 if data received
// 1 if disconnect received
int SocketRecvByServer(constr_t caller,
                        const int fd,
                        char ** msg, int * const msgLen);

#ifdef __cplusplus
}
#endif

#endif // SOCKETRECV_H