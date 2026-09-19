// ---------------------
// Internal definitions.
// ---------------------

#include <person.h>

#include <iopersonrtns.h>

// --------------------------------------------------------------------
// Read from table.
//
// Return:
// 0 - Valid
// 1 - Invalid
// < 0 - Major error
// --------------------------------------------------------------------

int DbPersonSelectKey(const int id, person_r * const person)
{
    person->id = id;
    // Set up record (as would have been done by the SQL)
    return(1);
}
