#ifndef THRINFO_H
#define THRINFO_H

#include <pthread.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void ThrInfoAdd(constr_t caller, const pthread_t id, constr_t name);
void ThrInfoRemove(constr_t caller, const pthread_t id);
void ThrInfoFree(constr_t caller);
string_t * ThrInfoFind(const pthread_t id);

#ifdef __cplusplus
}
#endif

#endif // THRINFO_H