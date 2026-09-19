#ifndef IOPERSONRTNS_H
#define IOPERSONRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <person.h>

#ifdef __cplusplus
extern "C" {
#endif

int DbPersonSelectKey(const int id, person_r * const person);

#ifdef __cplusplus
}
#endif

#endif	// IOPERSONRTNS_H