#ifndef IORECPRM_H
#define IORECPRM_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <recprm.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbRecprmReadKey(const string_t id[SZ_RECPRM_ID + 1],
                          recprm_r * const recprm);
void DbRecprmInitRec(recprm_r * const recprm);

#ifdef __cplusplus
}
#endif

#endif	// IORECPRM_H