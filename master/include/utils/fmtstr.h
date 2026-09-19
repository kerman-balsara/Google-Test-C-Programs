#ifndef FMTSTR_H
#define FMTSTR_H

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

string_t * FmtStr(const string_t * const format, ...);

#ifdef __cplusplus
}
#endif

#endif // FMTSTR_H