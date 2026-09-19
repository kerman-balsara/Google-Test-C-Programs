#include <stddef.h>

#include <string.h>

#include <logpkgdefs.h>
#include <strlstdefs.h>
#include <typedefs.h>

#include <csvrtns.h>
#include <lnklstrtns.h>
#include <logpkgcmds.h>
#include <logpkgrtns.h>
#include <logrtns.h>
#include <strutils.h>

static void procPkgCmds(constr_t cmd, strlst_t ** head, strlst_t ** tail);

// --------------------------------------------------------------------
// Process package commands to enable/disable logging for packages.
// We expect a comma separated list of packages as follows:
// +MEMORY,-WIRE which means enable logging for package MEMORY
// and disable logging for package WIRE. Note that LOGLVL_TRACE
// has to be enabled separately to see package level logging.
// For duplicates, the subsequent overrides the previous.
// Invalid entries are ignored.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logpkgcmds.cpp
// --------------------------------------------------------------------
void LogPkgCmdsProc(constr_t cmd)
{
    if (!*cmd || *cmd == '\n')
        return;
    strlst_t * head = NULL;
    strlst_t * tail = NULL;

    procPkgCmds(cmd, &head, &tail);

    LnkLstFree(__func__, &head);
}

static void procPkgCmds(constr_t cmd, strlst_t ** head, strlst_t ** tail)
{
    (void) CsvStrLst(cmd, head);

    for (strlst_t * elem = *head; elem; elem = elem->next)
    {
        if (*elem->str != '+' && *elem->str != '-')
        {
            LogWarnF("%s ignored: First character must be '+' or '-'", elem->str);
            continue;
        }

        logpkg_t logpkg = LogPkgFromStr(elem->str + 1);
        if (logpkg == LOGPKG_NONE)
        {
            LogWarnF("Invalid package %s", elem->str + 1);
        }
        else
        {
            string_t *(*LogPkg)(logpkg_t) = (*elem->str == '+') ? LogPkgEnable : LogPkgDisable;

            (void) LogPkg(logpkg);
        }
    }
}
