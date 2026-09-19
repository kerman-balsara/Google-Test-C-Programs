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

#include <recprm.h>

#include <iorecprm.h>
#include <recprmdata.h>

TEST(DbRecprmReadKey, IsOkValidKey)
{
    // Arrange
    string_t id[SZ_RECPRM_ID + 1];
    strlcpy(id, "RECEIPT", sizeof(id));

    recprm_r expRecprm;
    RecprmData(id, &expRecprm);

    recprm_r actRecprm;

    //Act
    int ret = DbRecprmReadKey(id, &actRecprm);

    //Assert        
    ASSERT_EQ(0, ret);
    ASSERT_EQ(0, memcmp(&expRecprm, &actRecprm, sizeof(recprm_r)));
}

TEST(DbRecprmReadKey, IsOkInvalidKey)
{
    // Arrange
    string_t id[SZ_RECPRM_ID + 1];
    strlcpy(id, "ABCDEF", sizeof(id));

    recprm_r recprm;

    //Act
    int ret = DbRecprmReadKey(id, &recprm);

    //Assert        
    EXPECT_EQ(1, ret);
}
