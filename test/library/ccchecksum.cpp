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

#include <ccchecksum.h>

static std::map<std::string, std::tuple<constr_t, bool_t>> IsValidCreditCardCheckSumD
{
    {"InvalidCheckSum0", std::make_tuple("411111111110", 0)},
    {"InvalidCheckSum1", std::make_tuple("411111111111", 0)},
    {"InvalidCheckSum2", std::make_tuple("411111111112", 0)},
    {"InvalidCheckSum3", std::make_tuple("411111111113", 0)},
    {"InvalidCheckSum4", std::make_tuple("411111111114", 0)},
    {"InvalidCheckSum5", std::make_tuple("411111111115", 0)},
    {"InvalidCheckSum6", std::make_tuple("411111111116", 0)},
    {"ValidCheckSum7", std::make_tuple("411111111117", 1)},
    {"InvalidCheckSum8", std::make_tuple("411111111118", 0)},
    {"InvalidCheckSum9", std::make_tuple("411111111119", 0)},
    {"ValidCheckSum1", std::make_tuple("4111111111111111", 1)},
    {"ValidCheckSum5", std::make_tuple("41111111111111111115", 1)},
    {"ValidCheckSum6", std::make_tuple("4111998877665540", 1)}
};

class IsValidCreditCardCheckSumF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, bool_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(ccchecksum, IsValidCreditCardCheckSumF, testing::ValuesIn(IsValidCreditCardCheckSumD),
[] (const testing::TestParamInfo<IsValidCreditCardCheckSumF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(IsValidCreditCardCheckSumF, IsOk)
{
    // Arrange
    std::tuple<constr_t, bool_t> tuple = GetParam().second;
    constr_t ccNo = std::get<0>(tuple); 
    const bool_t expReturnCode = std::get<1>(tuple);

    // Act
    bool_t actReturnCode = IsValidCreditCardCheckSum(ccNo);

    // Assert
    EXPECT_EQ((int) expReturnCode, (int) actReturnCode);
}


