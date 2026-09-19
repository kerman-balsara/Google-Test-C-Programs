// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <errmsgdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#include <errmsgrtns.h>
#include <ioperson.h>
#include <iopmttxn.h>
#include <iopmttxn.h>
#include <iorecprm.h>
#include <fileutils.h>
#include <filertns.h>
#include <fmtamt.h>
#include <receiptval.h>

// --------------------------------------------------------------------
// Validate passed details using the database.
//
// Return:
// 0 - Valid
// 1 - error (message in errmsg)
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/programs/receiptval.cpp
// --------------------------------------------------------------------

int ReceiptValidateDb(const int personId,
                      const int pmttxnSeqNo,
                      const string_t recprmId[SZ_RECPRM_ID + 1],
                            person_r * const person,
                            pmttxn_r * const pmttxn,
                            recprm_r * const recprm,
                            string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                            errmsg_t * const errmsg)
{
    int ret;

    ErrmsgInit(errmsg);

    ret = DbPersonReadKey(personId, person);
    if (ret < 0) return(ret);
    if (ret == 1)
    {
        errmsg->code = INVALID_PERSON_REC;
        snprintf(errmsg->message, sizeof(errmsg->message), "Invalid person id %d", personId);
        return(1);
    }

    if ((ret = DbPersonShowEmail(person, toEmailAddress)))
        return(ret);

    ret = DbPmttxnReadKey(personId, pmttxnSeqNo, pmttxn);
    if (ret < 0) return(ret);
    if (ret == 1)
    {
        errmsg->code = INVALID_PMTTXN_REC;
        snprintf(errmsg->message, sizeof(errmsg->message), "Invalid pmttxn id/seq %d/%d", personId, pmttxnSeqNo);
        return(1);
    }

    ret = DbRecprmReadKey(recprmId, recprm);
    if (ret < 0) return(ret);
    if (ret == 1)
    {
        errmsg->code = INVALID_RECPRM_REC;
        snprintf(errmsg->message, sizeof(errmsg->message), "Invalid recprm id %s", recprmId);
        return(1);
    }

    return(0);
}

// --------------------------------------------------------------------
// Validate folders in passed recprm.
//
// Return:
// 0 - Valid
// 1 - error (message in errmsg)
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/programs/receiptval.cpp
// --------------------------------------------------------------------

int ReceiptValidateFolders(const recprm_r * const recprm,
                                 errmsg_t * const errmsg)
{
    ErrmsgInit(errmsg);

    if (!FolderExists(recprm->archiveFolderName))
    {
        errmsg->code = INVALID_RECPRM_ARCHIVE;
        snprintf(errmsg->message, sizeof(errmsg->message), "Invalid recprm archive folder %s", recprm->archiveFolderName);
        return(1);
    }    
    if (!FolderExists(recprm->emailFolderName))
    {
        errmsg->code = INVALID_RECPRM_EMAIL;
        snprintf(errmsg->message, sizeof(errmsg->message), "Invalid recprm email folder %s", recprm->emailFolderName);
        return(1);
    }    

    return(0);
}

// --------------------------------------------------------------------
// Check if file needs to be created and the folder in which it is to
// be created.
//
// Return:
// 0 - Valid (check createFile and fullPathName)
// 1 - error (message in errmsg)
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/programs/receiptval.cpp
// --------------------------------------------------------------------

int ReceiptSelectFolder(const int personId,
                        const int pmttxnSeqNo,
                        const recprm_r * const recprm,
                              string_t   fullPathName[SZ_FULL_PATH_NAME + 1],
                              bool_t   * const createFile,
                              errmsg_t * const errmsg)
{
    ErrmsgInit(errmsg);
    *createFile = 0;
    *fullPathName = 0;

    // Check archive folder for file
    // If file does not exist
    // - Create file in email folder

    string_t fileName[SZ_FILE_NAME + 1];
    string_t fullArchivePathName[SZ_FULL_PATH_NAME + 1];

    snprintf(fileName, sizeof(fileName), "%06d_%06d_receipt", personId, pmttxnSeqNo);

    SetFileFullPath(recprm->archiveFolderName, fileName, fullArchivePathName);

    if (FileExists(fullArchivePathName))
    {
        strlcpy(fullPathName, fullArchivePathName, SZ_FULL_PATH_NAME + 1);
        return(0);
    }

    string_t fullEmailPathName[SZ_FULL_PATH_NAME + 1];

    SetFileFullPath(recprm->emailFolderName, fileName, fullEmailPathName);

    if (FileExists(fullEmailPathName))
    {
        errmsg->code = RECEIPT_ANOTHER_USER;
        strlcpy(errmsg->message, "Receipt is being generated by another user", sizeof(errmsg->message));
        return(1);
    }

    strlcpy(fullPathName, fullEmailPathName, SZ_FULL_PATH_NAME + 1);
    *createFile = 1;

    return(0);
}
