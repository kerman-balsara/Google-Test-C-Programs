// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <pmttxn.h>

#include <iopmttxn.h>
#include <pmttxndata.h>

// Return fake data with encrypted docNo
void PmttxnDataE(const int id, const int seqNo, pmttxn_r * const pmttxn)
{
    DbPmttxnInitRec(pmttxn);
    pmttxn->id = id;
    pmttxn->seqNo = seqNo;
    pmttxn->amt = 10000;
    strlcpy(pmttxn->docNo, "AABBCCDDEEFFGGHHII", sizeof(pmttxn->docNo));
    strlcpy(pmttxn->date, "2023-12-23 12:25:30", sizeof(pmttxn->date));
    strlcpy(pmttxn->receiptNo, "R123456", sizeof(pmttxn->receiptNo));
}

// Return fake decrypted docNo
void PmttxnDataP(const int id, const int seqNo, string_t docNo[SZ_PMT_DOC_NO + 1])
{
    memset(docNo, 0, SZ_PMT_DOC_NO + 1);
    snprintf(docNo, SZ_PMT_DOC_NO + 1, "4111%06d%06d", id, seqNo);
}
