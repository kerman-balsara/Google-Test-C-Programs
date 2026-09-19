// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <errmsgdefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#include <errmsgrtns.h>
#include <llstdio.h>
#include <persondata.h>
#include <pmttxndata.h>
#include <recprmdata.h>

extern std::vector<std::string> * rffStr;

// To simulate a failure, add an entry in the fakeRespArray[].
typedef struct
{
    constr_t mapKey;
    int errorCode;
    constr_t errorMessage;
    int returnCode;
} fakeresp_t;

fakeresp_t fakerespArray[] = 
{
    {"FailSysCmd", SYSCMD_FAILED, "Invalid command", 1}
};

static int fakerespArraySize = sizeof(fakerespArray) / sizeof(fakeresp_t);

static int findFakeResp(constr_t reqdMapKey, errmsg_t * const errmsg);

extern "C"
{
    // Only even ids are valid.
    int DbPersonSelectKey(const int id, person_r * const person)
    {
        if (id <= 0 || id % 2) return(1);
        PersonDataE(id, person);
        return(0);
    }
    int DbPersonPostReadDecrypt(const person_r * const person, string_t emailAddress[SZ_EMAIL_ADDRESS + 1])
    {
        PersonDataP(person->id, emailAddress);
        return(0);
    }
    // Only even ids are valid.
    // seqNo < 100 are valid
    int DbPmttxnSelectKey(const int id, const int seqNo, pmttxn_r * const pmttxn)
    {
        if (id <= 0 || id % 2) return(1);
        if (seqNo >= 100) return(1);
        PmttxnDataE(id, seqNo, pmttxn);
        return(0);
    }
    int DbPmttxnPostReadDecrypt(const pmttxn_r * const pmttxn, string_t docNo[SZ_PMT_DOC_NO + 1])
    {
        PmttxnDataP(pmttxn->id, pmttxn->seqNo, docNo);
        return(0);
    }
    // Only string id "RECEIPT" is valid.
    int DbRecprmSelectKey(const string_t id[SZ_RECPRM_ID + 1], recprm_r * const recprm)
    {
        if (strcmp(id, "RECEIPT")) return(1);
        RecprmData(id, recprm);
        return(0);
    }
    void FileOpen(FILE **stream, constr_t fileName, constr_t modes)
    {
    }
    void FileWriteStr(FILE *stream, const string_t * const str)
    {
        rffStr->push_back(str ? str : "\n");
    }
    void FileWriteStrF(FILE *stream, const string_t * const format, ...)
    {
        string_t wkStr[255 + 1];

        va_list arg;
        va_start(arg, format);
        (void) vsnprintf(wkStr, sizeof(wkStr), format, arg);
        va_end(arg);

        rffStr->push_back(wkStr);
    }
    void FileClose(FILE **stream)
    {
    }

    void FileDelete(constr_t fileName)
    {
    }

    int SysCmd(constr_t cmd, errmsg_t * const errmsg)
    {
        int ret;

        if ((ret = findFakeResp("FailSysCmd", errmsg)))
            return(ret);
 
        return(0);
    }
}

static int findFakeResp(constr_t reqdMapKey, errmsg_t * const errmsg)
{
    string_t mapKeyStr[strlen(testing::UnitTest::GetInstance()->current_test_info()->name()) + 1];
    strlcpy(mapKeyStr, testing::UnitTest::GetInstance()->current_test_info()->name(), sizeof(mapKeyStr));
    string_t * mapKey = strchr(mapKeyStr, '/');
    if (!mapKey) return(0);

    mapKey++;

    if (strcmp(mapKey, reqdMapKey))
        return(0);

    for (int i = 0; i < fakerespArraySize; i++)
    {
        if (!strcmp(fakerespArray[i].mapKey, mapKey))
        {
            if (errmsg)
            {
                ErrmsgInit(errmsg);
                errmsg->code = fakerespArray[i].errorCode;
                strlcpy(errmsg->message, fakerespArray[i].errorMessage, sizeof(errmsg->message));
            }

            return(fakerespArray[i].returnCode);
        }
    }

    return(0);
}
