// --------------------------------------------------------------------
// Wrappers to allow mocks.
// --------------------------------------------------------------------

#include <fcntl.h>

#include <llfcntl.h>

int llfcntl(int fd, int op, int flags)
{
    return(fcntl(fd, op, flags));
}