#ifndef ERRMSGDEFS_H
#define ERRMSGDEFS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

typedef struct
{
    int      code;                          //   4
    string_t message[SZ_ERRMSG_STR];        // 512
                                            // 516
} errmsg_t;

#endif	// ERRMSGDEFS_H