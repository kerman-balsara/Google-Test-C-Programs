#ifndef PERSON_H
#define PERSON_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

typedef struct person_r
{
    int id;                                             // 4

    string_t name[SZ_PERSON_NAME + 1];                  // 61
    string_t lastname[SZ_PERSON_LASTNAME + 1];          // 61
    string_t emailAddress[SZ_EMAIL_ADDRESS + 1];        // 51

    char padding[3];                                    // 3
                                                        // 180
} person_r;

#endif	// PERSON_H