// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <recprm.h>

#include <iorecprmrtns.h>

// --------------------------------------------------------------------
// Read from table.
//
// Return:
// 0 - Valid
// 1 - Invalid
// < 0 - Major error
// --------------------------------------------------------------------

int DbRecprmSelectKey(const string_t id[SZ_RECPRM_ID + 1], recprm_r * const recprm)
{
    strlcpy(recprm->id, id, SZ_RECPRM_ID + 1);
    // Set up record (as would have been done by the SQL)
    return(1);
}
