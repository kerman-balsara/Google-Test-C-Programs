// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::StartsWith;

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <pmttxn.h>

#include <iopmttxn.h>
#include <pmttxndata.h>

TEST(DbPmttxnReadKey, IsOkValidKey)
{
    // Arrange
    const int id = 8;
    const int seqNo = 1;

    pmttxn_r expPmttxn;
    PmttxnDataE(id, seqNo, &expPmttxn);
    pmttxn_r actualPmttxn;

    //Act
    int ret = DbPmttxnReadKey(id, seqNo, &actualPmttxn);

    //Assert        
    ASSERT_EQ(0, ret);
    ASSERT_EQ(0, memcmp(&expPmttxn, &actualPmttxn, sizeof(pmttxn_r)));
}

TEST(DbPmttxnReadKey, IsOkInvalidKey)
{
    // Arrange
    const int id = 0;
    const int seqNo = 1;
    pmttxn_r pmttxn;

    //Act
    int ret = DbPmttxnReadKey(id, seqNo, &pmttxn);

    //Assert        
    EXPECT_EQ(1, ret);
}

TEST(DbPmttxnHideDocNo, IsOk)
{
    // Arrange
    const int id = 8;
    const int seqNo = 1;
    pmttxn_r pmttxn;

    string_t expEncryptedDocNo[SZ_PMT_DOC_NO + 1];
    strlcpy(expEncryptedDocNo, "AABBCCDD", sizeof(expEncryptedDocNo));
    PmttxnDataE(id, seqNo, &pmttxn);
    strlcpy(pmttxn.docNo, expEncryptedDocNo, sizeof(pmttxn.docNo));

    string_t actHiddenDocNo[SZ_PMT_DOC_NO + 1];

    //Act
    int ret = DbPmttxnHideDocNo(&pmttxn, actHiddenDocNo);

    //Assert  
    ASSERT_EQ(0, ret);
    EXPECT_STREQ(pmttxn.docNo, expEncryptedDocNo);
    EXPECT_THAT(actHiddenDocNo, StartsWith("*"));
}

