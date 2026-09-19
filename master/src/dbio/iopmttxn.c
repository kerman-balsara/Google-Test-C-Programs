// ---------------------
// External definitions.
// ---------------------

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <pmttxn.h>

#include <iopmttxn.h>
#include <iopmttxncipher.h>
#include <iopmttxnrtns.h>
#include <strutils.h>

// --------------------------------------------------------------------
// Check whether the passed id / seqNo belongs to a valid pmttxn.
//
// Return:
// 0 - Valid
// 1 - Invalid
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/dbio/iopmttxn.cpp
// --------------------------------------------------------------------

int DbPmttxnReadKey(const int id, const int seqNo, pmttxn_r * const pmttxn)
{
    int ret;

    DbPmttxnInitRec(pmttxn);

    if ((ret = DbPmttxnSelectKey(id, seqNo, pmttxn)))
        return(ret);

    return(DbPmttxnPostRead(pmttxn));
}

// --------------------------------------------------------------------
// Initialise passed record.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/misc/structinit.cpp
// --------------------------------------------------------------------

void DbPmttxnInitRec(pmttxn_r * const pmttxn)
{
    memset(pmttxn, 0, sizeof(pmttxn_r));
    pmttxn->id = 0;
    pmttxn->seqNo = 0;
    pmttxn->amt = 0;
}

// --------------------------------------------------------------------
// Set up record after a successful read from the database.
//
// Return:
// 0 - Valid
// < 0 - Major error
// --------------------------------------------------------------------

int DbPmttxnPostRead(pmttxn_r * const pmttxn)
{
    return(0);
}

// --------------------------------------------------------------------
// Hide decrypted docNo.
//
// Return:
// 0 - Valid
// < 0 - Major error
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/dbio/iopmttxn.cpp
// --------------------------------------------------------------------

int DbPmttxnHideDocNo(const pmttxn_r * const pmttxn,
                            string_t   docNo[SZ_PMT_DOC_NO + 1])
{
    int ret;

    if ((ret = DbPmttxnPostReadDecrypt(pmttxn, docNo)))
        return(ret);

    PmtDocNoHide(docNo);

    return(0);
}
