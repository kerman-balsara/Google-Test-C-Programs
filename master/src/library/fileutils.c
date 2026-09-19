// ---------------------
// External definitions.
// ---------------------

#include <sys/stat.h>

#include <stdio.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <pathstatus.h>
#include <fileutils.h>

// --------------------------------------------------------------------
// Check if passed file exists.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/library/fileutils.cpp
// --------------------------------------------------------------------

bool_t FileExists(const string_t fileName[SZ_FULL_PATH_NAME + 1])
{
    struct stat fileStatus;

    return(PathExists(fileName, &fileStatus) && !S_ISDIR(fileStatus.st_mode));
}

// --------------------------------------------------------------------
// Check if passed folder exists.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/fileutils.cpp
// --------------------------------------------------------------------

bool_t FolderExists(const string_t folderName[SZ_FOLDER_NAME + 1])
{
    struct stat folderStatus;

    return(PathExists(folderName, &folderStatus) && S_ISDIR(folderStatus.st_mode));
}

// --------------------------------------------------------------------
// Set full path name using folder name and file name.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/fileutils.cpp
// --------------------------------------------------------------------

void SetFileFullPath(const string_t folderName[SZ_FOLDER_NAME + 1],
                     const string_t fileName[SZ_FILE_NAME + 1],
                           string_t fullPathName[SZ_FULL_PATH_NAME + 1])
{
    snprintf(fullPathName, SZ_FULL_PATH_NAME + 1, "%s/%s", folderName, fileName);
}
