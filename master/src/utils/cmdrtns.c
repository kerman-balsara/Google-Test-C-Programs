#include <string.h>
#include <stdio.h>

#include <typedefs.h>

#include <cmdrtns.h>
#include <logcmds.h>
#include <logpkgcmds.h>
#include <logpkgrtns.h>
#include <logrtns.h>
#include <memrtns.h>
#include <strutils.h>

static int fmtResponse(constr_t str, string_t ** response);

// --------------------------------------------------------------------
// Testing
// file:///home/kerman/projects/GoogleTestC/test/utils/cmdproc.cpp
//
// --------------------------------------------------------------------
int CmdProc(const buffer_t * const cmd, const int cmdLen, string_t ** response)
{
    if (cmdLen < 4)
        return(fmtResponse("ERROR: Invalid request (length)", response));

    string_t wkCmd[cmdLen + 1];
    (void) BufToStr(cmd, cmdLen, wkCmd);

    BufToUpperCase(wkCmd, cmdLen);

    if (!memcmp(wkCmd, "?LOG:", 5))
    {
        LogCmdsProc(wkCmd+5);
        return(fmtResponse(LogLvlsInfo(), response));
    }
    else if (cmdLen == 4 && !memcmp(wkCmd, "?LOG", 4))
    {
        return(fmtResponse(LogLvlsInfo(), response));
    }
    else if (!memcmp(wkCmd, "?PKG:", 5))
    {
        LogPkgCmdsProc(wkCmd+5);
        return(fmtResponse(LogPkgInfo(), response));
    }
    else if (cmdLen == 4 && !memcmp(wkCmd, "?PKG", 4))
    {
        return(fmtResponse(LogPkgInfo(), response));
    }
    else
    {
        return(fmtResponse("ERROR: Invalid request (unknown)", response));
    }
}

static int fmtResponse(constr_t str, string_t ** response)
{
    int len = strlen(str) + 1;
    *response = MemAlloc("cmdrtns", len);
    strlcpy(*response, str, len);
    return(len - 1);
}