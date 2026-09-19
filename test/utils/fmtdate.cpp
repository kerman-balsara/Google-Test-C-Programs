// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <fmtdate.h>

TEST(FmtDateDDMonYYYY, IsOk)
{
    // Arrange
    string_t passedDate[SZ_DATETIME_HMS + 1];
    strlcpy(passedDate, "2023-12-23 12:25:30", sizeof(passedDate));

    string_t expFmtDate[9 + 1];
    strlcpy(expFmtDate, "23Dec2023", sizeof(expFmtDate));

    string_t actFmtDate[9 + 1];
    
    // Act
    strlcpy(actFmtDate, FmtDateDDMonYYYY(passedDate), sizeof(actFmtDate));

    // Assert
    EXPECT_STREQ(expFmtDate, actFmtDate);
}
