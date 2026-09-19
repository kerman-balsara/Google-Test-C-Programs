// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errmsgrtns.h>

// --------------------------------------------------------------------
// Set passed errmsg_t based on the passed code and message.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/library/errmsgrtns.cpp
// --------------------------------------------------------------------

void ErrmsgSet(const int code, constr_t message, errmsg_t * const errmsg)
{
    ErrmsgInit(errmsg);
    errmsg->code = code;
    snprintf(errmsg->message, sizeof(errmsg->message), "%s", message);
}

// --------------------------------------------------------------------
// Initialise passed errmsg_t.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/misc/structinit.cpp
// --------------------------------------------------------------------

void ErrmsgInit(errmsg_t * const errmsg)
{
    memset(errmsg, 0, sizeof(errmsg_t));
    errmsg->code = 0;
}
