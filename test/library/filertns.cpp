// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

#include <errno.h>

#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <filertns.h>

TEST(FileOpen, IsOk)
{
    // Arrange
    FILE *actFptr;
    string_t fileName[SZ_FULL_PATH_NAME + 1];
    strlcpy(fileName, "/home/kerman/projects/GoogleTestC/testfolder/testfile.txt", sizeof(fileName));

    // Act
    FileOpen(&actFptr, fileName, "w+");

    // Assert
    ASSERT_NE(nullptr, actFptr);

    (void) fclose(actFptr);
}

TEST(FileClose, IsOk)
{
    // Arrange
    FILE *actFptr;
    string_t fileName[SZ_FULL_PATH_NAME + 1];
    strlcpy(fileName, "/home/kerman/projects/GoogleTestC/testfolder/testfile.txt", sizeof(fileName));

    actFptr = fopen(fileName, "w+");
    ASSERT_NE(nullptr, actFptr);

    // Act
    FileClose(&actFptr);

    // Assert
    ASSERT_EQ(nullptr, actFptr);
}

TEST(FileDelete, IsOk)
{
    // Arrange
    struct stat statbuf;
    FILE *actFptr;
    string_t fileName[SZ_FULL_PATH_NAME + 1];
    strlcpy(fileName, "/home/kerman/projects/GoogleTestC/testfolder/testfile.txt", sizeof(fileName));

    actFptr = fopen(fileName, "w+");
    ASSERT_NE(nullptr, actFptr);
    (void) fclose(actFptr);

    // Act
    FileDelete(fileName);

    // Assert
    int expRet = -1;
    int expErrno = ENOENT;
    int actRet = stat(fileName, &statbuf);
    EXPECT_EQ(expRet, actRet);
    EXPECT_EQ(expErrno, errno);
}

