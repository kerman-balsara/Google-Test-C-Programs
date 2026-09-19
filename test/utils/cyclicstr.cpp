// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <map>
#include <string>
#include <tuple>
#include <utility>

using ::testing::HasSubstr;

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <cyclicstr.h>

static std::map<std::string, std::tuple<constr_t, constr_t>> CyclicStrAddD
{
    {"ValidAdd1", std::make_tuple("123456789012345678901234567890123456789012345678901234567890",
                                  "123456789012345678901234567890123456789012345678901234567890")},
    {"ValidAdd2", std::make_tuple("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ")},
    {"ValidAdd3", std::make_tuple("123456789012345678901234567890123456789012345678901234567890",
                                  "123456789012345678901234567890123456789012345678901234567890")}
};

class CyclicStrAddF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, constr_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(cyclicstr, CyclicStrAddF, testing::ValuesIn(CyclicStrAddD),
[] (const testing::TestParamInfo<CyclicStrAddF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(CyclicStrAddF, IsOk)
{
    // Arrange
    std::tuple<constr_t, constr_t> tuple = GetParam().second;
    constr_t inStr = std::get<0>(tuple); 
    constr_t expStr = std::get<1>(tuple);

    // Act
    const string_t * actStr = CyclicStrAdd(inStr);

    // Assert
    EXPECT_STREQ(expStr, actStr);
}

TEST(CyclicStrAddDeathTest, DiesOverflow)
{
    // Arrange
    // Test with string length > SZ_CYCLIC_STR
    string_t str[SZ_CYCLIC_STR + 1];
    memset(str, '1', SZ_CYCLIC_STR);
    str[SZ_CYCLIC_STR] = '\0';

    // Act & Assert
    EXPECT_EXIT(CyclicStrAdd(str), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(STR_SIZE_EXCEEDED)));
}

