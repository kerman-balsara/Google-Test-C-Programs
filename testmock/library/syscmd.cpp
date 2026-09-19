// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::InSequence;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetErrnoAndReturn;

#include <errno.h>

#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <errcodes.h>
#include <errmsgdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <filertns.h>
#include <wrapsyscmd.h>

TEST(SysCmdDeathTest, Diespopen)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            string_t cmd[20 + 1];
            strlcpy(cmd, "date", sizeof(cmd));
            errmsg_t errmsg;

            InSequence seq;
 
            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llpopen)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, ((FILE *) NULL)));

            // Act
            SysCmd(cmd, &errmsg);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_POPEN)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(SysCmdDeathTest, Diesfgets)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            FILE * fPtr = fopen("/home/kerman/projects/GoogleTestC/testfolder/donotdeletetestfile.txt", "r");

            string_t cmd[20 + 1];
            strlcpy(cmd, "date", sizeof(cmd));
            errmsg_t errmsg;

            InSequence seq;
        
            EXPECT_CALL(*MockedFnsObj, llpopen)
            .Times(1)
            .WillOnce(Return(fPtr));
            EXPECT_CALL(*MockedFnsObj, llfgets)
            .Times(1)
            .WillOnce(Return((string_t *)NULL));
            EXPECT_CALL(*MockedFnsObj, llferror)
            .Times(1)
            .WillOnce(Return(1));

            // Act
            SysCmd(cmd, &errmsg);

            (void) fclose(fPtr);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        HasSubstr("code: " + std::to_string(SYSCALL_FGETS)));
}

TEST(SysCmdDeathTest, Diespclose)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            FILE * fPtr = fopen("/home/kerman/projects/GoogleTestC/testfolder/donotdeletetestfile.txt", "r");

            string_t cmd[10 + 1];
            strlcpy(cmd, "date", sizeof(cmd));
            errmsg_t errmsg;

            InSequence seq;

            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llpopen)
            .Times(1)
            .WillOnce(Return(fPtr));
            EXPECT_CALL(*MockedFnsObj, llfgets)
            .Times(1)
            .WillOnce(Return((string_t *)NULL));
            EXPECT_CALL(*MockedFnsObj, llferror)
            .Times(1)
            .WillOnce(Return(0));
            EXPECT_CALL(*MockedFnsObj, llpclose)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            //Act
            SysCmd(cmd, &errmsg);

            (void) fclose(fPtr);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_PCLOSE)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
}

TEST(SysCmd, IsOkInvalidCmd)
{
    // Arrange

    FILE * fPtr = fopen("/home/kerman/projects/GoogleTestC/testfolder/donotdeletetestfile.txt", "r");

    string_t cmd[10 + 1];
    strlcpy(cmd, "invalidCmd", sizeof(cmd));
    errmsg_t errmsg;

    string_t resultWithNl[20 + 1];
    strlcpy(resultWithNl, "Invalid command\n", sizeof(resultWithNl));
    string_t result[20 + 1];
    strlcpy(result, "Invalid command", sizeof(result));

    {
        MockedFnsObj = std::make_shared<MockedFns>();

        InSequence seq;

        EXPECT_CALL(*MockedFnsObj, llpopen)
        .Times(1)
        .WillOnce(Return(fPtr));
        EXPECT_CALL(*MockedFnsObj, llfgets)
        .Times(1)
        .WillOnce(Return(resultWithNl));
        EXPECT_CALL(*MockedFnsObj, llpclose)
        .Times(1)
        .WillOnce(Return(1));
    }

    //Act
    int ret = SysCmd(cmd, &errmsg);

    (void) fclose(fPtr);

    //Assert        
    EXPECT_EQ(1, ret);
    EXPECT_EQ(SYSCMD_FAILED, errmsg.code);
    EXPECT_STREQ(result, errmsg.message);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SysCmd, IsOkValidCmd)
{
    // Arrange

    FILE * fPtr = fopen("/home/kerman/projects/GoogleTestC/testfolder/donotdeletetestfile.txt", "r");

    string_t cmd[10 + 1];
    strlcpy(cmd, "date", sizeof(cmd));
    errmsg_t errmsg;

    {
        MockedFnsObj = std::make_shared<MockedFns>();

        InSequence seq;

        EXPECT_CALL(*MockedFnsObj, llpopen)
        .Times(1)
        .WillOnce(Return(fPtr));
        EXPECT_CALL(*MockedFnsObj, llfgets)
        .Times(1)
        .WillOnce(Return((string_t *)NULL));
        EXPECT_CALL(*MockedFnsObj, llferror)
        .Times(1)
        .WillOnce(Return(0));
        EXPECT_CALL(*MockedFnsObj, llpclose)
        .Times(1)
        .WillOnce(Return(0));
    }

    //Act
    int ret = SysCmd(cmd, &errmsg);

    (void) fclose(fPtr);

    //Assert        
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, errmsg.code);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}
