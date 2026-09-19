#ifndef STRLSTDEFS_H
#define STRLSTDEFS_H

#include <typedefs.h>

typedef struct strlst_struct
{
    struct strlst_struct *next;
    string_t str[];
} strlst_t;

#endif // STRLSTDEFS_H