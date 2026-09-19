// ---------------------
// External definitions.
// ---------------------

#include <errno.h>
#include <stddef.h>

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <errmsgdefs.h>
#include <strlstdefs.h>
#include <typedefs.h>

#include <errmsgrtns.h>
#include <filertns.h>
#include <fmtstr.h>
#include <llstdio.h>
#include <lnklstrtns.h>
#include <memrtns.h>
#include <strutils.h>
#include <syscmd.h>
#include <sysexit.h>

static int syscmd(constr_t cmd, strlst_t ** strlstHead, errmsg_t * const errmsg);

// --------------------------------------------------------------------
// Run the passed command.
//
// Return:
// 0 - Valid
// 1 - error (message in errmsg)
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/library/syscmd.cpp
// --------------------------------------------------------------------

int SysCmd(constr_t cmd, errmsg_t * const errmsg)
{
    return(syscmd(cmd, NULL, errmsg));
}

int SysCmdStrLst(constr_t cmd, strlst_t ** strlstHead, errmsg_t * const errmsg)
{
    return(syscmd(cmd, strlstHead, errmsg));
}

static int syscmd(constr_t cmd, strlst_t ** strlstHead, errmsg_t * const errmsg)
{
    ErrmsgInit(errmsg);

    string_t wkCmd[strlen(cmd) + 1];
    strlcpy(wkCmd, cmd, sizeof(wkCmd));

    FILE * fPtr = llpopen(FmtStr("%s 2>&1", StrRmTrailNewLine(wkCmd)), "r");
    if (fPtr == NULL)
        SysExit(__func__, errno, SYSCALL_POPEN);

    // Save result of first call to llfgets(). This can contain an error message or valid output of command. We can only
    // know which when llpclose() is called.
    string_t firstMessage[SZ_SYSCMD_STR];
    *firstMessage = 0;

    strlst_t * strlstTail = NULL;
 
    // Process the (error) message only if pclose returns 1.
    for (;;)
    {
        string_t message[SZ_SYSCMD_STR];
        string_t * result = llfgets(message, SZ_SYSCMD_STR, fPtr);
        if (result == NULL)
        {
            if (llferror(fPtr))
                SysExit(__func__, 0, SYSCALL_FGETS);
            break;
        }

        if (!*firstMessage)
            strlcpy(firstMessage, result, sizeof(firstMessage));

        if (strlstHead)
        {
            int resultLen = strlen(result) + 1;
            strlst_t * strlstElem = MemAlloc("syscmd", sizeof(strlst_t) + resultLen);
            strlcpy(strlstElem->str, result, resultLen);
            LnkLstAppend(strlstElem, strlstHead, &strlstTail);
        }
        else
        {
            break;
        }
    }
    int pcloseRet = llpclose(fPtr);
    if (pcloseRet && errmsg->code == 0)
    {
        if (pcloseRet == -1)
            SysExit(__func__, errno, SYSCALL_PCLOSE);

        ErrmsgSet(SYSCMD_FAILED, FmtStr("%s", StrRmTrailNewLine(firstMessage)), errmsg);
    }

    return(errmsg->code ? 1 : 0);
}
