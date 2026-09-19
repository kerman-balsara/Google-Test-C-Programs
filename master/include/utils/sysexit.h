#ifndef SYSEXIT_H
#define SYSEXIT_H

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void SysExit(constr_t functionName, const int sysErrNo, const int errCode);
void SysSetPrintStack(const bool_t value);
void SysPstack(void);

#ifdef __cplusplus
}
#endif

#endif // SYSEXIT_H