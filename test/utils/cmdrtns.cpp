#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::ContainsRegex;

#include <string.h>

#include <typedefs.h>

#include <cmdrtns.h>
#include <memrtns.h>

static std::map<const std::string, std::tuple<constr_t, constr_t>> CmdProcD
{
    { "InvalidLen", std::make_tuple("?a", "ERROR") },
    { "InvalidCmd", std::make_tuple("?invcmd", "ERROR") },
    { "ValidCmdLogInfo", std::make_tuple("?log", "[+-]") },
    { "ValidCmdLogEnable", std::make_tuple("?log:-t", "[+-]") },
    { "ValidCmdLogPkgInfo", std::make_tuple("?pkg", "[+-]") },
    { "ValidCmdLogPkgEnable", std::make_tuple("?pkg:-memory", "[+-]") },
};

class CmdProcF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, constr_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(cmdrtns, CmdProcF, testing::ValuesIn(CmdProcD),
[] (const testing::TestParamInfo<CmdProcF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(CmdProcF, IsOk)
{
    std::tuple<constr_t, constr_t> tuple = GetParam().second;
    constr_t cmdStr = std::get<0>(tuple);
    constr_t expResponse = std::get<1>(tuple);
    string_t *response = NULL;

    // Act
    const int actLen = CmdProc(cmdStr, strlen(cmdStr), &response);

    // Assert
    EXPECT_THAT(response, ContainsRegex(expResponse));
    EXPECT_EQ(actLen, (int) strlen(response));

    MemFree("CmdProcF", &response);
}
