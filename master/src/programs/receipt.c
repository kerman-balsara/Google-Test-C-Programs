// ---------------------
// External definitions.
// ---------------------

#include <errno.h>

#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <errmsgdefs.h>
#include <receiptdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#include <errmsgrtns.h>
#include <filertns.h>
#include <fileutils.h>
#include <fmtstr.h>
#include <ioperson.h>
#include <iopmttxn.h>
#include <iorecprm.h>
#include <receipt.h>
#include <receiptval.h>
#include <receiptwrite.h>
#include <syscmd.h>
#include <sysexit.h>

static int receiptFile(const person_r *  const person,
                       const pmttxn_r *  const pmttxn,
                       const recprm_r *  const recprm,
                       constr_t          fullPathName,
                       const string_t    toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                             errmsg_t * const errmsg);

static int receiptEmail(const person_r * const person,
                        const pmttxn_r * const pmttxn,
                        const recprm_r * const recprm,
                        constr_t         fullPathName,
                        const string_t   toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                              FILE     **fPtr,
                              int      * const rectfile,
                              errmsg_t * const errmsg);
static int receiptEmailErr(constr_t     fullPathName,
                           const int    ret,
                                 int  * const rectfile,
                                 FILE **fPtr);

// TODO
// ReceiptSelectFolder returns 0 with createFile 0 when file exists in archive 
// email file

// --------------------------------------------------------------------
// Generate receipt as an email
//
// Return:
// 0 - Valid
// 1 - error (message in errmsg)
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfakedbrealfile/programs/receipt.cpp
// file:///home/kerman/projects/GoogleTestC/testfake/programs/receipt.cpp
// --------------------------------------------------------------------

int ReceiptMain(const int personId,
                const int pmttxnSeqNo,
                const string_t recprmId[SZ_RECPRM_ID + 1],
                      errmsg_t * const errmsg)
{
    int ret;

    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;

    string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1];

    ErrmsgInit(errmsg);

    if ((ret = ReceiptValidateDb(personId, pmttxnSeqNo, recprmId, &person, &pmttxn, &recprm, toEmailAddress, errmsg)))
        return(ret);

    if ((ret = ReceiptValidateFolders(&recprm, errmsg)))
        return(ret);

    string_t fullPathName[SZ_FULL_PATH_NAME + 1];
    bool_t createFile;
    if ((ret = ReceiptSelectFolder(personId, pmttxnSeqNo, &recprm, fullPathName, &createFile, errmsg)))
        return(ret);
    if (!createFile) return(0);

    return(receiptFile(&person, &pmttxn, &recprm, fullPathName, toEmailAddress, errmsg));
}

// --------------------------------------------------------------------
// Open relevant file for writing a receipt email.
//
// Return:
// 0 - Valid
// 1 - error (message in errmsg)
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/programs/receipt.cpp
// --------------------------------------------------------------------

static int receiptFile(const person_r *  const person,
                       const pmttxn_r *  const pmttxn,
                       const recprm_r *  const recprm,
                       constr_t          fullPathName,
                       const string_t    toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                             errmsg_t * const errmsg)
{
    FILE * fPtr = NULL;
    int rectfile;

    ErrmsgInit(errmsg);

    int ret = receiptEmail(person, pmttxn, recprm, fullPathName, toEmailAddress, &fPtr, &rectfile, errmsg);
    return(ret ? receiptEmailErr(fullPathName, ret, &rectfile, &fPtr) : 0);
}                      

static int receiptEmail(const person_r * const person,
                        const pmttxn_r * const pmttxn,
                        const recprm_r * const recprm,
                        constr_t         fullPathName,
                        const string_t   toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                              FILE     **fPtr,
                              int      * const rectfile,
                              errmsg_t * const errmsg)
{
    int ret;

    ErrmsgInit(errmsg);

    *rectfile = RECTFILE_INIT;

    FileOpen(fPtr, fullPathName, "w");

    *rectfile |= (RECTFILE_OPEN | RECTFILE_DELETE);

    ReceiptFileWrite(person, pmttxn, recprm, toEmailAddress, *fPtr);

    FileClose(fPtr);

    *rectfile &= ~RECTFILE_OPEN;

    // Archive

    if ((ret = SysCmd(FmtStr("%s %s %s", recprm->archiveCmd, fullPathName, recprm->archiveFolderName), errmsg)))
        return(ret);

    *rectfile &= ~RECTFILE_DELETE;

    return(0);
}

// --------------------------------------------------------------------
// Process errors while creating a receipt email.
//
// Return:
// ret - from receiptEmail()
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/programs/receipt.cpp
// --------------------------------------------------------------------

static int receiptEmailErr(constr_t     fullPathName,
                           const int    ret,
                                 int  * rectfile,
                                 FILE **fPtr)
{
    if (*rectfile & RECTFILE_OPEN)
    {
        FileClose(fPtr);
        *rectfile &= ~RECTFILE_OPEN;
    }
    if (*rectfile & RECTFILE_DELETE)
    {
        (void) FileDelete(fullPathName);
        *rectfile &= ~RECTFILE_DELETE;
    }
    return(ret);
}
