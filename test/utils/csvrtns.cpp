#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::HasSubstr;

#include <errcodes.h>
#include <lnklstdefs.h>
#include <strlstdefs.h>
#include <typedefs.h>

#include <csvrtns.h>
#include <lnklstrtns.h>

static strlst_t * strlstHead = NULL;

class CsvStrLstF : public ::testing::Test
{
    protected:
    void TearDown() override
    {
        LnkLstFree("CsvStrLstF", &strlstHead);
    }
};

TEST_F(CsvStrLstF, IsOkMultiItems)
{
    // Arrange
    string_t csvstr[100];
    strlcpy(csvstr, "ABCD,EF,G", sizeof(csvstr));
    int expItems = 3;
    string_t expArr[3][100] = { "ABCD", "EF", "G" };

    // Act
    int actItems = CsvStrLst(csvstr, &strlstHead);

    // ASSERT
    ASSERT_EQ(expItems, actItems);

    strlst_t * strlstElem = strlstHead;
    for (int i = 0; i < actItems; i++, strlstElem = strlstElem->next)
        EXPECT_STREQ(expArr[i], strlstElem->str);
}

TEST_F(CsvStrLstF, IsOkOneItem)
{
    // Arrange
    string_t csvstr[100];
    strlcpy(csvstr, "ABCD", sizeof(csvstr));
    int expItems = 1;

    // Act
    int actItems = CsvStrLst(csvstr, &strlstHead);

    // ASSERT
    strlst_t * strlstElem = strlstHead;
    ASSERT_EQ(expItems, actItems);
    EXPECT_STREQ("ABCD", strlstElem->str);
}

TEST_F(CsvStrLstF, IsOkZeroItems)
{
    // Arrange
    string_t csvstr[100];
    strlcpy(csvstr, "", sizeof(csvstr));
    int expItems = 0;

    // Act
    int actItems = CsvStrLst(csvstr, &strlstHead);

    // ASSERT
    EXPECT_EQ(expItems, actItems);
}

TEST(CsvStrLstDeathTest, DiesDoubleQuote)
{
    // Arrange
    string_t csvstr[100];
    strlcpy(csvstr, "ABCD,\"E,F\",G", sizeof(csvstr));
 
    // Act & Assert
    EXPECT_EXIT(CsvStrLst(csvstr, &strlstHead), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(STR_QUOTES_INVALID)));
}

TEST(CsvStrLstDeathTest, DiesSingleQuote)
{
    // Arrange
    string_t csvstr[100];
    strlcpy(csvstr, "ABCD,\'E,F\',G", sizeof(csvstr));
 
    // Act & Assert
    EXPECT_EXIT(CsvStrLst(csvstr, &strlstHead), ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(STR_QUOTES_INVALID)));
}

