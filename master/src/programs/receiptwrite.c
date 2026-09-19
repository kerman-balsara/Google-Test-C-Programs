// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>
#include <pmttxn.h>
#include <recprm.h>

#include <ioperson.h>
#include <iopmttxn.h>
#include <iopmttxn.h>
#include <iorecprm.h>
#include <fileutils.h>
#include <filertns.h>
#include <fmtamt.h>
#include <fmtdate.h>
#include <receiptwrite.h>

// --------------------------------------------------------------------
// Write receipt file.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/receiptwrite.cpp
// --------------------------------------------------------------------

void ReceiptFileWrite(const person_r * const person,
                      const pmttxn_r * const pmttxn,
                      const recprm_r * const recprm,
                      const string_t   toEmailAddress[SZ_EMAIL_ADDRESS + 1],
                            FILE     * fPtr)
{
    FileWriteStr(fPtr, "MIME-Version: 1.0\n");
    FileWriteStrF(fPtr, "Subject: Payment Acknowledgement: Customer Id %d\n", pmttxn->id);
    FileWriteStrF(fPtr, "From: <%s>\n", recprm->emailAddress);
    FileWriteStrF(fPtr, "To: %s %s <%s>\n", person->name, person->lastname, toEmailAddress);
    FileWriteStr(fPtr, "Content-Type: text/plain; charset=\"UTF-8\"\n");
    FileWriteStr(fPtr, NULL);

    FileWriteStrF(fPtr, "Dear %s %s,\n", person->name, person->lastname);

    FileWriteStrF(fPtr, "Thank you for your payment of %s on %s\n", FmtAmt(pmttxn->amt, 2), FmtDateDDMonYYYY(pmttxn->date));
    FileWriteStrF(fPtr, "Your receipt number is %s\n", pmttxn->receiptNo);
    FileWriteStr(fPtr, NULL);

    FileWriteStr(fPtr, "Kind Regards,\n");
    FileWriteStr(fPtr, "The team at kbtest\n");
    FileWriteStr(fPtr, NULL);

    FileWriteStr(fPtr, "This is an auto-generated acknowledgement. Please do not reply to this mail.\n");
}
