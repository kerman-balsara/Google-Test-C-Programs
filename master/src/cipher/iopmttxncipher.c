// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <pmttxn.h>

#include <iopmttxncipher.h>

// --------------------------------------------------------------------
// Decrypt data before returning it.
//
// Return:
// 0 - Valid
// < 0 - Major error
// --------------------------------------------------------------------

int DbPmttxnPostReadDecrypt(const pmttxn_r * const pmttxn,
                                  string_t   docNo[SZ_PMT_DOC_NO + 1])
{
    // Decrypt docNo using the database
    return(0);
}
