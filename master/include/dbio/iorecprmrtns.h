#ifndef IORECPRMRTNS_H
#define IORECPRMRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <recprm.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbRecprmSelectKey(const string_t id[SZ_RECPRM_ID + 1], recprm_r * const recprm);

#ifdef __cplusplus
}
#endif

#endif	// IORECPRMRTNS_H