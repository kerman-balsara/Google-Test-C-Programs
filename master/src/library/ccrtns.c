// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <ccchecksum.h>
#include <ccrtns.h>
#include <strutils.h>

// --------------------------------------------------------------------
// Check whether passed credit card number is valid.
//
// Return:
// 1 - Valid
// 0 - Invalid
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/library/ccrtns.cpp
// --------------------------------------------------------------------

bool_t IsValidCreditCardNo(constr_t ccNo)
{
    if (!IsValidStrLenMinMax(ccNo, SZ_CC_NO_MIN_LEN, SZ_CC_NO_MAX_LEN))
        return(0);

    return(IsValidCreditCardCheckSum(ccNo));
}