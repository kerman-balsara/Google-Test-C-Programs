// ---------------------
// External definitions.
// ---------------------

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <cyclicstr.h>
#include <fmtstr.h>
#include <llstdio.h>
#include <logrtns.h>
#include <sysexit.h>

// --------------------------------------------------------------------
// Return a pointer to a formatted string.
//
// Return:
// Pointer to string (uses a cyclic buffer so use immediately)
//
// Exits if formatted string is too long (max SZ_CYCLIC_STR).
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/fmtstr.cpp
// --------------------------------------------------------------------

string_t * FmtStr(const string_t * const format, ...)
{
    string_t str[SZ_CYCLIC_STR];
    memset(str, 0, sizeof(str));

    va_list arg;
    va_start(arg, format);
    int ret = vsnprintf(str, sizeof(str), format, arg);
    va_end(arg);

    if (ret < 0)
    {
        int sysErrNo = errno;
        SysExit(__func__, sysErrNo, SYSCALL_VSNPRINTF);
    }

    if (ret >= sizeof(str))
    {
        LogErrorF("String size: %d", ret);
        SysExit(__func__, 0, STR_SIZE_EXCEEDED);
    }

    return(CyclicStrAdd(str));
}
