#ifndef SERVERMSGDEFS_H
#define SERVERMSGDEFS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <bankmsgdefs.h>

typedef struct
{
    char initialMsg;
    bankmsg_t bankmsg;
    char status[SZ_SERVER_STATUS];
} servermsg_t;

#endif	// SERVERMSGDEFS_H