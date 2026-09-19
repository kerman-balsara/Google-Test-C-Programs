#ifndef DBMSGDEFS_H
#define DBMSGDEFS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>

typedef struct
{
    char id[SZ_DB_ID];
    char auditNo[SZ_BANK_AUDIT_NO];
} dbmsg_t;

#endif	// DBMSGDEFS_H