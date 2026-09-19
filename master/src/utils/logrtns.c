#include <errno.h>
#include <stddef.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

#include <errcodes.h>
#include <loglvldefs.h>
#include <typedefs.h>

#include <datmutils.h>
#include <fmtstr.h>
#include <logrtns.h>
#include <sysexit.h>
#include <thrinfo.h>

typedef struct
{
    constr_t loglvlStr;
    char loglvlChar;
    int      loglvl;
    bool_t   enabled;
} logwrite_t;

// Error logging is enabled by default.
static logwrite_t LogWrite[] =
{
    { "ERROR", 'E', LOGLVL_ERROR, 1 },
    { "WARN", 'W', LOGLVL_WARN, 1 },
    { "INFO", 'I', LOGLVL_INFO, 0 },
    { "TRACE", 'T', LOGLVL_TRACE, 0 },
    { NULL }
};

static void logmsg(constr_t loglvlStr, constr_t str);
static bool_t loglvlEnabled(constr_t loglvlStr);

// --------------------------------------------------------------------
// Enable passed loglvl. This can be a single value or multiple values
// OR'ed together. For values, see
// file:///home/kerman/projects/GoogleTestC/master/include/defs/loglvldefs.h
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

int LogLvlEnable(const int loglvl)
{
    int loglvlsEnabled = 0;
    for (logwrite_t * lw = LogWrite; lw->loglvlStr; lw++)
    {
        if (lw->loglvl & loglvl)
            lw->enabled = 1;

        if (lw->enabled)
            loglvlsEnabled |= lw->loglvl;
               
    }

    return(loglvlsEnabled);
}

// --------------------------------------------------------------------
// Disable passed loglvl. This can be a single value or multiple values
// OR'ed together. For values, see
// file:///home/kerman/projects/GoogleTestC/master/include/defs/loglvldefs.h
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

int LogLvlDisable(const int loglvl)
{
    int loglvlsDisabled = 0;
    for (logwrite_t * lw = LogWrite; lw->loglvlStr; lw++)
    {
        if (lw->loglvl & loglvl)
            lw->enabled = 0;

        if (!lw->enabled)
            loglvlsDisabled |= lw->loglvl;    
    }

    return(loglvlsDisabled);
}

// --------------------------------------------------------------------
// Return log levels enabled. This can be a single value or multiple values
// OR'ed together. For values, see
// file:///home/kerman/projects/GoogleTestC/master/include/defs/loglvldefs.h
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

int LogLvlsEnabled(void)
{
    int loglvlsEnabled = 0;
    for (logwrite_t * lw = LogWrite; lw->loglvlStr; lw++)
    {
        if (lw->enabled)
            loglvlsEnabled |= lw->loglvl;
    }
    return(loglvlsEnabled);
}

// --------------------------------------------------------------------
// Return log levels enabled/disabled in string form.
// For e.g., if LOGLVL_ERROR and LOGLVL_INFO are enabled and the other
// log levels are disabled, the returned string is "+E,-W,+I,-T".
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

string_t * LogLvlsInfo(void)
{
    string_t loglvls[(LOGLVL_COUNT * 3) + 1];
    memset(loglvls, 0, sizeof(loglvls));
    char * loglvlsPtr = loglvls;
    for (logwrite_t * lw = LogWrite; lw->loglvlStr; lw++)
    {
        if (loglvlsPtr != loglvls)
        {
            *loglvlsPtr = ',';
            loglvlsPtr++;
        }
        *loglvlsPtr = (lw->enabled) ? '+' : '-';
        loglvlsPtr++;
        *loglvlsPtr = lw->loglvlChar;
        loglvlsPtr++;
    }
    return(FmtStr("Current log levels are %s", loglvls));
}

// --------------------------------------------------------------------
// Return log level based on the passed loglvlChar.
// For e.g., if passed character is "e", we return LOGLVL_ERROR.
// For a loglvlChar other than the one specified in LogWrite[],
// 0 is returned.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

int LoglvlFromChar(const char loglvlChar)
{
    for (logwrite_t * lw = LogWrite; lw->loglvlStr; lw++)
    {
        if (lw->loglvlChar == toupper(loglvlChar))
            return(lw->loglvl);
    }

    return(0);
}

// --------------------------------------------------------------------
// Log an "ERROR" message if LOGLVL_ERROR is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogErrorF(constr_t format, ...)
{
    if (!loglvlEnabled("ERROR"))
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

    LogError(str);
}

// --------------------------------------------------------------------
// Log an "ERROR" message if LOGLVL_ERROR is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogError(constr_t str)
{
    if (loglvlEnabled("ERROR"))
        logmsg("ERROR", str);
}

// --------------------------------------------------------------------
// Log a "WARN" message if LOGLVL_WARN is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogWarnF(constr_t format, ...)
{
    if (!loglvlEnabled("WARN"))
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

    LogWarn(str);
}

// --------------------------------------------------------------------
// Log a "WARN" message if LOGLVL_WARN is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogWarn(constr_t str)
{
    if (loglvlEnabled("WARN"))
        logmsg("WARN", str);
}

// --------------------------------------------------------------------
// Log an "INFO" message if LOGLVL_INFO is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogInfoF(constr_t format, ...)
{
    if (!loglvlEnabled("INFO"))
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

    LogInfo(str);
}

// --------------------------------------------------------------------
// Log an "INFO" message if LOGLVL_INFO is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogInfo(constr_t str)
{
    if (loglvlEnabled("INFO"))
        logmsg("INFO", str);
}

// --------------------------------------------------------------------
// Log a "TRACE" message if LOGLVL_TRACE is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogTraceF(constr_t format, ...)
{
    if (!loglvlEnabled("TRACE"))
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
// Log a "TRACE" message if LOGLVL_TRACE is enabled.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logrtns.cpp
// --------------------------------------------------------------------

void LogTrace(constr_t str)
{
    if (loglvlEnabled("TRACE"))
        logmsg("TRACE", str);
}

static void logmsg(constr_t loglvlStr, constr_t str)
{
    fprintf(stderr, "%s %-5s %-20s %s\n", DatmNowHms(), loglvlStr, ThrInfoFind(pthread_self()), str);
}

static bool_t loglvlEnabled(constr_t loglvlStr)
{
    for (logwrite_t * lw = LogWrite; lw->loglvlStr; lw++)
    {
        if (!strcmp(loglvlStr, lw->loglvlStr))
            return(lw->enabled);
    }

    return(0);
}

