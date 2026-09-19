#ifndef RECEIPTVAL_H
#define RECEIPTVAL_H

// ---------------------
// Internal definitions.
// ---------------------

#include <errmsgdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#ifdef __cplusplus
extern "C" {
#endif

int ReceiptValidateDb(const int personId,
                      const int pmttxnSeqNo,
                      const string_t recprmId[SZ_RECPRM_ID + 1],
                            person_r * const person,
                            pmttxn_r * const pmttxn,
                            recprm_r * const recprm,
                            string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                            errmsg_t * const errmsg);

int ReceiptValidateFolders(const recprm_r * const recprm,
                                 errmsg_t * const errmsg);

                                 
int ReceiptSelectFolder(const int personId,
                        const int pmttxnSeqNo,
                        const recprm_r * const recprm,
                              string_t   fullPathName[SZ_FULL_PATH_NAME + 1],
                              bool_t   * const createFile,
                              errmsg_t * const errmsg);

                           
#ifdef __cplusplus
}
#endif

#endif // RECEIPTVAL_H