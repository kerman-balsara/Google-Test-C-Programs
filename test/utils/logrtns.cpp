// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::HasSubstr;

#include <errno.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <loglvldefs.h>
#include <typedefs.h>
#include <syssizes.h>

#include <logrtns.h>

static FILE * LogfilePtr;
static constr_t LogfileName = "/home/kerman/projects/GoogleTestC/testfolder/logrtns.txt";
static int SaveStderrFd;
static long SaveFilePos;

static int saveLogLvlsEnabled;

// Testing Log... and Log...F functions. Since the setup is resource intensive, we use function pointers to call the
// functions we want to test. We write a single line to the log using the functions under test and read them back to
// verify the content. 
static std::map<std::string, std::tuple<int, bool_t, void (*)(constr_t), void (*)(constr_t, ...), constr_t, constr_t, constr_t>> LogXxxD
{
    {"LogErrorEnable", std::make_tuple(LOGLVL_ERROR, 1, LogError, nullptr, nullptr, "This tests LogError", "ERROR") },
    {"LogWarnEnable", std::make_tuple(LOGLVL_WARN, 1, LogWarn, nullptr, nullptr, "This tests LogWarn", "WARN") },
    {"LogInfoEnable", std::make_tuple(LOGLVL_INFO, 1, LogInfo, nullptr, nullptr, "This tests LogInfo", "INFO") },
    {"LogTraceEnable", std::make_tuple(LOGLVL_TRACE, 1, LogTrace, nullptr, nullptr, "This tests LogTrace", "TRACE") },
    {"LogErrorFEnable", std::make_tuple(LOGLVL_ERROR, 1, nullptr, LogErrorF, "%s", "This tests LogErrorF", "ERROR") },
    {"LogWarnFEnable", std::make_tuple(LOGLVL_WARN, 1, nullptr, LogWarnF, "%s", "This tests LogWarnF", "WARN") },
    {"LogInfoFEnable", std::make_tuple(LOGLVL_INFO, 1, nullptr, LogInfoF, "%s", "This tests LogInfoF", "INFO") },
    {"LogTraceFEnable", std::make_tuple(LOGLVL_TRACE, 1, nullptr, LogTraceF, "%s", "This tests LogTraceF", "TRACE") },

    {"LogErrorDisable", std::make_tuple(LOGLVL_ERROR, 0, LogError, nullptr, nullptr, "This tests LogError", "ERROR") },
    {"LogWarnDisable", std::make_tuple(LOGLVL_WARN, 0, LogWarn, nullptr, nullptr, "This tests LogWarn", "WARN") },
    {"LogInfoDisable", std::make_tuple(LOGLVL_INFO, 0, LogInfo, nullptr, nullptr, "This tests LogInfo", "INFO") },
    {"LogTraceDisable", std::make_tuple(LOGLVL_TRACE, 0, LogTrace, nullptr, nullptr, "This tests LogTrace", "TRACE") },
    {"LogErrorFDisable", std::make_tuple(LOGLVL_ERROR, 0, nullptr, LogErrorF, "%s", "This tests LogErrorF", "ERROR") },
    {"LogWarnFDisable", std::make_tuple(LOGLVL_WARN, 0, nullptr, LogWarnF, "%s", "This tests LogWarnF", "WARN") },
    {"LogInfoFDisable", std::make_tuple(LOGLVL_INFO, 0, nullptr, LogInfoF, "%s", "This tests LogInfoF", "INFO") },
    {"LogTraceFDisable", std::make_tuple(LOGLVL_TRACE, 0, nullptr, LogTraceF, "%s", "This tests LogTraceF", "TRACE") },
};

class LogXxxF : public testing::TestWithParam<std::pair<const std::string,
                                                        std::tuple<int,
                                                                   bool_t, 
                                                                   void (*)(constr_t),
                                                                   void (*)(constr_t, ...),
                                                                   constr_t,
                                                                   constr_t,
                                                                   constr_t>>>
{
    public:
    static void SetUpTestSuite()
    {
        saveLogLvlsEnabled = LogLvlsEnabled();

        // Save stderr
        SaveStderrFd = dup(STDERR_FILENO);
        ASSERT_NE(-1, SaveStderrFd) << "SetUpTestSuite dup failed errno: " << errno;

        // Open log file
        LogfilePtr = fopen(LogfileName, "w+");
        ASSERT_NE(nullptr, LogfilePtr) << "SetUpTestSuite fopen failed errno: " << errno;

        // Redirect stderr to log file
        int ret = dup2(fileno(LogfilePtr), STDERR_FILENO);
        ASSERT_NE(-1, ret) << "SetUpTestSuite dup2 failed errno: " << errno;

    }
    static void TearDownTestSuite()
    {
        LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
        LogLvlEnable(saveLogLvlsEnabled);

        // Close log file
        int ret = fclose(LogfilePtr);
        ASSERT_NE(-1, ret) << "TearDownTestSuite close LogfilePtr failed errno: " << errno;

        // Restore stderr
        ret = dup2(SaveStderrFd, STDERR_FILENO);
        ASSERT_NE(-1, ret) << "TearDownTestSuite dup2 failed errno: " << errno;

        // Close saved stderr
        ret = close(SaveStderrFd);
        ASSERT_NE(-1, ret) << "TearDownTestSuite close SaveStderrFd failed errno: " << errno;
    }
    void SetUp()
    {
        SaveFilePos = ftell(LogfilePtr);
    }
};

