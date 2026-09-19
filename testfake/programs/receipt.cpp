// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <errmsgdefs.h>
#include <receiptdefs.h>

#include <persondata.h>
#include <pmttxndata.h>
#include <recprmdata.h>

#include <wrapreceipt.h>

extern std::vector<std::string> * rffStr;

class receiptCommonF
{
    public:

    static void SetUpTestSuite()
    {
        rffStr = new std::vector<std::string>();
    }
    static void TearDownTestSuite()
    {
        delete rffStr;
    }
    void TearDown()
    {
        rffStr->clear();
    }
};

class ReceiptMainF : public receiptCommonF, public ::testing::Test
{
    protected:
    static void SetUpTestSuite()
    {
        receiptCommonF::SetUpTestSuite();
    }
    static void TearDownTestSuite()
    {
        receiptCommonF::TearDownTestSuite();
    }
    void TearDown()
    {
        receiptCommonF::TearDown();
    }
};

TEST_F(ReceiptMainF, IsOk)
{
    // Arrange
    errmsg_t errmsg;

    typedef struct
    {
        constr_t header;
        bool_t   found;
    } emailln_t;

    emailln_t emailLines[] =
    {
        {"MIME-Version:", 0},
        {"Subject:", 0},
        {"From:", 0},
        {"To:", 0},
        {"Content-Type:", 0}
    };

    int emailLinesSize = sizeof(emailLines) / sizeof(emailln_t);

    // Act
    int ret = ReceiptMain(999998, 98, "RECEIPT", &errmsg);

    // Assert
    ASSERT_EQ(ret, 0);

    int rffStrSize = rffStr->size();

    // for (int i = 0; i < rffStrSize; i++)
    // {
    //     std::cout << rffStr->at(i);
    // }

    for (int i = 0, j = 0; i < rffStrSize && j < emailLinesSize; i++)
    {
        if (!strcmp(rffStr->at(i).c_str(), "\n"))
            continue;

        emailln_t * currEmailLine = &emailLines[j++];
        if (!strncmp(rffStr->at(i).c_str(), currEmailLine->header, strlen(currEmailLine->header)))
        {
            currEmailLine->found = 1;
        }
    }

    for (int j = 0; j < emailLinesSize; j++)
    {
        emailln_t * currEmailLine = &emailLines[j];
        EXPECT_EQ(1, (int) currEmailLine->found) << currEmailLine->header << " not found\n";
    }
}

// Any changes to map key names beginning with "Fail" need to be reflected in fakedfns.cpp.
// These are used to simulate failures in their respective functions. For e.g, "FailFileWriteStr"
// is used to simulate a failure in FailFileWriteStr() with return code SYSCALL_FOPEN. Similarly,
// "FailSysCmd" is used to simulate a failure in SysCmd() with return code 1 and errmsg_t code set to
// SYSCMD_FAILED.
static std::map<std::string, std::tuple<int, int>> receiptFileD
{
    {"Valid", std::make_tuple(0, 0)},
    {"FailSysCmd", std::make_tuple(1, SYSCMD_FAILED)},
};

