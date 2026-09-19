#ifndef WRAPRECEIPT_H
#define WRAPRECEIPT_H

// ---------------------
// Internal definitions.
// ---------------------

#include <receipt.h>

#ifdef __cplusplus
extern "C" {
#endif

int WRAPreceiptFile(const person_r *  const person,
                    const pmttxn_r *  const pmttxn,
                    const recprm_r *  const recprm,
                    constr_t          fullPathName,
                    const string_t    toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                          errmsg_t * const errmsg);

int WRAPreceiptEmail(const person_r * const person,
                     const pmttxn_r * const pmttxn,
                     const recprm_r * const recprm,
                     constr_t         fullPathName,
                     const string_t   toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                           FILE     **fPtr,
                           int      * const rectfile,
                           errmsg_t * const errmsg);

int WRAPreceiptEmailErr(constr_t     fullPathName,
                        const int    ret,
                              int  * rectfile,
                              FILE **fPtr);

                              #ifdef __cplusplus
}
#endif

#endif // TESTRECEIPT_H