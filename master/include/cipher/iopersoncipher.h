#ifndef IOPERSONCIPHER_H
#define IOPERSONCIPHER_H

// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbPersonPostReadDecrypt(const person_r * const person,
                                  string_t   emailAddress[SZ_EMAIL_ADDRESS + 1]);

#ifdef __cplusplus
}
#endif

#endif	// IOPERSONCIPHER_H