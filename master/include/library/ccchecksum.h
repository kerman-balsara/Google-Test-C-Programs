#ifndef CCCHECKSUM_H
#define CCCHECKSUM_H

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

bool_t IsValidCreditCardCheckSum(constr_t ccNo);

#ifdef __cplusplus
}
#endif

#endif // CCCHECKSUM_H