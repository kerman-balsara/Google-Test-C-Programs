#include <errno.h>
#include <stddef.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <pthread.h>

#include <errcodes.h>
#include <logpkgdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <fmtstr.h>
#include <logpkgrtns.h>
#include <logrtns.h>
#include <sysexit.h>

typedef struct
{
    constr_t logpkgStr;
    logpkg_t logpkg;
    bool_t   enabled;
} pkgwrite_t;

static pkgwrite_t PkgWrite[] =
{
    { "MEMORY", LOGPKG_MEMORY, 0 },
    { "WIRE", LOGPKG_WIRE, 0 },
    { NULL }
};

// --------------------------------------------------------------------
// Enable logging for the passed package.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logpkgrtns.cpp
// --------------------------------------------------------------------

string_t *LogPkgEnable(logpkg_t logpkg)
{
    PkgWrite[logpkg].enabled = 1;
    return(LogPkgInfo());
}

// --------------------------------------------------------------------
// Disable logging for the passed package.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logpkgrtns.cpp
// --------------------------------------------------------------------

string_t *LogPkgDisable(logpkg_t logpkg)
{
    PkgWrite[logpkg].enabled = 0;
    return(LogPkgInfo());
}

// --------------------------------------------------------------------
// Get package for the passed string.
// For e.g., for passed string "MEMORY", LOGPKG_MEMORY is returned.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logpkgrtns.cpp
// --------------------------------------------------------------------

logpkg_t LogPkgFromStr(constr_t logpkgStr)
{
    for (pkgwrite_t * pw = PkgWrite; pw->logpkgStr; pw++)
    {
        if (!strcasecmp(logpkgStr, pw->logpkgStr))
            return(pw->logpkg);
    }

    return(LOGPKG_NONE);
}

// --------------------------------------------------------------------
// Get info for all packages whether enabled or disabled.
// Returned string would be similar to "-MEMORY,+WIRE" indicating that
// logging is disabled for LOGLVL_MEMORY and enabled for LOGLVL_WIRE.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logpkgrtns.cpp
// --------------------------------------------------------------------

string_t *LogPkgInfo(void)
{
    string_t pkgInfo[SZ_LOGPKG_INFO];
    memset(pkgInfo, 0, sizeof(pkgInfo));
    string_t *pkgInfoPtr = pkgInfo; 
    for (pkgwrite_t * pw = PkgWrite; pw->logpkgStr; pw++)
    {
        if (pkgInfoPtr != pkgInfo)
        {
            *pkgInfoPtr = ',';
            pkgInfoPtr++;
        }
        *pkgInfoPtr = (pw->enabled) ? '+' : '-';
        pkgInfoPtr++;
        int len = strlen(pw->logpkgStr);
        memcpy(pkgInfoPtr, pw->logpkgStr, len);
        pkgInfoPtr += len;
    }

    return(FmtStr(pkgInfo));
}

// --------------------------------------------------------------------
// Log a message for the passed package.
// Note that for a message to appear on the log, logging for the
// package must be enabled AND LOGLVL_TRACE must be enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logpkgrtns.cpp
// --------------------------------------------------------------------

void LogPkgTraceF(logpkg_t logpkg, constr_t format, ...)
{
    if (!PkgWrite[logpkg].enabled)
        return;

    string_t str[SZ_LOGMSG_STR];
    memset(str, 0, sizeof(str));

    va_list arg;
    va_start(arg, format);
    int ret = vsnprintf(str, sizeof(str), format, arg);
    if (ret < 0)
    {
        int sysErrNo = errno;
        SysExit(__func__, sysErrNo, SYSCALL_VSNPRINTF);
    }
    va_end(arg);

    LogTrace(str);
}

// --------------------------------------------------------------------
// Log a message for the passed package.
// Note that for a message to appear on the log, logging for the
// package must be enabled AND LOGLVL_TRACE must be enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logpkgrtns.cpp
// --------------------------------------------------------------------

void LogPkgTrace(logpkg_t logpkg, constr_t str)
{
    if (!PkgWrite[logpkg].enabled)
        return;

    LogTrace(str);
}