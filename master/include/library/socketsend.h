#ifndef SOCKETSEND_H
#define SOCKETSEND_H

// ---------------------
// External definitions.
// ---------------------

// ---------------------
// Internal definitions.
// ---------------------

#ifdef __cplusplus
extern "C" {
#endif

// Function assumes that caller has checked that a message is ready to be sent
// to the fd.
// The function does not return until the entire message has been sent.
int SocketSendNoYield(const int fd, const char * const buf, const int bufLen);

// Function assumes that caller has checked that a message is ready to be sent
// to the fd.
// The function can return
// 0 - All of the message is sent
// 1 - All of the message not sent; it is the caller's responsibility to recall
//     this function until the entire message has been sent.
// This function should be used when the passed fd is one end of a socket pair.
// i.e., if this function is used by a client, there is a corresponding socket
// at the server; if this function is being used by a server, there is a
// corresponding socket at the client.
int SocketSendYield(const int fd,
                    const char * const buf,
                    const int bufLen,
                    int * prevBytesSent);

#ifdef __cplusplus
}
#endif

#endif // SOCKETSEND_H