INSTANTIATE_TEST_SUITE_P(logrtns, LogXxxF, testing::ValuesIn(LogXxxD),
[] (const testing::TestParamInfo<LogXxxF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(LogXxxF, IsOk)
{
    // Arrange
    std::tuple<int , bool_t, void (*)(constr_t), void (*)(constr_t, ...), constr_t, constr_t, constr_t> tuple = GetParam().second;
    int loglvl = std::get<0>(tuple);
    bool_t enable = std::get<1>(tuple);
    void (*logfn)(constr_t) = std::get<2>(tuple);
    void (*logfnf)(constr_t, ...) = std::get<3>(tuple);
    constr_t format = std::get<4>(tuple);
    constr_t parameter = std::get<5>(tuple);
    constr_t loglvlStr = std::get<6>(tuple);

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    int ret;

    if (enable)
        (void) LogLvlEnable(loglvl);
    else
        (void) LogLvlDisable(loglvl);    

    // Act
    logfn ? logfn(parameter) : logfnf(format, parameter);

    // Flush the buffer and move the LogfilePtr ready to read and verify contents.
    ret = fflush(LogfilePtr);
    ASSERT_NE(-1, ret) << mapKey << " fflush failed errno: " << errno;
    ret = fseek(LogfilePtr, SaveFilePos, SEEK_SET);
    ASSERT_NE(-1, ret) << mapKey << " lseek failed errno: " << errno;

    // Assert
    string_t output[SZ_LOGMSG_STR];
    string_t * result = fgets(output, sizeof(output), LogfilePtr);
    if (enable)
    {
        ASSERT_NE(nullptr, result) << mapKey << " failed";
        EXPECT_THAT(result, AllOf(HasSubstr(loglvlStr),
                                  HasSubstr(parameter))) << mapKey << " failed";
    }
    else
    {
        ASSERT_EQ(nullptr, result) << mapKey << " failed";
    }
}

class LogLvlXxxF : public ::testing::Test
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

TEST_F(LogLvlXxxF, IsOkLogLvlEnable)
{
    // Arrange
    LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    int expEnabled = LOGLVL_ERROR | LOGLVL_INFO;
    // Act
    int actEnabled = LogLvlEnable(LOGLVL_ERROR | LOGLVL_INFO);
    // Assert
    EXPECT_EQ(expEnabled, actEnabled);
}

TEST_F(LogLvlXxxF, IsOkLogLvlDisable)
{
    // Arrange
    LogLvlEnable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    int expDisabled = LOGLVL_WARN | LOGLVL_TRACE;
    // Act
    int actDisabled = LogLvlDisable(LOGLVL_WARN | LOGLVL_TRACE);
    // Assert
    EXPECT_EQ(expDisabled, actDisabled);
}

TEST_F(LogLvlXxxF, IsOkLogLvlsInfoAllEnabled)
{
    // Arrange
    constr_t expLogLvlStr = "+E,+W,+I,+T";
    // Act
    LogLvlEnable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    constr_t actLogLvlStr = LogLvlsInfo();
    // Assert
    EXPECT_THAT(actLogLvlStr, HasSubstr(expLogLvlStr));
}

TEST_F(LogLvlXxxF, IsOkLogLvlsInfoAllDisabled)
{
    // Arrange
    constr_t expLogLvlStr = "-E,-W,-I,-T";
    // Act
    LogLvlDisable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    constr_t actLogLvlStr = LogLvlsInfo();
    // Assert
    EXPECT_THAT(actLogLvlStr, HasSubstr(expLogLvlStr));
}

TEST_F(LogLvlXxxF, IsOkLogLvlsEnabled)
{
    // Arrange
    LogLvlEnable(LOGLVL_ERROR | LOGLVL_WARN | LOGLVL_INFO | LOGLVL_TRACE);
    LogLvlDisable(LOGLVL_WARN | LOGLVL_TRACE);
    int expLogLvls = LOGLVL_ERROR | LOGLVL_INFO;
    // Act
    int actLogLvls = LogLvlsEnabled();
    // Assert
    EXPECT_EQ(expLogLvls, actLogLvls);
}

TEST(LogLvlFromChar, IsOk)
{
    char loglvlChar[5] = { 'e', 'w', 'i', 't', '?' };
    int expLogLvl[5] = { LOGLVL_ERROR, LOGLVL_WARN, LOGLVL_INFO, LOGLVL_TRACE, 0 };

    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(expLogLvl[i], LoglvlFromChar(loglvlChar[i]));
    }

}
