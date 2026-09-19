#include <stddef.h>

#include <string.h>

#include <errcodes.h>
#include <strlstdefs.h>
#include <typedefs.h>

#include <csvrtns.h>
#include <lnklstrtns.h>
#include <memrtns.h>
#include <sysexit.h>

// --------------------------------------------------------------------
// Break passed comma separated csvstr into a strlst_t.
// String cannot contain single or double quotes.
//
// Return: Number of items in strlst_t.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/csvrtns.cpp
// --------------------------------------------------------------------

int CsvStrLst(const string_t * csvstr, strlst_t ** strlstHead)
{
    if (strchr(csvstr, '\'') || strchr(csvstr, '"'))
        SysExit(__func__, 0, STR_QUOTES_INVALID);

    int items = 0;
    string_t * saveptr;
    strlst_t * strlstTail = NULL;
    for (string_t * csvstrptr = (string_t *) csvstr; ; csvstrptr = NULL)
    {
        string_t * token = strtok_r(csvstrptr, ",", &saveptr);
        if (token == NULL)
            break;
        ++items;
        int tokenLen = strlen(token) + 1;
        strlst_t * strlstElem = MemAlloc("CsvStrLst", sizeof(strlst_t) + tokenLen);
        strlcpy(strlstElem->str, token, tokenLen);
        LnkLstAppend(strlstElem, strlstHead, &strlstTail);
    }

    return(items);
}