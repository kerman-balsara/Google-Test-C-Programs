// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Mock;
using ::testing::Return;

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <person.h>

#include <wrapioperson.h>

TEST(DbPersonReadKey, IsOkValidKey)
{
    // Arrange
    person_r person;

    {
        MockedFnsObj = std::make_shared<MockedFns>();

        InSequence seq;

        EXPECT_CALL(*MockedFnsObj, DbPersonSelectKey(999999, &person))
        .Times(1)
        .WillOnce(Return(0));

        EXPECT_CALL(*MockedFnsObj, DbPersonPostReadDecrypt)
        .Times(0);
    }

    //Act
    int ret = DbPersonReadKey(999999, &person);

    //Assert        
    EXPECT_EQ(0, ret);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(DbPersonReadKey, IsOkInvalidKey)
{
    // Arrange
    person_r person;

    {
        MockedFnsObj = std::make_shared<MockedFns>();

        InSequence seq;

        EXPECT_CALL(*MockedFnsObj, DbPersonSelectKey(999999, &person))
        .Times(1)
        .WillOnce(Return(1));

        EXPECT_CALL(*MockedFnsObj, DbPersonPostReadDecrypt)
        .Times(0);
    }

    //Act
    int ret = DbPersonReadKey(999999, &person);

    //Assert        
    EXPECT_EQ(1, ret);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}
