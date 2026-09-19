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

#include <typedefs.h>

#include <ccrtns.h>

static std::map<std::string, std::tuple<constr_t, bool_t>> IsValidCreditCardNoD
{
    {"InvalidLen12", std::make_tuple("411111111117", 0)},
    {"InvalidLen20", std::make_tuple("41111111111111111115", 0)},
    {"InvalidCheckSum0", std::make_tuple("4111111111111110", 0)},
    {"ValidCreditCardNo", std::make_tuple("4111111111111111", 1)},
    {"InvalidCheckSum2", std::make_tuple("4111111111111112", 0)},
    {"InvalidCheckSum3", std::make_tuple("4111111111111113", 0)},
    {"InvalidCheckSum4", std::make_tuple("4111111111111114", 0)},
    {"InvalidCheckSum5", std::make_tuple("4111111111111115", 0)},
    {"InvalidCheckSum6", std::make_tuple("4111111111111116", 0)},
    {"InvalidCheckSum7", std::make_tuple("4111111111111117", 0)},
    {"InvalidCheckSum8", std::make_tuple("4111111111111118", 0)},
    {"InvalidCheckSum9", std::make_tuple("4111111111111119", 0)}
};

class IsValidCreditCardNoF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, bool_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(ccrtns, IsValidCreditCardNoF, testing::ValuesIn(IsValidCreditCardNoD),
[] (const testing::TestParamInfo<IsValidCreditCardNoF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(IsValidCreditCardNoF, IsOk)
{
    // Arrange
    std::tuple<constr_t, bool_t> tuple = GetParam().second;
    constr_t ccNo = std::get<0>(tuple); 
    const bool_t expReturnCode = std::get<1>(tuple);

    // Act
    bool_t actReturnCode = IsValidCreditCardNo(ccNo);

    // Assert
    EXPECT_EQ((int) expReturnCode, (int) actReturnCode);
}

