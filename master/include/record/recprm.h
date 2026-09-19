#ifndef RECPRM_H
#define RECPRM_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

typedef struct
{
    string_t id[SZ_RECPRM_ID + 1];                          //   9
    char     padding1[3];                                   //   3

    string_t archiveFolderName[SZ_FOLDER_NAME + 1];         // 201
    string_t emailFolderName[SZ_FOLDER_NAME + 1];           // 201
    string_t emailAddress[SZ_EMAIL_ADDRESS + 1];            //  51
    string_t archiveCmd[SZ_ARCHIVE_CMD + 1];                //  51
                                                            // 516
} recprm_r;

#endif	// RECPRM_H