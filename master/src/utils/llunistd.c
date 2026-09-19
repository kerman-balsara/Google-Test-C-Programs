// --------------------------------------------------------------------
// Wrappers to allow mocks.
// --------------------------------------------------------------------

#include <unistd.h>

#include <llunistd.h>

int llclose(int fd)
{
    return(close(fd));
}