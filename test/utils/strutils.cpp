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

#include <strutils.h>

static std::map<std::string, std::tuple<constr_t, bool_t>> IsValidStrLenMinMaxD
{
    {"InvalidLen5", std::make_tuple("12345", 0)},
    {"InvalidLen12", std::make_tuple("123456789012", 0)},
    {"ValidLen13", std::make_tuple("1234567890123", 1)},
    {"ValidLen14", std::make_tuple("12345678901234", 1)},
    {"ValidLen16", std::make_tuple("1234567890123456", 1)},
    {"ValidLen18", std::make_tuple("123456789012345678", 1)},
    {"ValidLen19", std::make_tuple("1234567890123456789", 1)},
    {"InvalidLen20", std::make_tuple("12345678901234567890", 0)},
    {"InvalidLen30", std::make_tuple("123456789012345678901234567890", 0)}
};

class IsValidStrLenMinMaxF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, bool_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(strutils, IsValidStrLenMinMaxF, testing::ValuesIn(IsValidStrLenMinMaxD),
[] (const testing::TestParamInfo<IsValidStrLenMinMaxF::ParamType>& info)
{
    return info.param.first;
});

testing::AssertionResult IsValidStrLenMinMaxA(const char * expr1,
                                              const char * expr2,
                                              const char * expr3,
                                              const char * expr4,
                                              constr_t str,
                                              const int minLen,
                                              const int maxLen,
                                              const bool_t expValue)
{
    if (IsValidStrLenMinMax(str, minLen, maxLen) == expValue)
        return testing::AssertionSuccess();

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    return testing::AssertionFailure() << "strutils." << mapKey << ": " <<
        "Length of " << expr1 << "[" << str << "] is not within " << expr2 << "[" << minLen << "] and " << 
        expr3 << "[" << maxLen << "]";
}

TEST_P(IsValidStrLenMinMaxF, IsOk)
{
    // Arrange
    const int minLen = 13;
    const int maxLen = 19;

    std::tuple<constr_t, bool_t> tuple = GetParam().second;
    constr_t str = std::get<0>(tuple); 
    const bool_t expValue = std::get<1>(tuple);

    // Act & Assert
    ASSERT_PRED_FORMAT4(IsValidStrLenMinMaxA, str, minLen, maxLen, expValue);
}

