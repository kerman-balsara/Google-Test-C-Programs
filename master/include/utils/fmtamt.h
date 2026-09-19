#ifndef FMTAMT_H
#define FMTAMT_H

// ---------------------
// Internal definitions.
// ---------------------

#include <stdint.h>

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

string_t * FmtAmt(const int64_t amt, const int places);

#ifdef __cplusplus
}
#endif

#endif // FMTAMT_H
