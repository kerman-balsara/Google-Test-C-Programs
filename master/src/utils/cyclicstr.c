// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <cyclicstr.h>
#include <logrtns.h>
#include <sysexit.h>

static __thread string_t   CyclicStr[SZ_CYCLIC_STR];
static __thread string_t * CyclicPtr;

// --------------------------------------------------------------------
// Return a pointer to passed str in a cyclic buffer.
//
// Return:
// Pointer to string (uses a cyclic buffer so use immediately)
//
// Exits if passed string is too long to fit in the cyclic buffer.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/cyclicstr.cpp
// --------------------------------------------------------------------

string_t * CyclicStrAdd(constr_t str)
{
    int strLenPlusNull = strlen(str) + 1;

    if (strLenPlusNull > SZ_CYCLIC_STR)
    {
        LogErrorF("String size: %d", strLenPlusNull);
        SysExit(__func__, 0, STR_SIZE_EXCEEDED);
    }

    if (!CyclicPtr || (CyclicPtr + strLenPlusNull > CyclicStr + SZ_CYCLIC_STR))
    {
    	CyclicPtr = CyclicStr;
    }

    string_t * rtnPtr = CyclicPtr;
    CyclicPtr += strLenPlusNull;
    memcpy(rtnPtr, str, strLenPlusNull);

    return(rtnPtr);
}
