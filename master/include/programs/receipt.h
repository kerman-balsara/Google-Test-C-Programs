#ifndef RECEIPT_H
#define RECEIPT_H

// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errmsgdefs.h>
#include <receiptdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#ifdef __cplusplus
extern "C" {
#endif

int ReceiptMain(const int        personId,
                const int        pmttxnSeqNo,
                const string_t   recprmId[SZ_RECPRM_ID + 1],
                      errmsg_t * const errmsg);

#ifdef __cplusplus
}
#endif

#endif // RECEIPT_H