TEST(IsValidStrLenMinMaxDeathTest, DiesMinNeg)
{
    // Arrange
    string_t str[SZ_PMT_DOC_NO + 1];
    strlcpy(str, "str", sizeof(str));

    // Act & Assert
    EXPECT_EXIT(IsValidStrLenMinMax(str, -1, 10), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(IsValidStrLenMinMaxDeathTest, DiesMinZero)
{
    // Arrange
    string_t str[SZ_PMT_DOC_NO + 1];
    strlcpy(str, "str", sizeof(str));

    // Act & Assert
    EXPECT_EXIT(IsValidStrLenMinMax(str, 0, 10), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(IsValidStrLenMinMaxDeathTest, DiesMaxNeg)
{
    // Arrange
    string_t str[SZ_PMT_DOC_NO + 1];
    strlcpy(str, "str", sizeof(str));

    // Act & Assert
    EXPECT_EXIT(IsValidStrLenMinMax(str, 1, -10), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(IsValidStrLenMinMaxDeathTest, DiesMaxZero)
{
    // Arrange
    string_t str[SZ_PMT_DOC_NO + 1];
    strlcpy(str, "str", sizeof(str));

    // Act & Assert
    EXPECT_EXIT(IsValidStrLenMinMax(str, 1, 0), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(IsValidStrLenMinMaxDeathTest, DiesMinGtMax)
{
    // Arrange
    string_t str[SZ_PMT_DOC_NO + 1];
    strlcpy(str, "str", sizeof(str));

    // Act & Assert
    EXPECT_EXIT(IsValidStrLenMinMax(str, 10, 1), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(EmailAddressUserHide, IsOk)
{
    // Arrange
    string_t emailAddress[SZ_EMAIL_ADDRESS + 1];
    strlcpy(emailAddress, "username@domainname.com", sizeof(emailAddress));

    // Act
    EmailAddressUserHide(emailAddress);

    // Assert
    EXPECT_STREQ(emailAddress, "u*******@domainname.com");
}

TEST(PmtDocNoHide, IsOk)
{
    // Arrange
    string_t docNo[SZ_PMT_DOC_NO + 1];
    strlcpy(docNo, "4111111111111111", sizeof(docNo));

    // Act
    PmtDocNoHide(docNo);

    // Assert
    EXPECT_STREQ(docNo, "**************11");
}

TEST(StrRmTrailNewLine, IsOkNewLine)
{
    // Arrange
    string_t strWithNl[SZ_ERRMSG_STR];
    strlcpy(strWithNl, "Invalid command\n", sizeof(strWithNl));
    string_t expStr[SZ_ERRMSG_STR];
    strlcpy(expStr, "Invalid command", sizeof(expStr));

    // Act
    string_t * actStr = StrRmTrailNewLine(strWithNl);

    // Assert
    EXPECT_STREQ(expStr, actStr);
    EXPECT_STREQ(expStr, strWithNl);
}

TEST(StrRmTrailNewLine, IsOkNoNewLine)
{
    // Arrange
    string_t strWithoutNl[SZ_ERRMSG_STR];
    strlcpy(strWithoutNl, "Invalid command", sizeof(strWithoutNl));
    string_t expStr[SZ_ERRMSG_STR];
    strlcpy(expStr, "Invalid command", sizeof(expStr));

    // Act
    string_t * actStr = StrRmTrailNewLine(strWithoutNl);

    // Assert
    EXPECT_STREQ(expStr, actStr);
    EXPECT_STREQ(expStr, strWithoutNl);
}

TEST(ShortToBuf, IsOk)
{
    // Arrange
    char buf[2];

    // Act
    ShortToBuf(6, buf);

    // Assert
    EXPECT_EQ(0x00, buf[0]);
    EXPECT_EQ(0x06, buf[1]);
}

TEST(BufToShort, IsOk)
{
    // Arrange
    char buf[2];
    buf[0] = 0x00;
    buf[1] = 0x06;

    // Act
    short actValue = BufToShort(buf);;

    // Assert
    EXPECT_EQ(6, actValue);
}

TEST(BufPrintf, IsOk)
{
    // Arrange
    char id[SZ_DB_ID];

    // Act
    BufPrintf(id, SZ_DB_ID, "%06d", 10);

    // Assert
    EXPECT_EQ(0, memcmp("000010", id, SZ_DB_ID));
}

TEST(BufPrintf, DiesLenNeg)
{
    // Arrange
    char id[SZ_DB_ID];

    // Act & Assert
    EXPECT_EXIT(BufPrintf(id, -1, "%06d", 10), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufPrintf, DiesLenZero)
{
    // Arrange
    char id[SZ_DB_ID];

    // Act & Assert
    EXPECT_EXIT(BufPrintf(id, 0, "%06d", 10), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufPrintf, DiesStrSizeExceeded)
{
    // Arrange
    char id[SZ_DB_ID];

    // Act & Assert
    EXPECT_EXIT(BufPrintf(id, SZ_DB_ID, "%06d", 9999999), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(STR_SIZE_EXCEEDED)));
}

TEST(BufToStr, IsOk)
{
    // Arrange
    char buf[SZ_CC_NO_MAX_LEN];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "4111", 4);
    char str[SZ_CC_NO_MAX_LEN + 1];

    // Act
    int ret = BufToStr(buf, sizeof(buf), str);

    // Assert
    EXPECT_EQ(4, ret);
    EXPECT_STREQ("4111", str);
}

TEST(BufToStr, DiesLenNeg)
{
    // Arrange
    char buf[SZ_CC_NO_MAX_LEN];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "4111", 4);
    char str[SZ_CC_NO_MAX_LEN + 1];

    // Act & Assert
    EXPECT_EXIT(BufToStr(buf, -1, str), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufToStr, DiesLenZero)
{
    // Arrange
    char buf[SZ_CC_NO_MAX_LEN];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "4111", 4);
    char str[SZ_CC_NO_MAX_LEN + 1];

    // Act & Assert
    EXPECT_EXIT(BufToStr(buf, 0, str), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(StrToBuf, IsOkAllCopied)
{
    // Arrange
    char str[SZ_CC_NO_MAX_LEN + 1];
    strlcpy(str, "4111111111111111111", sizeof(str));
    char buf[SZ_CC_NO_MAX_LEN];
    char expBuf[SZ_CC_NO_MAX_LEN];
    memcpy(expBuf, "4111111111111111111", SZ_CC_NO_MAX_LEN);

    // Act
    int actLen = StrToBuf(str, buf, sizeof(buf));

    // Assert
    EXPECT_EQ((int) strlen(str), actLen);
    EXPECT_EQ(0, memcmp(expBuf, buf, sizeof(buf)));
}

TEST(StrToBuf, IsOkLargeBuffer)
{
    // Arrange
    char str[SZ_CC_NO_MAX_LEN + 1];
    strlcpy(str, "4111111111111111111", sizeof(str));
    char buf[21];
    char expBuf[21];
    memcpy(expBuf, "4111111111111111111  ", sizeof(expBuf));

    // Act
    int actLen = StrToBuf(str, buf, sizeof(buf));

    // Assert
    EXPECT_EQ((int) strlen(str), actLen);
    EXPECT_EQ(0, memcmp(expBuf, buf, sizeof(buf)));
}

TEST(StrToBufDeathTest, DiesLenNeg)
{
    // Arrange
    char str[SZ_CC_NO_MAX_LEN + 1];
    strlcpy(str, "4111111111111111111", sizeof(str));
    char buf[4];

    // Act & Assert
    EXPECT_EXIT(StrToBuf(str, buf, -1), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(StrToBufDeathTest, DiesLenZero)
{
    // Arrange
    char str[SZ_CC_NO_MAX_LEN + 1];
    strlcpy(str, "4111111111111111111", sizeof(str));
    char buf[4];

    // Act & Assert
    EXPECT_EXIT(StrToBuf(str, buf, 0), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(StrToBufDeathTest, DiesBufferTooSmall)
{
    // Arrange
    char str[SZ_CC_NO_MAX_LEN + 1];
    strlcpy(str, "4111111111111111111", sizeof(str));
    char buf[4];

    // Act & Assert
    EXPECT_EXIT(StrToBuf(str, buf, sizeof(buf)), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufLen, IsOk)
{
    // Arrange
    char buf[SZ_CC_NO_MAX_LEN];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "4111", 4);

    // Act
    int ret = BufLen(buf, sizeof(buf));

    // Assert
    EXPECT_EQ(4, ret);
}

TEST(BufLenDeathTest, DiesLenNeg)
{
    // Arrange
    char buf[SZ_CC_NO_MAX_LEN];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "4111", 4);

    // Act & Assert
    EXPECT_EXIT(BufLen(buf, -1), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufLenDeathTest, DiesLenZero)
{
    // Arrange
    char buf[SZ_CC_NO_MAX_LEN];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "4111", 4);

    // Act & Assert
    EXPECT_EXIT(BufLen(buf, 0), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufToUpperCase, IsOk)
{
    // Arrange
    char buf[10];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "a b 2 d", 7);

    char expBuf[10];
    memset(expBuf, ' ', sizeof(expBuf));
    memcpy(expBuf, "A B 2 D", 7);

    // Act
    BufToUpperCase(buf, sizeof(buf));

    // Assert
    EXPECT_EQ(0, memcmp(expBuf, buf, sizeof(buf)));
}

TEST(BufToUpperCase, DiesLenNeg)
{
    // Arrange
    char buf[10];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "a b 2 d", 7);
 
    // Act & Assert
    EXPECT_EXIT(BufToUpperCase(buf, -1), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufToUpperCase, DiesLenZero)
{
    // Arrange
    char buf[10];
    memset(buf, ' ', sizeof(buf));
    memcpy(buf, "a b 2 d", 7);
 
    // Act & Assert
    EXPECT_EXIT(BufToUpperCase(buf, 0), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(LENGTH_INVALID)));
}

TEST(BufPrintf, IsOk2)
{
    // Arrange
    int i = -253;
    char buf[4];
    int buflen = 4;

    // Act
    BufPrintf(buf, buflen, "%04d", i);

    // Assert
    EXPECT_EQ(0, memcmp("-253", buf, 4));
}