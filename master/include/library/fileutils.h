#ifndef FILEUTILS_H
#define FILEUTILS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

bool_t FileExists(const string_t fileName[SZ_FULL_PATH_NAME + 1]);
bool_t FolderExists(const string_t folderName[SZ_FOLDER_NAME + 1]);

void SetFileFullPath(const string_t folderName[SZ_FOLDER_NAME + 1],
                     const string_t fileName[SZ_FILE_NAME + 1],
                           string_t fullPathName[SZ_FULL_PATH_NAME + 1]);


#ifdef __cplusplus
}
#endif

#endif // FILEUTILS_H
