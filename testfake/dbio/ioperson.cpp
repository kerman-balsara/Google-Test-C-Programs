// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::HasSubstr;

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <person.h>

#include <ioperson.h>
#include <persondata.h>

TEST(DbPersonReadKey, IsOkValidKey)
{
    // Arrange
    const int id = 8;

    person_r expPerson;
    PersonDataE(id, &expPerson);

    person_r actPerson;

    //Act
    int ret = DbPersonReadKey(id, &actPerson);

    //Assert        
    ASSERT_EQ(0, ret);
    EXPECT_EQ(0, memcmp(&expPerson, &actPerson, sizeof(person_r)));
}

TEST(DbPersonReadKey, IsOkInvalidKey)
{
    // Arrange
    const int id = 0;
    person_r person;

    //Act
    int ret = DbPersonReadKey(id, &person);

    //Assert        
    EXPECT_EQ(1, ret);
}

TEST(DbPersonHideEmail, IsOk)
{
    // Arrange
    const int id = 8;
    person_r person;

    string_t expEncryptedEmailAddress[SZ_EMAIL_ADDRESS + 1];
    strlcpy(expEncryptedEmailAddress, "AABBCCDD", sizeof(expEncryptedEmailAddress));
    PersonDataE(id, &person);
    strlcpy(person.emailAddress, expEncryptedEmailAddress, sizeof(person.emailAddress));

   string_t actHiddenEmailAddress[SZ_EMAIL_ADDRESS + 1];

    //Act
    int ret = DbPersonHideEmail(&person, actHiddenEmailAddress);

    //Assert        
    ASSERT_EQ(0, ret);
    EXPECT_STREQ(person.emailAddress, expEncryptedEmailAddress);
    EXPECT_THAT(actHiddenEmailAddress, HasSubstr("*@"));
}

TEST(DbPersonShowEmail, IsOk)
{
    // Arrange
    const int id = 8;
    person_r person;

    string_t expEncryptedEmailAddress[SZ_EMAIL_ADDRESS + 1];
    strlcpy(expEncryptedEmailAddress, "AABBCCDD", sizeof(expEncryptedEmailAddress));
    PersonDataE(id, &person);
    strlcpy(person.emailAddress, expEncryptedEmailAddress, sizeof(person.emailAddress));

   string_t actTextEmailAddress[SZ_EMAIL_ADDRESS + 1];

    //Act
    int ret = DbPersonShowEmail(&person, actTextEmailAddress);

    //Assert        
    ASSERT_EQ(0, ret);
    EXPECT_STREQ(person.emailAddress, expEncryptedEmailAddress);
    EXPECT_THAT(actTextEmailAddress, HasSubstr("@"));
    EXPECT_THAT(actTextEmailAddress, Not(HasSubstr("*")));
}

