// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>

#include <iopersoncipher.h>

// --------------------------------------------------------------------
// Decrypt data before returning it.
//
// Return:
// 0 - Valid
// < 0 - Major error
// --------------------------------------------------------------------

int DbPersonPostReadDecrypt(const person_r * const person,
                                  string_t   emailAddress[SZ_EMAIL_ADDRESS + 1])
{
    // Decrypt email address using the database
    return(0);
}
