#ifndef HASHPWD_H
#define HASHPWD_H

// ---------------------
// External definitions.
// ---------------------

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

string_t * HashPwd(constr_t passwd);
int ValidatePwd(constr_t passwd, constr_t setting);

#ifdef __cplusplus
}
#endif

#endif	// HASHPWD_H