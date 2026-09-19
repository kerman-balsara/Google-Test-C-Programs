#ifndef IOPMTTXNRTNS_H
#define IOPMTTXNRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <pmttxn.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbPmttxnSelectKey(const int id, const int seqNo, pmttxn_r * const pmttxn);

#ifdef __cplusplus
}
#endif

#endif	// IOPMTTXNRTNS_H