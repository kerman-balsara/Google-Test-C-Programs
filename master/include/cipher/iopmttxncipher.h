#ifndef IOPMTTXNCIPHER_H
#define IOPMTTXNCIPHER_H

// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <pmttxn.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbPmttxnPostReadDecrypt(const pmttxn_r * const pmttxn,
                                  string_t   docNo[SZ_PMT_DOC_NO + 1]);

#ifdef __cplusplus
}
#endif

#endif	// IOPMTTXNCIPHER_H