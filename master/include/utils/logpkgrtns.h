#ifndef LOGPKGRTNS_H
#define LOGPKGRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <logpkgdefs.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

string_t *LogPkgEnable(logpkg_t logpkg);
string_t *LogPkgDisable(logpkg_t logpkg);
logpkg_t LogPkgFromStr(constr_t logpkgStr);
string_t *LogPkgInfo(void);
void LogPkgTraceF(logpkg_t logpkg, constr_t format, ...);
void LogPkgTrace(logpkg_t logpkg, constr_t str);

#ifdef __cplusplus
}
#endif

#endif // LOGPKGRTNS_H