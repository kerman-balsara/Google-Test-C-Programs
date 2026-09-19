// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errmsgdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

// System under test
#include <receipt.c>

int WRAPreceiptFile(const person_r *  const person,
                    const pmttxn_r *  const pmttxn,
                    const recprm_r *  const recprm,
                    constr_t          fullPathName,
                    const string_t    toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                          errmsg_t * const errmsg)
{
    return(receiptFile(person, pmttxn, recprm, fullPathName, toEmailAddress, errmsg));
}

int WRAPreceiptEmail(const person_r * const person,
                     const pmttxn_r * const pmttxn,
                     const recprm_r * const recprm,
                     constr_t         fullPathName,
                     const string_t   toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                           FILE     **fPtr,
                           int      * const rectfile,
                           errmsg_t * const errmsg)
{
    return(receiptEmail(person, pmttxn, recprm, fullPathName, toEmailAddress, fPtr, rectfile, errmsg));
}

int WRAPreceiptEmailErr(constr_t     fullPathName,
                        const int    ret,
                              int  * rectfile,
                              FILE **fPtr)
{
    return(receiptEmailErr(fullPathName, ret, rectfile, fPtr));
}                              
