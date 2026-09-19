// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>

#include <ioperson.h>
#include <persondata.h>

// Return fake data with encrypted emailAddress
void PersonDataE(const int id, person_r * const person)
{
    DbPersonInitRec(person);
    person->id = id;
    strlcpy(person->name, "Name", sizeof(person->name));
    strlcpy(person->lastname, "Lastname", sizeof(person->lastname));
    strlcpy(person->emailAddress, "AABBCCDDEEFFGGHHII", sizeof(person->emailAddress));
}

// Return fake decrypted emailAddress
void PersonDataP(const int id, string_t emailAddress[SZ_EMAIL_ADDRESS + 1])
{
    memset(emailAddress, 0, SZ_EMAIL_ADDRESS + 1);
    snprintf(emailAddress, SZ_EMAIL_ADDRESS + 1, "kbtest%d@gmail.com", id);
}
