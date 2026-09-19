// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::HasSubstr;

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetErrnoAndReturn;

#include <sys/stat.h>
#include <errno.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <wrapfilertns.h>

TEST(FileOpenDeathTest, Diesfopen)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            FILE *fPtr;
            string_t fileName[SZ_FULL_PATH_NAME + 1];

            InSequence seq;

            int sysErrno = EACCES;
        
            EXPECT_CALL(*MockedFnsObj, llfopen)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, (FILE *) NULL));

            // Act
            FileOpen(&fPtr, fileName, "w");
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_FOPEN)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
}

TEST(FileCloseDeathTest, Diesfclose)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            FILE *fPtr;

            InSequence seq;

            int sysErrno = EBADF;
        
            EXPECT_CALL(*MockedFnsObj, llfclose)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            FileClose(&fPtr);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_FCLOSE)),
              HasSubstr("errno: " + std::to_string(EBADF) + " [" + strerror(EBADF) + "]")));
}

TEST(FileWriteStrDeathTest, Diesfputs)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            FILE *fPtr = NULL;

            InSequence seq;

            EXPECT_CALL(*MockedFnsObj, llfputs)
            .Times(1)
            .WillOnce(Return(-1));

            // Act
            FileWriteStr(fPtr, "Test");
        },
        // Assert
        ::testing::ExitedWithCode(1),
        HasSubstr("code: " + std::to_string(SYSCALL_FPUTS)));
}

TEST(FileWriteStrDeathTest, Diesfputc)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            FILE *fPtr = NULL;

            InSequence seq;

            EXPECT_CALL(*MockedFnsObj, llfputc)
            .Times(1)
            .WillOnce(Return(-1));

            // Act
            FileWriteStr(fPtr, NULL);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        HasSubstr("code: " + std::to_string(SYSCALL_FPUTC)));
}

TEST(FileWriteStrFDeathTest, Diesvfprintf)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            FILE *fPtr = NULL;

            InSequence seq;

            EXPECT_CALL(*MockedFnsObj, llvfprintf)
            .Times(1)
            .WillOnce(Return(-1));

            // Act
            FileWriteStrF(fPtr, "%s", "Test");
        },
        // Assert
        ::testing::ExitedWithCode(1),
        HasSubstr("code: " + std::to_string(SYSCALL_VFPRINTF)));
}

TEST(FileDeleteDeathTest, Diesunlink)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            string_t fileName[SZ_FULL_PATH_NAME + 1];

            InSequence seq;

            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llunlink)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            FileDelete(fileName);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_UNLINK)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
}
