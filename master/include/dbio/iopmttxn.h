#ifndef IOPMTTXN_H
#define IOPMTTXN_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <pmttxn.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbPmttxnReadKey(const int        id,
                    const int        seqNo,
                          pmttxn_r * const pmttxn);

void DbPmttxnInitRec(pmttxn_r * const pmttxn);
int DbPmttxnPostRead(pmttxn_r * const pmttxn);
int DbPmttxnHideDocNo(const pmttxn_r * const pmttxn,
                            string_t   docNo[SZ_PMT_DOC_NO + 1]);

#ifdef __cplusplus
}
#endif

#endif	// IOPMTTXN_H