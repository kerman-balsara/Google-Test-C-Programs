#ifndef STRUTILS_H
#define STRUTILS_H

#include <stdarg.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

bool_t IsValidStrLenMinMax(constr_t str,
                           const int minLen,
                           const int maxLen);

void EmailAddressUserHide(string_t emailAddress[SZ_EMAIL_ADDRESS + 1]);
void PmtDocNoHide(string_t docNo[SZ_PMT_DOC_NO + 1]);
string_t * StrRmTrailNewLine(string_t * const str);
void ShortToBuf(const short value, buffer_t buf[2]);
short BufToShort(const buffer_t buf[2]);
void BufPrintf(buffer_t * const buf, const int bufLen, constr_t format, ...);
// Returns length of string
int BufToStr(const buffer_t * const buf, const int bufLen, string_t * const str);
// Returns length of buffer
int StrToBuf(constr_t str, buffer_t * const buf, const int bufLen);
// Returns length of buffer
int BufLen(const buffer_t * const buf, const int bufLen);
void BufToUpperCase(buffer_t * const buf, const int bufLen);

#ifdef __cplusplus
}
#endif

#endif // STRUTILS_H