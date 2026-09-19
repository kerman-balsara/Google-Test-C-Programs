// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <person.h>

#include <ioperson.h>
#include <iopersoncipher.h>
#include <iopersonrtns.h>
#include <strutils.h>

static int decryptEmail(const person_r * const person,
                              string_t   emailAddress[SZ_EMAIL_ADDRESS + 1]);

// --------------------------------------------------------------------
// Check whether the passed id belongs to a valid person on a database.
//
// Return:
// 0 - Valid
// 1 - Invalid
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/dbio/ioperson.cpp
// --------------------------------------------------------------------

int DbPersonReadKey(const int id, person_r * const person)
{
    int ret;

    DbPersonInitRec(person);

    if ((ret = DbPersonSelectKey(id, person)))
        return(ret);

    return(DbPersonPostRead(person));
}

// --------------------------------------------------------------------
// Initialise passed record.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/misc/structinit.cpp
// --------------------------------------------------------------------

void DbPersonInitRec(person_r * const person)
{
    memset(person, 0, sizeof(person_r));
    person->id = 0;
}

// --------------------------------------------------------------------
// Set up record after a successful read from the database.
//
// Return:
// 0 - Valid
// < 0 - Major error
// --------------------------------------------------------------------

int DbPersonPostRead(person_r * const person)
{
    return(0);
}

// --------------------------------------------------------------------
// Hide decrypted email.
//
// Return:
// 0 - Valid
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/dbio/ioperson.cpp
// --------------------------------------------------------------------

int DbPersonHideEmail(const person_r * const person,
                            string_t   emailAddress[SZ_EMAIL_ADDRESS + 1])
{
    int ret;

    if ((ret = decryptEmail(person, emailAddress)))
        return(ret);

    EmailAddressUserHide(emailAddress);

    return(0);
}

// --------------------------------------------------------------------
// Show decrypted email.
//
// Return:
// 0 - Valid
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/dbio/ioperson.cpp
// --------------------------------------------------------------------

int DbPersonShowEmail(const person_r * const person,
                            string_t   emailAddress[SZ_EMAIL_ADDRESS + 1])
{
    return(decryptEmail(person, emailAddress));
}

// --------------------------------------------------------------------
// Decrypt email.
//
// Return:
// 0 - Valid
// < 0 - Major error
// --------------------------------------------------------------------

static int decryptEmail(const person_r * const person,
                              string_t   emailAddress[SZ_EMAIL_ADDRESS + 1])
{
    strlcpy(emailAddress, person->emailAddress, SZ_EMAIL_ADDRESS + 1);

    return(DbPersonPostReadDecrypt(person, emailAddress));
}
