// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <recprm.h>

#include <iorecprm.h>
#include <recprmdata.h>

// Return fake data 
void RecprmData(const string_t id[SZ_RECPRM_ID + 1], recprm_r * const recprm)
{
    DbRecprmInitRec(recprm);
    strlcpy(recprm->id, id, sizeof(recprm->id));
    strlcpy(recprm->archiveFolderName, "/home/kerman/projects/GoogleTestC/testfolder/archive/receipt", sizeof(recprm->archiveFolderName));
    strlcpy(recprm->emailFolderName, "/home/kerman/projects/GoogleTestC/testfolder/email", sizeof(recprm->emailFolderName));
    strlcpy(recprm->emailAddress, "noreply@finance.kbtest.com.au", sizeof(recprm->emailAddress));
    strlcpy(recprm->archiveCmd, "mv", sizeof(recprm->archiveCmd));
}
