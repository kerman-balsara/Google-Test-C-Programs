// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <datmdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <datmutils.h>
#include <fmtstr.h>

static string_t * datmNowHms(const bool_t isFilename);

// --------------------------------------------------------------------
// Return current date and time (SZ_DATETIME_HMS format YYYY-MM-DD HH:MM:SS).
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/datmutils.cpp
// --------------------------------------------------------------------

string_t * DatmNowHms(void)
{
    return(datmNowHms(0));
}

// --------------------------------------------------------------------
// Return current date and time (SZ_DATETIME_HMS_FILENAME format YYYY-MM-DD_HHMMSS).
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/datmutils.cpp
// --------------------------------------------------------------------

string_t * DatmNowHmsFilename(void)
{
    return(datmNowHms(1));
}

static string_t * datmNowHms(const bool_t isFilename)
{
    struct tm	   tm;
    struct timeval timeval;

    gettimeofday(&timeval, NULL);
    (void) localtime_r(&timeval.tv_sec, &tm);

    // Choose higher of SZ_DATETIME_HMS (19) and SZ_DATETIME_HMS_FILENAME (17)
    char datm[SZ_DATETIME_HMS + 1];

    snprintf(datm, SZ_DATETIME_HMS + 1, "%04hd-%02hd-%02hd%c%02hd%s%02hd%s%02hd",
					tm.tm_year + 1900,
					tm.tm_mon + 1,
					tm.tm_mday,
                    isFilename ? '_' : ' ',
					tm.tm_hour,
                    isFilename ? "" : ":",
					tm.tm_min,
                    isFilename ? "" : ":",
					tm.tm_sec);

    return(FmtStr("%s", datm));                  
}

// --------------------------------------------------------------------
// Parse passed date into struct tm and return tm.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/datmutils.cpp
// --------------------------------------------------------------------

void DatmHmsParse(const string_t inDate[SZ_DATETIME_HMS + 1], datm_t * const datm)
{
    string_t wkDate[SZ_DATETIME_HMS + 1];
    strlcpy(wkDate, inDate, sizeof(wkDate));

    int * parsedDatm[6] = { &datm->tm_year, &datm->tm_mon, &datm->tm_mday,
                            &datm->tm_hour, &datm->tm_min, &datm->tm_sec};
    datm->tm_wday = 0;
    datm->tm_yday = 0;
    datm->tm_isdst = 0;

    const char separator[] = { '-', '-', ' ', ':', ':', '\0'};

    string_t * startPtr = wkDate;
    const string_t * endPtr = &wkDate[strlen(wkDate)];
    string_t * sepPtr;

    for (int i = 0; startPtr < endPtr; startPtr++, i++)
    {
        sepPtr = strchr(startPtr, separator[i]);
        *sepPtr = '\0';
        *parsedDatm[i] = atoi(startPtr);
        startPtr = sepPtr;
   }
}
