// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <cyclicstr.h>
#include <fmtamt.h>

// --------------------------------------------------------------------
// Format passed amount up to passed places.
//
// Return:
// string - formatted amount (uses cyclic string so use immediately)
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/fmtamt.cpp
// --------------------------------------------------------------------

string_t * FmtAmt(const int64_t amt, const int places)
{
	string_t fmtAmt[SZ_AMT_STR];
    string_t * str = fmtAmt;

    if (amt < 0) *str++ = '-';

	sprintf(str, "%lld", llabs(amt));

    if (places > 0)
	{
		// For amt 5, places 2, str is 005
		sprintf(str, "%.*lld", (int) MAX(places + 1, strlen(str)), llabs(amt));
		// Make space for decimal point and position to implied decimal point
		strcat(str, " ");
		str += strlen(str) - places - 1;
		memmove(str + 1, str, places);
		*str = '.';
	}

	return(CyclicStrAdd(fmtAmt));
}
