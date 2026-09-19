// ---------------------
// External definitions.
// ---------------------

#include <sys/stat.h>

#include <errno.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <typedefs.h>

#include <llstdio.h>
#include <pathstatus.h>
#include <sysexit.h>

// --------------------------------------------------------------------
// Check if passed path exists.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/library/pathstatus.cpp
// file:///home/kerman/projects/GoogleTestC/testmock/library/pathstatus.cpp
// --------------------------------------------------------------------

bool_t PathExists(constr_t path, struct stat * const pathStatus)
{
    if (llstat(path, pathStatus))
    {
        if (errno == ENOENT)
            return(0);
        int sysErrNo = errno;
        SysExit(__func__, sysErrNo, SYSCALL_STAT);
    }

    return(1);
}

