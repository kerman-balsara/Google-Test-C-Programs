// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <errmsgdefs.h>

#include <recprmdata.h>
#include <receiptval.h>

TEST(ReceiptValidateDb, IsOkInvalidPerson)
{
    // Arrange
    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;

    string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1];

    errmsg_t errmsg;

    // Act
    int ret = ReceiptValidateDb(1, 2, "RECEIPT", &person, &pmttxn, &recprm, toEmailAddress, &errmsg);

    // Assert
    ASSERT_EQ(ret, 1);
    EXPECT_EQ(errmsg.code, INVALID_PERSON_REC);
    EXPECT_STRNE(errmsg.message, "");
}

TEST(ReceiptValidateDb, IsOkInvalidPmttxn)
{
    // Arrange
    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;

    string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1];

    errmsg_t errmsg;

    // Act
    int ret = ReceiptValidateDb(2, 100, "RECEIPT", &person, &pmttxn, &recprm, toEmailAddress, &errmsg);

    // Assert
    ASSERT_EQ(ret, 1);
    EXPECT_EQ(errmsg.code, INVALID_PMTTXN_REC);
    EXPECT_STRNE(errmsg.message, "");
}

TEST(ReceiptValidateDb, IsOkInvalidRecprm)
{
    // Arrange
    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;

    string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1];

    errmsg_t errmsg;

    // Act
    int ret = ReceiptValidateDb(2, 1, "ABCDEF", &person, &pmttxn, &recprm, toEmailAddress, &errmsg);

    // Assert
    ASSERT_EQ(ret, 1);
    EXPECT_EQ(errmsg.code, INVALID_RECPRM_REC);
    EXPECT_STRNE(errmsg.message, "");
}

TEST(ReceiptValidateDb, IsOk)
{
    // Arrange
    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;

    string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1];

    errmsg_t errmsg;

    // Act
    int ret = ReceiptValidateDb(2, 1, "RECEIPT", &person, &pmttxn, &recprm, toEmailAddress, &errmsg);

    // Assert
    ASSERT_EQ(ret, 0) << " ret:[" << ret << "] code:[" << errmsg.code << "] message:[" << errmsg.message << "]";
}

TEST(ReceiptValidateFolders, IsOkInvalidEmailFolder)
{
    // Arrange
    errmsg_t errmsg;
    recprm_r recprm;
    string_t invalidFolder[SZ_FOLDER_NAME + 1];
    strlcpy(invalidFolder, "/home/kerman/projects/GoogleTestC/testfolder/invalidfolder", sizeof(invalidFolder));
    RecprmData("RECEIPT", &recprm);
    strlcpy(recprm.emailFolderName, invalidFolder, sizeof(recprm.emailFolderName));

    // Act
    int ret = ReceiptValidateFolders(&recprm, &errmsg);

    // Assert
    ASSERT_EQ(ret, 1);
    EXPECT_EQ(errmsg.code, INVALID_RECPRM_EMAIL);
    EXPECT_STRNE(errmsg.message, "");
}

TEST(ReceiptValidateFolders, IsOkInvalidArchiveFolder)
{
    // Arrange
    errmsg_t errmsg;
    recprm_r recprm;
    string_t invalidFolder[SZ_FOLDER_NAME + 1];
    strlcpy(invalidFolder, "/home/kerman/projects/GoogleTestC/testfolder/invalidfolder", sizeof(invalidFolder));
    RecprmData("RECEIPT", &recprm);
    strlcpy(recprm.archiveFolderName, invalidFolder, sizeof(recprm.archiveFolderName));

    // Act
    int ret = ReceiptValidateFolders(&recprm, &errmsg);

    // Assert
    ASSERT_EQ(ret, 1);
    EXPECT_EQ(errmsg.code, INVALID_RECPRM_ARCHIVE);
    EXPECT_STRNE(errmsg.message, "");
}

TEST(ReceiptValidateFolders, IsOk)
{
    // Arrange
    errmsg_t errmsg;
    recprm_r recprm;
    RecprmData("RECEIPT", &recprm);

    // Act
    int ret = ReceiptValidateFolders(&recprm, &errmsg);

    // Assert
    ASSERT_EQ(ret, 0) << " ret:[" << ret << "] code:[" << errmsg.code << "] message:[" << errmsg.message << "]";
}

TEST(ReceiptSelectFolder, IsOkArchiveFolder)
{
    // Arrange
    errmsg_t errmsg;
    recprm_r recprm;
    string_t expFullPathName[SZ_FULL_PATH_NAME + 1];
    int personId = 999998;
    int pmttxnSeqNo = 1;
    RecprmData("RECEIPT", &recprm);
    snprintf(expFullPathName, sizeof(expFullPathName), "%s/%06d_%06d_receipt", recprm.archiveFolderName, personId, pmttxnSeqNo);

    string_t actFullPathName[SZ_FULL_PATH_NAME + 1];
    bool_t createFile;

    // Act
    int ret = ReceiptSelectFolder(personId, pmttxnSeqNo, &recprm, actFullPathName, &createFile, &errmsg);

    // Assert
    ASSERT_EQ(ret, 0) << " ret:[" << ret << "] code:[" << errmsg.code << "] message:[" << errmsg.message << "]";
    EXPECT_STREQ(expFullPathName, actFullPathName);
    EXPECT_EQ(0, (int) createFile);
}

TEST(ReceiptSelectFolder, IsOkEmailFolder)
{
    // Arrange
    errmsg_t errmsg;
    recprm_r recprm;
    string_t expFullPathName[SZ_FULL_PATH_NAME + 1];
    int personId = 999998;
    int pmttxnSeqNo = 3;
    RecprmData("RECEIPT", &recprm);
    snprintf(expFullPathName, sizeof(expFullPathName), "%s/%06d_%06d_receipt", recprm.emailFolderName, personId, pmttxnSeqNo);

    string_t actFullPathName[SZ_FULL_PATH_NAME + 1];
    bool_t createFile;

    // Act
    int ret = ReceiptSelectFolder(personId, pmttxnSeqNo, &recprm, actFullPathName, &createFile, &errmsg);

    // Assert
    ASSERT_EQ(ret, 0) << " ret:[" << ret << "] code:[" << errmsg.code << "] message:[" << errmsg.message << "]";
    EXPECT_STREQ(expFullPathName, actFullPathName);
    EXPECT_EQ(1, (int) createFile);
}

TEST(ReceiptSelectFolder, IsOkAnotherUserError)
{
    // Arrange
    errmsg_t errmsg;
    recprm_r recprm;
    int personId = 999998;
    int pmttxnSeqNo = 2;
    RecprmData("RECEIPT", &recprm);

    string_t actFullPathName[SZ_FULL_PATH_NAME + 1];
    bool_t createFile;

    // Act
    int ret = ReceiptSelectFolder(personId, pmttxnSeqNo, &recprm, actFullPathName, &createFile, &errmsg);

    // Assert
    ASSERT_EQ(ret, 1);
    EXPECT_EQ(0, (int) createFile);
    EXPECT_EQ(errmsg.code, RECEIPT_ANOTHER_USER);
    EXPECT_STRNE(errmsg.message, "");
}
