#ifndef IOPERSON_H
#define IOPERSON_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbPersonReadKey(const int id,
                          person_r * const person);

int DbPersonReadFile(const int id,
                           person_r * const person);

void DbPersonInitRec(person_r * const person);
int DbPersonPostRead(person_r * const person);

int DbPersonHideEmail(const person_r * const person,
                            string_t   emailAddress[SZ_EMAIL_ADDRESS + 1]);
int DbPersonShowEmail(const person_r * const person,
                            string_t   emailAddress[SZ_EMAIL_ADDRESS + 1]);

#ifdef __cplusplus
}
#endif

#endif	// IOPERSON_H