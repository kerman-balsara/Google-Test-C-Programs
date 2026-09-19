#ifndef ERRMSGRTNS_H
#define ERRMSGRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <errmsgdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void ErrmsgSet(const int code, constr_t message, errmsg_t * const errmsg);
void ErrmsgInit(errmsg_t * const errmsg);

#ifdef __cplusplus
}
#endif

#endif // ERRMSGRTNS_H