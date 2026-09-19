// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#include <persondata.h>
#include <pmttxndata.h>
#include <recprmdata.h>

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
}