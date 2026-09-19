#ifndef PMTTXN_H
#define PMTTXN_H

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

typedef struct
{
    int id;                                         //  4
    int seqNo;                                      //  4

    int amt;                                        //  4
    string_t docNo[SZ_PMT_DOC_NO + 1];              // 31
    string_t date[SZ_DATETIME_HMS + 1];             // 20
    string_t receiptNo[SZ_PMT_RECEIPT_NO + 1];      // 11
    char padding[2];                                //  2
                                                    // 76
} pmttxn_r;

#endif	// PMTTXN_H