class receiptFileF : public receiptCommonF, public testing::TestWithParam<std::pair<const std::string, std::tuple<int, int>>>
{
    protected:
    static void SetUpTestSuite()
    {
        receiptCommonF::SetUpTestSuite();
    }
    static void TearDownTestSuite()
    {
        receiptCommonF::TearDownTestSuite();
    }
    void TearDown()
    {
        receiptCommonF::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P(receipt, receiptFileF, testing::ValuesIn(receiptFileD),
[] (const testing::TestParamInfo<receiptFileF::ParamType>& info)
{
    return info.param.first;
});

static testing::AssertionResult receiptFileA(const char * expr1,
                                             const char * expr2,
                                             const int expRet,
                                             const int expErrmsgCode)
{
    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;
    errmsg_t errmsg;

    PersonDataE(8, &person);
    PmttxnDataE(8, 1, &pmttxn);
    RecprmData("RECEIPT", &recprm);
    int actRet = WRAPreceiptFile(&person, &pmttxn, &recprm, "_", "_", &errmsg);
    if (actRet == expRet && errmsg.code == expErrmsgCode)
        return testing::AssertionSuccess();

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    return testing::AssertionFailure() << "receipt." << mapKey << ": " <<
        "exp/act ret[" << expRet << "/" << actRet << 
        "], exp/act errmsgCode[" << expErrmsgCode << "/" << errmsg.code << "]";
}

TEST_P(receiptFileF, IsOk)
{
    // Arrange
    std::tuple<int, int> tuple = GetParam().second;
    const int expRet = std::get<0>(tuple); 
    const int expErrmsgCode = std::get<1>(tuple); 

    // Act & Assert
    ASSERT_PRED_FORMAT2(receiptFileA, expRet, expErrmsgCode);
}

// Any changes to map key names beginning with "Fail" need to be reflected in fakedfns.cpp.
// These are used to simulate failures in their respective functions. For e.g, "FailFileWriteStr"
// is used to simulate a failure in FailFileWriteStr() with return code SYSCALL_FOPEN. Similarly,
// "FailSysCmd" is used to simulate a failure in SysCmd() with return code 1 and errmsg_t code set to
// SYSCMD_FAILED.
static std::map<std::string, std::tuple<int, int, int>> receiptEmailD
{
    {"Valid", std::make_tuple(0, 0, RECTFILE_INIT)},
    {"FailSysCmd", std::make_tuple(1, SYSCMD_FAILED, RECTFILE_DELETE)},
};

class receiptEmailF : public receiptCommonF, public testing::TestWithParam<std::pair<const std::string, std::tuple<int, int, int>>>
{
    protected:
    static void SetUpTestSuite()
    {
        receiptCommonF::SetUpTestSuite();
    }
    static void TearDownTestSuite()
    {
        receiptCommonF::TearDownTestSuite();
    }
    void TearDown()
    {
        receiptCommonF::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P(receipt, receiptEmailF, testing::ValuesIn(receiptEmailD),
[] (const testing::TestParamInfo<receiptEmailF::ParamType>& info)
{
    return info.param.first;
});

static testing::AssertionResult receiptEmailA(const char * expr1,
                                              const char * expr2,
                                              const char * expr3,
                                              const int expRet,
                                              const int expErrmsgCode,
                                              const int expRectFile)
{
    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;
    errmsg_t errmsg;

    PersonDataE(8, &person);
    PmttxnDataE(8, 1, &pmttxn);
    RecprmData("RECEIPT", &recprm);

    FILE *fptr = NULL;

    int actRectFile;
    int actRet = WRAPreceiptEmail(&person, &pmttxn, &recprm, "_", "_", &fptr, &actRectFile, &errmsg);
    if (actRet == expRet && errmsg.code == expErrmsgCode && actRectFile == expRectFile)
        return testing::AssertionSuccess();

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    return testing::AssertionFailure() << "receipt." << mapKey << ": " <<
        "exp/act ret[" << expRet << "/" << actRet << 
        "], exp/act errmsgCode[" << expErrmsgCode << "/" << errmsg.code << "]" <<
        "], exp/act rectfile[" << expRectFile << "/" << actRectFile << "]";
}

TEST_P(receiptEmailF, IsOk)
{
    // Arrange
    std::tuple<int, int, int> tuple = GetParam().second;
    const int expRet = std::get<0>(tuple); 
    const int expErrmsgCode = std::get<1>(tuple); 
    const int expRectFile = std::get<2>(tuple);

    // Act & Assert
    ASSERT_PRED_FORMAT3(receiptEmailA, expRet, expErrmsgCode, expRectFile);
}

static std::map<std::string, std::tuple<int, int>> receiptEmailErrD
{
    {"Invalid1", std::make_tuple(1, RECTFILE_INIT)},
    {"Invalid2", std::make_tuple(1, RECTFILE_OPEN)},
    {"Invalid3", std::make_tuple(1, RECTFILE_DELETE)},
    {"Invalid4", std::make_tuple(1, RECTFILE_OPEN | RECTFILE_DELETE)},
};

class receiptEmailErrF : public testing::TestWithParam<std::pair<const std::string, std::tuple<int, int>>>
{
};


INSTANTIATE_TEST_SUITE_P(receipt, receiptEmailErrF, testing::ValuesIn(receiptEmailErrD),
[] (const testing::TestParamInfo<receiptEmailErrF::ParamType>& info)
{
    return info.param.first;
});

testing::AssertionResult receiptEmailErrA(const char * expr1,
                                          const char * expr2,
                                          const int passedRet,
                                          const int passedRectFile)
{
    int returnedRectFile = passedRectFile;
    FILE *fptr = NULL;
    int actRet = WRAPreceiptEmailErr("_", passedRet, &returnedRectFile, &fptr);
    if (actRet == passedRet && returnedRectFile == RECTFILE_INIT)
        return testing::AssertionSuccess();

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    return testing::AssertionFailure() << "receipt." << mapKey << ": " <<
        "exp/act ret[" << passedRet << "/" << actRet << "], exp/act rectfile[" << RECTFILE_INIT << "/" << returnedRectFile << "]";
}

TEST_P(receiptEmailErrF, IsOk)
{
    // Arrange
    std::tuple<int, int> tuple = GetParam().second;
    const int passedRet = std::get<0>(tuple); 
    const int passedRectFile = std::get<1>(tuple);

    // Act & Assert
    ASSERT_PRED_FORMAT2(receiptEmailErrA, passedRet, passedRectFile);
}
