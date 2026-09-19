#ifndef RECEIPTWRITE_H
#define RECEIPTWRITE_H

// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#ifdef __cplusplus
extern "C" {
#endif

void ReceiptFileWrite(const person_r * const person,
                      const pmttxn_r * const pmttxn,
                      const recprm_r * const recprm,
                      const string_t   toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                            FILE     * fPtr);
                           
#ifdef __cplusplus
}
#endif

#endif // RECEIPTWRITE_H