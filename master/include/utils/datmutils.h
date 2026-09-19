#ifndef DATMUTILS_H
#define DATMUTILS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <datmdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

string_t * DatmNowHms(void);
string_t * DatmNowHmsFilename(void);
void DatmHmsParse(const string_t inDate[SZ_DATETIME_HMS + 1], datm_t * const datm);

#ifdef __cplusplus
}
#endif

#endif // DATMUTILS_H