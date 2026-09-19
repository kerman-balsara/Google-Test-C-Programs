#ifndef LOCKFILERTNS_H
#define LOCKFILERTNS_H

#include <unistd.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void LockfileCheck(constr_t LockfileName);
void LockfileWritePid(constr_t LockfileName, const pid_t pid);
void LockfileDelete(constr_t LockfileName);

#ifdef __cplusplus
}
#endif

#endif // LOCKFILERTNS_H