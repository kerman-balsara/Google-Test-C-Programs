// ---------------------
// Internal definitions.
// ---------------------

#include <pmttxn.h>

#include <iopmttxnrtns.h>

// --------------------------------------------------------------------
// Read from table.
//
// Return:
// 0 - Valid
// 1 - Invalid
// < 0 - Major error
// --------------------------------------------------------------------

int DbPmttxnSelectKey(const int id, const int seqNo, pmttxn_r * const pmttxn)
{
    pmttxn->id = id;
    pmttxn->seqNo = seqNo;

    // Set up pmttxn_r (as would have been done by the SQL)
    return(1);
}
