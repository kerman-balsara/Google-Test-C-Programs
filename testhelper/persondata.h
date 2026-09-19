#ifndef PERSONDATA_H
#define PERSONDATA_H

// ---------------------
// Internal definitions.
// ---------------------

#include <person.h>

#include <syssizes.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void PersonDataE(const int id, person_r * const person);
void PersonDataP(const int id, string_t emailAddress[SZ_EMAIL_ADDRESS + 1]);

#ifdef __cplusplus
}
#endif

#endif	// PERSONDATA_H