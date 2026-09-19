#ifndef PATHSTATUS_H
#define PATHSTATUS_H

// ---------------------
// External definitions.
// ---------------------

#include <sys/stat.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

bool_t PathExists(constr_t path, struct stat * const pathStatus);

#ifdef __cplusplus
}
#endif

#endif // PATHSTATUS_H