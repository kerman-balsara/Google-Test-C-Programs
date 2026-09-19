// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>
#include <vector>

using ::testing::HasSubstr;
using ::testing::MatchesRegex;

#include <stdlib.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <fmtamt.h>
#include <fmtdate.h>
#include <person.h>
#include <persondata.h>
#include <pmttxn.h>
#include <pmttxndata.h>
#include <receiptwrite.h>
#include <recprm.h>
#include <recprmdata.h>

std::vector<std::string> * rffStr;

class ReceiptFileWriteF : public ::testing::Test
{
    protected:

    void SetUp() override
    {
        rffStr = new std::vector<std::string>();
    }

    void TearDown() override
    {
        delete rffStr;
    }
};

TEST_F(ReceiptFileWriteF, IsOk)
{
    // Arrange
    person_r person;
    pmttxn_r pmttxn;
    recprm_r recprm;

    const int id = 8;

    PersonDataE(id, &person);
    PmttxnDataE(id, 1, &pmttxn);
    RecprmData("RECEIPT", &recprm);

    string_t expId[9 + 1];
    snprintf(expId, sizeof(expId), "Id %d", id);

    string_t expFromDetails[SZ_EMAIL_ADDRESS + 2 + 1];
    snprintf(expFromDetails, sizeof(expFromDetails), "<%s>", recprm.emailAddress);

    string_t expToDetails [SZ_PERSON_NAME + SZ_PERSON_LASTNAME + SZ_EMAIL_ADDRESS + 4 + 1];
    string_t toEmailAddress[SZ_EMAIL_ADDRESS + 1];

    strlcpy(toEmailAddress, "kbtest8@gmail.com", sizeof(toEmailAddress));
    snprintf(expToDetails, sizeof(expToDetails), "%s %s <%s>", person.name, person.lastname, toEmailAddress);

    string_t expPersonDetails [SZ_PERSON_NAME + SZ_PERSON_LASTNAME + 1 + 1];
    snprintf(expPersonDetails, sizeof(expPersonDetails), "%s %s", person.name, person.lastname);

    string_t expPaymentDetails[100 + 1];
    snprintf(expPaymentDetails, sizeof(expPaymentDetails), "%s on %s", FmtAmt(pmttxn.amt, 2), FmtDateDDMonYYYY(pmttxn.date));

    constr_t expReceiptNo = pmttxn.receiptNo;

    typedef struct
    {
        constr_t header;
        constr_t expSubstr;
        constr_t expRegex;
        constr_t errMsg;
        bool_t   found;
    } emailln_t;

    emailln_t emailLines[] =
    {
        {"MIME-Version:", "1.0", NULL, "Mime version", 0},
        {"Subject:", expId, NULL, "Subject ... details", 0},
        {"From:", expFromDetails, NULL, "From ... details", 0},
        {"To:", expToDetails, NULL, "To ... details", 0},
        {"Content-Type:", "text/plain", NULL, "Content type", 0},
        {"Dear", expPersonDetails, NULL, "Dear ... details", 0},
        {"Thank you", expPaymentDetails, NULL, "Thank you ... details", 0},
        {"Your receipt", expReceiptNo, NULL, "Your receipt ... details", 0}
    };

    int emailLinesSize = sizeof(emailLines) / sizeof(emailln_t);

    // Act
    ReceiptFileWrite(&person, &pmttxn, &recprm, toEmailAddress, NULL);

    // Assert

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
            EXPECT_THAT(rffStr->at(i), HasSubstr(currEmailLine->expSubstr)) <<
                "Line " << i + 1 << " " << currEmailLine->errMsg << " mismatch\n";
            if (currEmailLine->expRegex)
            {
                EXPECT_THAT(rffStr->at(i).c_str(), MatchesRegex(currEmailLine->expRegex));
            }    
        }
    }

    for (int j = 0; j < emailLinesSize; j++)
    {
        emailln_t * currEmailLine = &emailLines[j];
        EXPECT_EQ(1, (int) currEmailLine->found) << currEmailLine->errMsg << " not found\n";
    }
}
