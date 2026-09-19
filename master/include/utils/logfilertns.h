#ifndef LOGFILERTNS_H
#define LOGFILERTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void LogfileOpen(constr_t LogfileNamePrefix, string_t LogfileName[SZ_FULL_PATH_NAME], int * const LogfileFd);
void LogfileClose(int * const LogfileFd);

#ifdef __cplusplus
}
#endif

#endif // LOGFILERTNS_H