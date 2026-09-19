#ifndef BANKMSGDEFS_H
#define BANKMSGDEFS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

typedef struct
{
    char auditNo[SZ_BANK_AUDIT_NO];
    char ccNo[SZ_CC_NO_MAX_LEN];
    char status[SZ_BANK_STATUS];
} bankmsg_t;

#endif	// BANKMSGDEFS_H