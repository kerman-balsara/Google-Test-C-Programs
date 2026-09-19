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

#include <syssizes.h>
#include <typedefs.h>

#include <fmtamt.h>

static std::map<std::string, std::tuple<int64_t, int, constr_t>> FmtAmtD
{
    {"FmtAmt1", std::make_tuple((int64_t) 5, 0, "5")},
    {"FmtAmt2", std::make_tuple((int64_t) 55, 0, "55")},
    {"FmtAmt3", std::make_tuple((int64_t) 555, 0, "555")},
    {"FmtAmt4", std::make_tuple((int64_t) 5, 2, "0.05")},
    {"FmtAmt5", std::make_tuple((int64_t) 55, 2, "0.55")},
    {"FmtAmt6", std::make_tuple((int64_t) 555, 2, "5.55")},
    {"FmtAmt7", std::make_tuple((int64_t) 5555, 2, "55.55")},
    {"FmtAmt8", std::make_tuple((int64_t) -5, 0, "-5")},
    {"FmtAmt9", std::make_tuple((int64_t) -55, 0, "-55")},
    {"FmtAmt10", std::make_tuple((int64_t) -555, 0, "-555")},
    {"FmtAmt11", std::make_tuple((int64_t) -5, 2, "-0.05")},
    {"FmtAmt12", std::make_tuple((int64_t) -55, 2, "-0.55")},
    {"FmtAmt13", std::make_tuple((int64_t) -555, 2, "-5.55")},
    {"FmtAmt14", std::make_tuple((int64_t) -5555, 2, "-55.55")}
};

class FmtAmtF : public testing::TestWithParam<std::pair<const std::string, std::tuple<int64_t, int, constr_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(fmtamt, FmtAmtF, testing::ValuesIn(FmtAmtD),
[] (const testing::TestParamInfo<FmtAmtF::ParamType>& info)
{
    return info.param.first;
});

static testing::AssertionResult FmtAmtA(const char * expr1,
                                        const char * expr2,
                                        const char * expr3,
                                        const int64_t amt,
                                        const int places,
                                        constr_t expAmtStr)
{
    string_t actAmtStr[SZ_AMT_STR];
    strlcpy(actAmtStr, FmtAmt(amt, places), sizeof(actAmtStr));

    if (!strcmp(expAmtStr, actAmtStr))
        return testing::AssertionSuccess();

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    return testing::AssertionFailure() << "fmtamt." << mapKey << ": " <<
        expr1 << "[" << amt << "] " << expr2 << "[" << places <<  
        "] exp[" << expAmtStr << "] act[" << actAmtStr << "]";
}

TEST_P(FmtAmtF, IsOk)
{
    // Arrange
     std::tuple<int64_t, int, constr_t> tuple = GetParam().second;
    int64_t amt = std::get<0>(tuple);
    int places = std::get<1>(tuple);
    constr_t expAmtStr = std::get<2>(tuple);

    // Act & Assert
    ASSERT_PRED_FORMAT3(FmtAmtA, amt, places, expAmtStr);
}

