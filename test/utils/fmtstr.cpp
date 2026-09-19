// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::HasSubstr;

#include <string.h>

// ---------------------
// External definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <fmtamt.h>
#include <fmtstr.h>

TEST(FmtStr, IsOkExactLen)
{
    // Arrange
    string_t expStr[SZ_CYCLIC_STR];
    memset(expStr, ' ', SZ_CYCLIC_STR - 1);
    expStr[SZ_CYCLIC_STR - 2] = '-';
    expStr[SZ_CYCLIC_STR - 1] = '\0';

    string_t actStr[SZ_CYCLIC_STR];

    // Act
    strlcpy(actStr, FmtStr("%-*s%s", SZ_CYCLIC_STR - 2, "", "-"), sizeof(actStr));

    // Assert    
    EXPECT_STREQ(expStr, actStr);
}

TEST(FmtStr, IsOkSmallLen)
{
    // Arrange
    int amt = 550;

    string_t expStr[SZ_CYCLIC_STR];
    strlcpy(expStr, "The amount is: 5.50", sizeof(expStr));

    string_t actStr[SZ_CYCLIC_STR];

    // Act
    strlcpy(actStr, FmtStr("%s: %s", "The amount is", FmtAmt((int64_t) amt, 2)), sizeof(actStr));

    // Assert    
    EXPECT_STREQ(expStr, actStr);
}

TEST(FmtStrDeathTest, DiesOverflow)
{
    // Arrange
    int overflowLen = SZ_CYCLIC_STR + 1;

    string_t overflowStr[overflowLen];
    memset(overflowStr, ' ', overflowLen - 1);
    overflowStr[overflowLen] = '\0';

    // Act & Assert    
    EXPECT_EXIT(FmtStr("%s", overflowStr), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(STR_SIZE_EXCEEDED)));
}

