#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::HasSubstr;

#include <errno.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <errcodes.h>

#include <logpkgdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <logpkgcmds.h>
#include <logpkgrtns.h>
#include <logrtns.h>

static FILE * LogfilePtr;
static constr_t LogfileName = "/home/kerman/projects/GoogleTestC/testfolder/logpkgrtns.txt";
static int SaveStderrFd;
static long SaveFilePos;

static int saveLogLvlsEnabled;
static string_t saveLogPkgInfo[SZ_LOGPKG_INFO];

// Testing Log... and Log...F functions. Since the setup is resource intensive, we use function pointers to call the
// functions we want to test. We write a single line to the log using the functions under test and read them back to
// verify the content. 
static std::map<std::string, std::tuple<logpkg_t, bool_t, void (*)(logpkg_t, constr_t), void (*)(logpkg_t, constr_t, ...),
    constr_t, constr_t, constr_t>> LogPkgXxxD
{
    {"LogPkgTraceEnable", std::make_tuple(LOGPKG_MEMORY, 1, LogPkgTrace, nullptr, nullptr, "LogPkgTrace log this", "TRACE") },
    {"LogPkgTraceFEnable", std::make_tuple(LOGPKG_MEMORY, 1, nullptr, LogPkgTraceF, "%s", "LogPkgTraceF log this", "TRACE") },

    {"LogPkgTraceDisable", std::make_tuple(LOGPKG_WIRE, 0, LogPkgTrace, nullptr, nullptr, "LogPkgTrace don't log this", "TRACE") },
    {"LogPkgTraceFDisable", std::make_tuple(LOGPKG_WIRE, 0, nullptr, LogPkgTraceF, "%s", "LogPkgTraceF don't log this", "TRACE") },
};

class LogPkgXxxF : public testing::TestWithParam<std::pair<const std::string,
                                                           std::tuple<logpkg_t,
                                                                      bool_t, 
                                                                      void (*)(logpkg_t, constr_t),
                                                                      void (*)(logpkg_t, constr_t, ...),
                                                                      constr_t,
                                                                      constr_t,
                                                                      constr_t>>>
{
    public:
    static void SetUpTestSuite()
    {
        saveLogLvlsEnabled = LogLvlsEnabled();
        strlcpy(saveLogPkgInfo, LogPkgInfo(), sizeof(saveLogPkgInfo));
        LogLvlEnable(LOGLVL_TRACE);

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
        LogPkgCmdsProc(saveLogPkgInfo);
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

INSTANTIATE_TEST_SUITE_P(logpkgrtns, LogPkgXxxF, testing::ValuesIn(LogPkgXxxD),
[] (const testing::TestParamInfo<LogPkgXxxF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(LogPkgXxxF, IsOk)
{
    // Arrange
    std::tuple<int , bool_t, void (*)(logpkg_t, constr_t), void (*)(logpkg_t, constr_t, ...), constr_t, constr_t, constr_t> tuple =
        GetParam().second;
    logpkg_t logpkg = (logpkg_t) std::get<0>(tuple);
    bool_t enable = std::get<1>(tuple);
    void (*logpkgfn)(logpkg_t, constr_t) = std::get<2>(tuple);
    void (*logpkgfnf)(logpkg_t, constr_t, ...) = std::get<3>(tuple);
    constr_t format = std::get<4>(tuple);
    constr_t parameter = std::get<5>(tuple);
    constr_t loglvlStr = std::get<6>(tuple);

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    int ret;

    if (enable)
        (void) LogPkgEnable(logpkg);
    else
        (void) LogPkgDisable(logpkg);    

    // Act
    logpkgfn ? logpkgfn(logpkg, parameter) : logpkgfnf(logpkg, format, parameter);

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

class logpkgrtnsF : public ::testing::Test
{
    public:
    static void SetUpTestSuite()
    {
        strlcpy(saveLogPkgInfo, LogPkgInfo(), sizeof(saveLogPkgInfo));
    }
    static void TearDownTestSuite()
    {
        LogPkgCmdsProc(saveLogPkgInfo);
    }
};

TEST_F(logpkgrtnsF, IsOkLogPkgEnable)
{
    // Arrange
    string_t expLogPkgEnabled[SZ_LOGPKG_INFO];
    strlcpy(expLogPkgEnabled, "+MEMORY", sizeof(expLogPkgEnabled));
    // Act
    constr_t actLogPkgEnabled = LogPkgEnable(LOGPKG_MEMORY);
    EXPECT_THAT(actLogPkgEnabled, HasSubstr(expLogPkgEnabled));
}

TEST_F(logpkgrtnsF, IsOkLogPkgDisable)
{
    // Arrange
    string_t expLogPkgDisabled[SZ_LOGPKG_INFO];
    strlcpy(expLogPkgDisabled, "-WIRE", sizeof(expLogPkgDisabled));
    // Act
    constr_t actLogPkgDisabled = LogPkgDisable(LOGPKG_WIRE);
    EXPECT_THAT(actLogPkgDisabled, HasSubstr(expLogPkgDisabled));
}

TEST_F(logpkgrtnsF, IsOkLogPkgInfo)
{
    // Arrange
    string_t expLogPkgInfo[SZ_LOGPKG_INFO];
    strlcpy(expLogPkgInfo, "-MEMORY,+WIRE", sizeof(expLogPkgInfo));
    LogPkgDisable(LOGPKG_MEMORY);
    LogPkgEnable(LOGPKG_WIRE);
    // Act
    constr_t actLogPkgInfo = LogPkgInfo();
    EXPECT_THAT(actLogPkgInfo, HasSubstr(expLogPkgInfo));
}

TEST(LogPkgFromStr, IsOk)
{
    // Arrange
    logpkg_t expLogPkg = LOGPKG_WIRE;
    // Act
    logpkg_t actLogPkg = LogPkgFromStr("WIRE");
    EXPECT_EQ(expLogPkg, actLogPkg);
}

TEST(LogPkgFromStr, IsOkInvalidPkg)
{
    // Arrange
    logpkg_t expLogPkg = LOGPKG_NONE;
    // Act
    logpkg_t actLogPkg = LogPkgFromStr("INVALIDPKG");
    EXPECT_EQ(expLogPkg, actLogPkg);
}

