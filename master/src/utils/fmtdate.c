// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <datmdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <cyclicstr.h>
#include <datmutils.h>
#include <fmtdate.h>

typedef struct
{
    constr_t shortDesc;
    constr_t longDesc;
} mthdesc_t;

static const mthdesc_t mthArray[] =
{
    {"Jan", "January"},
    {"Feb", "February"},
    {"Mar", "March"},
    {"Apr", "April"},
    {"May", "May"},
    {"Jun", "June"},
    {"Jul", "July"},
    {"Aug", "August"},
    {"Sep", "September"},
    {"Oct", "October"},
    {"Nov", "November"},
    {"Dec", "December"}
};

// --------------------------------------------------------------------
// Format passed date.
//
// Return:
// string - formatted date (uses cyclic string so use immediately)
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/fmtdate.cpp
// --------------------------------------------------------------------

string_t * FmtDateDDMonYYYY(const string_t inDate[SZ_DATETIME_HMS + 1])
{
	string_t fmtDate[9 + 1];

    datm_t datm;

    DatmHmsParse(inDate, &datm);

    snprintf(fmtDate, sizeof(fmtDate), "%d%s%d", datm.tm_mday, mthArray[datm.tm_mon - 1].shortDesc, datm.tm_year);

	return(CyclicStrAdd(fmtDate));
}
