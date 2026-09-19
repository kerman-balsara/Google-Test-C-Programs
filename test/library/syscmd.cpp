// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <errmsgdefs.h>
#include <strlstdefs.h>

#include <lnklstrtns.h>
#include <logrtns.h>
#include <syscmd.h>

TEST(SysCmd, IsOkLineOutput)
{
    // Arrange
    errmsg_t errmsg;

    // Act
    int ret = SysCmd("date", &errmsg);

    // Assert
    EXPECT_EQ(0, ret);
}

TEST(SysCmd, IsOkNoOutput)
{
    // Arrange
    errmsg_t errmsg;

    // Act
    int ret = SysCmd("cd", &errmsg);

    // Assert
    EXPECT_EQ(0, ret);
}

TEST(SysCmd, IsOkInvalidCmd)
{
    // Arrange
    errmsg_t errmsg;

    // Act
    int ret = SysCmd("invalidCmd", &errmsg);

    // Assert
    EXPECT_EQ(1, ret);
    EXPECT_EQ(SYSCMD_FAILED, errmsg.code);
}

TEST(SysCmd, IsOkValidCmdStrLstOutput)
{
    // Arrange
    errmsg_t errmsg;
    strlst_t * strlstHead = NULL;

    // Act
    int ret = SysCmdStrLst("ls -lrt", &strlstHead, &errmsg);
    LnkLstFree("SysCmdStrLstValidCmd", &strlstHead);    

    // Assert
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, errmsg.code);
}

TEST(SysCmd, IsOkInvalidCmdStrLstOutput)
{
    // Arrange
    errmsg_t errmsg;
    strlst_t * strlstHead = NULL;

    // Act
    int ret = SysCmdStrLst("invalidCmd\n", &strlstHead, &errmsg);
    LnkLstFree("SysCmdStrLstInvalidCmd", &strlstHead);    

    // Assert
    EXPECT_EQ(1, ret);
    EXPECT_EQ(SYSCMD_FAILED, errmsg.code);
}
