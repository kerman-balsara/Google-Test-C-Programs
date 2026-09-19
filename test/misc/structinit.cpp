// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errmsgdefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#include <errmsgrtns.h>
#include <ioperson.h>
#include <iopmttxn.h>
#include <iorecprm.h>

testing::AssertionResult CharIsInit(const char * expr1,
                                    const char charValue)
{
    if (charValue == '\0')
        return testing::AssertionSuccess();

    return testing::AssertionFailure() <<
        testing::UnitTest::GetInstance()->current_test_case()->name() << "." <<
        testing::UnitTest::GetInstance()->current_test_info()->name() <<  ": " <<
        expr1 << " not initialised, has value [" << charValue << "]";
}

testing::AssertionResult NumberIsInit(const char * expr1,
                                       const int numValue)
{
    if (numValue == 0)
        return testing::AssertionSuccess();

    return testing::AssertionFailure() <<
        testing::UnitTest::GetInstance()->current_test_case()->name() << "." <<
        testing::UnitTest::GetInstance()->current_test_info()->name() <<  ": " <<
        expr1 << " not initialised, has value [" << numValue << "]";
}

testing::AssertionResult StringIsInit(const char * expr1,
                                      const char * expr2,
                                      const char * strValue,
                                      const int strSize)
{
    string_t expValue[strSize];
    memset(expValue, 0, strSize);
    if (!memcmp(strValue, expValue, strSize))
        return testing::AssertionSuccess();

    string_t actValue[strSize + 1];
    actValue[strSize] = '\0';

    for (int i = 0; i < strSize; i++)
    {
        if (strValue[i] == '\0')
            actValue[i] = '_';
        else
            actValue[i] = strValue[i];
    }
    
    const char * actValuePtr = actValue;

    return testing::AssertionFailure() <<
        "structinit." << testing::UnitTest::GetInstance()->current_test_info()->name() <<  ": " <<
        expr1 << "[" << strSize << "] not initialised, has value [" << actValuePtr << "]";
}

TEST(DbPersonInitRec, IsOk)
{
    // Arrange
    person_r person;
    memset(&person, '_', sizeof(person_r));

    // Act
    DbPersonInitRec(&person);

    // Assert
    EXPECT_PRED_FORMAT1(NumberIsInit, person.id);
    EXPECT_PRED_FORMAT2(StringIsInit, person.name, sizeof(person.name));
    EXPECT_PRED_FORMAT2(StringIsInit, person.lastname, sizeof(person.lastname));
    EXPECT_PRED_FORMAT2(StringIsInit, person.emailAddress, sizeof(person.emailAddress));
    EXPECT_PRED_FORMAT2(StringIsInit, person.padding, sizeof(person.padding));
}

TEST(DbPmttxnInitRec, IsOk)
{
    // Arrange
    pmttxn_r pmttxn;
    memset(&pmttxn, '_', sizeof(pmttxn_r));

    // Act
    DbPmttxnInitRec(&pmttxn);

    // Assert
    EXPECT_PRED_FORMAT1(NumberIsInit, pmttxn.id);
    EXPECT_PRED_FORMAT1(NumberIsInit, pmttxn.seqNo);
    EXPECT_PRED_FORMAT1(NumberIsInit, pmttxn.amt);
    EXPECT_PRED_FORMAT2(StringIsInit, pmttxn.docNo, sizeof(pmttxn.docNo));
    EXPECT_PRED_FORMAT2(StringIsInit, pmttxn.receiptNo, sizeof(pmttxn.receiptNo));
    EXPECT_PRED_FORMAT2(StringIsInit, pmttxn.date, sizeof(pmttxn.date));
    EXPECT_PRED_FORMAT2(StringIsInit, pmttxn.padding, sizeof(pmttxn.padding));
}

TEST(DbRecprmInitRec, IsOk)
{
    // Arrange
    recprm_r recprm;
    memset(&recprm, '_', sizeof(recprm_r));

    // Act
    DbRecprmInitRec(&recprm);

    // Assert
    EXPECT_PRED_FORMAT2(StringIsInit, recprm.id, sizeof(recprm.id));
    EXPECT_PRED_FORMAT2(StringIsInit, recprm.padding1, sizeof(recprm.padding1));
    EXPECT_PRED_FORMAT2(StringIsInit, recprm.archiveFolderName, sizeof(recprm.archiveFolderName));
    EXPECT_PRED_FORMAT2(StringIsInit, recprm.emailFolderName, sizeof(recprm.emailFolderName));
    EXPECT_PRED_FORMAT2(StringIsInit, recprm.archiveCmd, sizeof(recprm.archiveCmd));
}

TEST(ErrmsgInit, IsOk)
{
    // Arrange
    errmsg_t errmsg;
    memset(&errmsg, '_', sizeof(errmsg_t));

    // Act
    ErrmsgInit(&errmsg);

    // Assert
    EXPECT_PRED_FORMAT1(NumberIsInit, errmsg.code);
    EXPECT_PRED_FORMAT2(StringIsInit, errmsg.message, sizeof(errmsg.message));
}
