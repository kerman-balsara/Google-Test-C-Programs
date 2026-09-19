#ifndef CSVRTNS_H
#define CSVRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <strlstdefs.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

int CsvStrLst(const string_t * str, strlst_t ** strlstHead);

#ifdef __cplusplus
}
#endif

#endif // CSVRTNS_H