// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <recprm.h>

#include <iorecprm.h>
#include <iorecprmrtns.h>

// --------------------------------------------------------------------
// Check whether the passed id belongs to a valid recprm.
//
// Return:
// 0 - Valid
// 1 - Invalid
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/dbio/iorecprm.cpp
// --------------------------------------------------------------------

int DbRecprmReadKey(const string_t id[SZ_RECPRM_ID + 1], recprm_r * const recprm)
{
    DbRecprmInitRec(recprm);

    return(DbRecprmSelectKey(id, recprm));
}

// --------------------------------------------------------------------
// Initialise passed record.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/misc/structinit.cpp
// --------------------------------------------------------------------

void DbRecprmInitRec(recprm_r * const recprm)
{
    memset(recprm, 0, sizeof(recprm_r));
}
