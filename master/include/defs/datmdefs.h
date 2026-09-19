#ifndef DATMDEFS_H
#define DATMDEFS_H

// ---------------------
// Internal definitions.
// ---------------------

typedef struct
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} datm_t;

#endif	// DATMDEFS_H