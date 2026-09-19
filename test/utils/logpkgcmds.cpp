#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::HasSubstr;

#include <loglvldefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <logpkgcmds.h>
#include <logpkgrtns.h>
#include <logrtns.h>

static string_t saveLogPkgInfo[SZ_LOGPKG_INFO];
static int saveLogLvlsEnabled;

class LogPkgCmdsProcF : public ::testing::Test
{
    protected:
    static void SetUpTestSuite()
    {
        strlcpy(saveLogPkgInfo, LogPkgInfo(), sizeof(saveLogPkgInfo));
        saveLogLvlsEnabled = LogLvlsEnabled();
        LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    }
    static void TearDownTestSuite()
    {
        LogPkgCmdsProc(saveLogPkgInfo);
        LogLvlEnable(saveLogLvlsEnabled);
    }
};

TEST_F(LogPkgCmdsProcF, IsSomeOk)
{
    // Arrange
    string_t cmd[SZ_LOGPKG_INFO];
    strlcpy(cmd, "+pkg,pkg+,+wire,-memory,-wire", sizeof(cmd));
    string_t expStr[SZ_LOGPKG_INFO];
    strlcpy(expStr, "-MEMORY,-WIRE", sizeof(expStr));

    // LogPkgCmdsProc() will:
    // - ignore +pkg, pkg+ since these are invalid commands
    // - override +WIRE (-WIRE follows)
    // - Process -MEMORY
    // Act
    LogPkgCmdsProc(cmd);

    // Assert
    EXPECT_THAT(LogPkgInfo(), HasSubstr(expStr));
}

TEST_F(LogPkgCmdsProcF, IsOk)
{
    // Arrange
    string_t cmd[SZ_LOGPKG_INFO];
    strlcpy(cmd, "+wire,-memory,-wire", sizeof(cmd));
    string_t expStr[SZ_LOGPKG_INFO];
    strlcpy(expStr, "-MEMORY,-WIRE", sizeof(expStr));

    // LogPkgCmdsProc() will:
    // - override +WIRE (-WIRE follows)
    // - Process -MEMORY
    // Act
    LogPkgCmdsProc(cmd);

    // Assert
    EXPECT_THAT(LogPkgInfo(), HasSubstr(expStr));
}

TEST_F(LogPkgCmdsProcF, IsOkNullCmd)
{
    // Arrange
    string_t cmd[20 + 1];
    *cmd = 0;
    string_t expStr[SZ_LOGPKG_INFO];
    strlcpy(expStr, LogPkgInfo(), sizeof(expStr));

    // Act
    LogPkgCmdsProc(cmd);

    // Assert
    EXPECT_THAT(LogPkgInfo(), HasSubstr(expStr));
}

TEST_F(LogPkgCmdsProcF, IsOkNewLineCmd)
{
    // Arrange
    string_t cmd[20 + 1];
    *cmd = '\n';
    cmd[1] = '\0';
    string_t expStr[SZ_LOGPKG_INFO];
    strlcpy(expStr, LogPkgInfo(), sizeof(expStr));

    // Act
    LogPkgCmdsProc(cmd);

    // Assert
    EXPECT_THAT(LogPkgInfo(), HasSubstr(expStr));
}
