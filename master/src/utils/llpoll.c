// --------------------------------------------------------------------
// Wrappers to allow mocks.
// --------------------------------------------------------------------

#include <poll.h>

#include <llpoll.h>

int llpoll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    return(poll(fds, nfds, timeout));
}
