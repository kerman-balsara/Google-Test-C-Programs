#ifndef PMTTXNDATA_H
#define PMTTXNDATA_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <pmttxn.h>

#ifdef __cplusplus
extern "C" {
#endif

void PmttxnDataE(const int id, const int seqNo, pmttxn_r * const pmttxn);
void PmttxnDataP(const int id, const int seqNo, string_t docNo[SZ_PMT_DOC_NO + 1]);

#ifdef __cplusplus
}
#endif

#endif	// PMTTXNDATA_H