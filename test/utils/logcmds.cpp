#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <loglvldefs.h>
#include <typedefs.h>

#include <logcmds.h>
#include <logrtns.h>

static int saveLogLvlsEnabled;

class LogCmdsProcF : public ::testing::Test
{
    protected:
    static void SetUpTestSuite()
    {
        saveLogLvlsEnabled = LogLvlsEnabled();
    }
    static void TearDownTestSuite()
    {
        LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
        LogLvlEnable(saveLogLvlsEnabled);
    }
};

TEST_F(LogCmdsProcF, IsSomeOk)
{
    // Arrange
    LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    string_t cmd[20 + 1];
    // LogCmdsProc() will:
    // - ignore +st, t+ and +x since these are invalid commands
    // - Process +w (enable warnings)
    // - Process -w (disable warnings) 
    strlcpy(cmd, "+st,t+,+x,+w,-w", sizeof(cmd));
    int expLogLvl = 0;
    // Act
    LogCmdsProc(cmd);
    // Assert
    EXPECT_EQ(expLogLvl, LogLvlsEnabled());
}

TEST_F(LogCmdsProcF, IsOk)
{
    // Arrange
    LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    string_t cmd[20 + 1];
    strlcpy(cmd, "+e,-w,+i,-t", sizeof(cmd));
    int expLogLvl = LOGLVL_ERROR | LOGLVL_INFO;
    // Act
    LogCmdsProc(cmd);
    // Assert
    EXPECT_EQ(expLogLvl, LogLvlsEnabled());
}

TEST_F(LogCmdsProcF, IsOkNullCmd)
{
    // Arrange
    LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    string_t cmd[20 + 1];
    *cmd = 0;
    int expLogLvl = 0;
    // Act
    LogCmdsProc(cmd);
    // Assert
    EXPECT_EQ(expLogLvl, LogLvlsEnabled());
}

TEST_F(LogCmdsProcF, IsOkNewLineCmd)
{
    // Arrange
    LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    string_t cmd[20 + 1];
    *cmd = '\n';
    cmd[1] = '\0';
    int expLogLvl = 0;
    // Act
    LogCmdsProc(cmd);
    // Assert
    EXPECT_EQ(expLogLvl, LogLvlsEnabled());
}
