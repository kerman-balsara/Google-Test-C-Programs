#include <pthread.h>
#include <string.h>

#include <errno.h>
#include <stddef.h>

#include <errcodes.h>
#include <typedefs.h>

#include <fmtstr.h>
#include <lnklstrtns.h>
#include <memrtns.h>
#include <sysexit.h>
#include <thrinfo.h>

typedef struct thrinfo_s
{
    struct thrinfo_s *next;
    pthread_t id;
    string_t name[];
} thrinfo_t;

static thrinfo_t * thrinfoHead = NULL;
static thrinfo_t * thrinfoTail = NULL;

// --------------------------------------------------------------------
// Add passed id and name to a list.
// Used for logging in a multithreaded program where the thread name
// is also shown.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/thrinfo.cpp
// --------------------------------------------------------------------

void ThrInfoAdd(constr_t caller, const pthread_t id, constr_t name)
{
    int len = strlen(name) + 1;
    thrinfo_t * thrinfoElem = MemAlloc(caller, sizeof(thrinfo_t) + len);
    thrinfoElem->id = id;
    strlcpy(thrinfoElem->name, name, len);
    LnkLstAppend(thrinfoElem, &thrinfoHead, &thrinfoTail);
}

// --------------------------------------------------------------------
// Remove passed id from the list.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/thrinfo.cpp
// --------------------------------------------------------------------

void ThrInfoRemove(constr_t caller, const pthread_t id)
{
    thrinfo_t * thrinfoElem;
    for (thrinfoElem = thrinfoHead; thrinfoElem; thrinfoElem = thrinfoElem->next)
    {
        if (id == thrinfoElem->id)
            LnkLstRemove(caller, &thrinfoHead, &thrinfoElem, &thrinfoTail);
    }

}

// --------------------------------------------------------------------
// Free the list.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/thrinfo.cpp
// --------------------------------------------------------------------

void ThrInfoFree(constr_t caller)
{
    LnkLstFree(caller, &thrinfoHead);
}

// --------------------------------------------------------------------
// Return name for the passed id in the list. If not found, return "Main".
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/thrinfo.cpp
// --------------------------------------------------------------------

string_t * ThrInfoFind(const pthread_t id)
{
    thrinfo_t * thrinfoElem;
    for (thrinfoElem = thrinfoHead; thrinfoElem; thrinfoElem = thrinfoElem->next)
    {
        if (id == thrinfoElem->id)
            return(FmtStr(thrinfoElem->name));
    }

    return(FmtStr("Main"));
}