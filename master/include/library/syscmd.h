#ifndef SYSCMD_H
#define SYSCMD_H

// ---------------------
// Internal definitions.
// ---------------------

#include <strlstdefs.h>
#include <typedefs.h>
#include <errmsgdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

int SysCmd(constr_t cmd, errmsg_t * const errmsg);
int SysCmdStrLst(constr_t cmd, strlst_t ** strlstHead, errmsg_t * const errmsg);

#ifdef __cplusplus
}
#endif

#endif // SYSCMD_H