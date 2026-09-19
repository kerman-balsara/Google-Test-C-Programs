// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

#include <ctype.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <datmdefs.h>
#include <syssizes.h>
#include <typedefs.h>

#include <datmutils.h>

TEST(DatmHmsNow, IsOk)
{
    string_t datm[SZ_DATETIME_HMS + 1];

    // Act
    strlcpy(datm, DatmNowHms(), sizeof(datm));

    // Assert
    EXPECT_TRUE(isdigit(datm[0]));
    EXPECT_TRUE(isdigit(datm[1]));
    EXPECT_TRUE(isdigit(datm[2]));
    EXPECT_TRUE(isdigit(datm[3]));
    EXPECT_EQ('-', datm[4]);
    EXPECT_TRUE(isdigit(datm[5]));
    EXPECT_TRUE(isdigit(datm[6]));
    EXPECT_EQ('-', datm[7]);
    EXPECT_TRUE(isdigit(datm[8]));
    EXPECT_TRUE(isdigit(datm[9]));
    EXPECT_EQ(' ', datm[10]);
    EXPECT_TRUE(isdigit(datm[11]));
    EXPECT_TRUE(isdigit(datm[12]));
    EXPECT_EQ(':', datm[13]);
    EXPECT_TRUE(isdigit(datm[14]));
    EXPECT_TRUE(isdigit(datm[15]));
    EXPECT_EQ(':', datm[16]);
    EXPECT_TRUE(isdigit(datm[17]));
    EXPECT_TRUE(isdigit(datm[18]));
    EXPECT_EQ(NULL, datm[19]);
}

TEST(DatmHmsNowFilename, IsOk)
{
    string_t datm[SZ_DATETIME_HMS + 1];

    // Act
    strlcpy(datm, DatmNowHmsFilename(), sizeof(datm));

    // Assert
    EXPECT_TRUE(isdigit(datm[0]));
    EXPECT_TRUE(isdigit(datm[1]));
    EXPECT_TRUE(isdigit(datm[2]));
    EXPECT_TRUE(isdigit(datm[3]));
    EXPECT_EQ('-', datm[4]);
    EXPECT_TRUE(isdigit(datm[5]));
    EXPECT_TRUE(isdigit(datm[6]));
    EXPECT_EQ('-', datm[7]);
    EXPECT_TRUE(isdigit(datm[8]));
    EXPECT_TRUE(isdigit(datm[9]));
    EXPECT_EQ('_', datm[10]);
    EXPECT_TRUE(isdigit(datm[11]));
    EXPECT_TRUE(isdigit(datm[12]));
    EXPECT_TRUE(isdigit(datm[13]));
    EXPECT_TRUE(isdigit(datm[14]));
    EXPECT_TRUE(isdigit(datm[15]));
    EXPECT_TRUE(isdigit(datm[16]));
    EXPECT_EQ(NULL, datm[17]);
}

TEST(DatmHmsParse, IsOk)
{
    // Arrange
    string_t passedDate[SZ_DATETIME_HMS + 1];
    strlcpy(passedDate, "2023-12-23 12:25:30", sizeof(passedDate));

    datm_t expDatm;
    expDatm.tm_sec = 30;
    expDatm.tm_min = 25;
    expDatm.tm_hour = 12;
    expDatm.tm_mday = 23;
    expDatm.tm_mon = 12;
    expDatm.tm_year = 2023;
    expDatm.tm_wday = 0;
    expDatm.tm_yday = 0;
    expDatm.tm_isdst = 0;

    datm_t actDatm;

    // Act
    DatmHmsParse(passedDate, &actDatm);

    // Assert
    EXPECT_EQ(0, memcmp(&expDatm, &actDatm, sizeof(datm_t)));
}
