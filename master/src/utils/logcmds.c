#include <stddef.h>

#include <ctype.h>
#include <string.h>

#include <loglvldefs.h>
#include <strlstdefs.h>
#include <typedefs.h>

#include <csvrtns.h>
#include <lnklstrtns.h>
#include <logrtns.h>

static void procLogCmds(constr_t cmd, strlst_t ** head, strlst_t ** tail);

// --------------------------------------------------------------------
// Process log commands to enable/disable logging.
// We expect a comma separated list as follows:
// +T,-W,+E
// which means enable LOGLVL_TRACE, disable LOGLVL_WARN,
// enable LOGLVL_ERROR. LOGLVL_INFO is left as previously set.
// For duplicates, the subsequent overrides the previous.
// Invalid entries are ignored.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/logcmds.cpp
// --------------------------------------------------------------------

void LogCmdsProc(constr_t cmd)
{
    if (!*cmd || *cmd == '\n')
        return;
    strlst_t * head = NULL;
    strlst_t * tail = NULL;

    procLogCmds(cmd, &head, &tail);

    LnkLstFree(__func__, &head);
}

static void procLogCmds(constr_t cmd, strlst_t ** head, strlst_t ** tail)
{
    (void) CsvStrLst(cmd, head);

    for (strlst_t * elem = *head; elem; elem = elem->next)
    {
        if (strlen(elem->str) > 2)
        {
            LogWarnF("%s ignored: Length must be 2", elem->str);
            continue;
        }

        if (*elem->str != '+' && *elem->str != '-')
        {
            LogWarnF("%s ignored: First character must be '+' or '-'", elem->str);
            continue;
        }

        int (*loglvlfn) (const int) = (*elem->str == '+') ? LogLvlEnable : LogLvlDisable;
        int loglvl = LoglvlFromChar(elem->str[1]);

        if (loglvl)
        {
            loglvlfn(loglvl);
        }
        else
        {
            LogWarnF("%s ignored: Log level is invalid", elem->str);
        }
    }
}
