// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <tuple>
#include <utility>

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <fileutils.h>

static std::map<std::string, std::tuple<constr_t, bool_t>> FileExistsD
{
    {"FileExists", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder/donotdeletetestfile.txt", 1)},
    {"FileDoesNotExist1", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder", 0)},
    {"FileDoesNotExist2", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder/invalidfile.txt", 0)}
};

class FileExistsF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, bool_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(fileutils, FileExistsF, testing::ValuesIn(FileExistsD),
[] (const testing::TestParamInfo<FileExistsF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(FileExistsF, IsOk)
{
    // Arrange
    std::tuple<constr_t, bool_t> tuple = GetParam().second;
    constr_t fullPathName = std::get<0>(tuple); 
    const bool_t expFileExists = std::get<1>(tuple);

    // Act
    bool_t actFileExists = FileExists(fullPathName);

    // Assert
    EXPECT_EQ((int) expFileExists, (int) actFileExists);
}

static std::map<std::string, std::tuple<constr_t, bool_t>> FolderExistsD
{
    {"FolderExists", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder", 1)},
    {"FolderDoesNotExist", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder/invalidfolder", 0)}
};

class FolderExistsF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, bool_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(fileutils, FolderExistsF, testing::ValuesIn(FolderExistsD),
[] (const testing::TestParamInfo<FolderExistsF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(FolderExistsF, IsOk)
{
    // Arrange
    std::tuple<constr_t, bool_t> tuple = GetParam().second;
    constr_t folderName = std::get<0>(tuple); 
    const bool_t expFolderExists = std::get<1>(tuple);

    // Act
    bool_t actFolderExists = FolderExists(folderName);

    // Assert
    EXPECT_EQ((int) expFolderExists, (int) actFolderExists);
}

TEST(SetFileFullPath, IsOk)
{
    // Arrange
    string_t folderName[SZ_FOLDER_NAME + 1];
    string_t fileName[SZ_FILE_NAME + 1];
    string_t actFullPathName[SZ_FULL_PATH_NAME + 1];
    string_t expFullPathName[SZ_FULL_PATH_NAME + 1];

    strlcpy(folderName, "/home/kerman/projects/GoogleTestC/foldername", sizeof(folderName));
    strlcpy(fileName, "filename", sizeof(fileName));
    strlcpy(expFullPathName, "/home/kerman/projects/GoogleTestC/foldername/filename", sizeof(expFullPathName));

    // Act
    SetFileFullPath(folderName, fileName, actFullPathName);

    // Assert
    ASSERT_STREQ(expFullPathName, actFullPathName);
}
