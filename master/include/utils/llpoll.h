#ifndef LLPOLL_H
#define LLPOLL_H

#include <poll.h>

// ---------------------
// Internal definitions.
// ---------------------

#ifdef __cplusplus
extern "C" {
#endif

int llpoll(struct pollfd *fds, nfds_t nfds, int timeout);

#ifdef __cplusplus
}
#endif

#endif // LLPOLL_H