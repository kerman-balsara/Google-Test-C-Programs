#ifndef FMTDATE_H
#define FMTDATE_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

string_t * FmtDateDDMonYYYY(const string_t inDate[SZ_DATETIME_HMS + 1]);

#ifdef __cplusplus
}
#endif

#endif // FMTDATE_H