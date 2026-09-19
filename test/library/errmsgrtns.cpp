// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errmsgdefs.h>
#include <typedefs.h>
#include <syssizes.h>

#include <errmsgrtns.h>

TEST(ErrmsgSet, IsOkSmallLen)
{
    // Arrange
    int expCode = 100;
    constr_t expMessage = "Small message";

    errmsg_t actErrmsg;

    // Act
    ErrmsgSet(expCode, expMessage, &actErrmsg);

    // Assert
    EXPECT_EQ(expCode, actErrmsg.code);
    EXPECT_STREQ(expMessage, actErrmsg.message);
}

TEST(ErrmsgSet, IsOkExactLen)
{
    // Arrange
    int expCode = 100;
    string_t expMessage[SZ_ERRMSG_STR];
    memset(expMessage, '1', SZ_ERRMSG_STR - 1);
    expMessage[SZ_ERRMSG_STR - 1] = '\0';

    errmsg_t actErrmsg;

    // Act
    ErrmsgSet(expCode, expMessage, &actErrmsg);

    // Assert
    EXPECT_EQ(expCode, actErrmsg.code);
    EXPECT_STREQ(expMessage, actErrmsg.message);
}

TEST(ErrmsgSet, IsOkExcessLen)
{
    // Arrange
    int expCode = 100;
    string_t longMessage[SZ_ERRMSG_STR + 1];
    memset(longMessage, '1', SZ_ERRMSG_STR);
    longMessage[SZ_ERRMSG_STR] = '\0';
 
    string_t expMessage[SZ_ERRMSG_STR];
    memset(expMessage, '1', SZ_ERRMSG_STR);
    expMessage[SZ_ERRMSG_STR - 1] = '\0';

    errmsg_t actErrmsg;

    // Act
    ErrmsgSet(expCode, longMessage, &actErrmsg);

    // Assert
    EXPECT_EQ(expCode, actErrmsg.code);
    EXPECT_STREQ(expMessage, actErrmsg.message);
}