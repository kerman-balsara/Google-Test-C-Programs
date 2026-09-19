// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <errno.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>

#include <sysexit.h>

using ::testing::AllOf;
using ::testing::HasSubstr;

TEST(SysExitDeathTest, DiesOverflow)
{
    // Arrange
    int sysErrNo = EPERM;
    int errCode = STR_SIZE_EXCEEDED;
    std::string errCodeDesc = "STR_SIZE_EXCEEDED";

    // Act & Assert
    EXPECT_EXIT(SysExit(__func__, sysErrNo, errCode), ::testing::ExitedWithCode(1),
                AllOf(HasSubstr("code: " + std::to_string(errCode)),
                      HasSubstr("desc: " + errCodeDesc),
                      HasSubstr("errno: " + std::to_string(EPERM) + " [" + strerror(EPERM) + "]")));
}
