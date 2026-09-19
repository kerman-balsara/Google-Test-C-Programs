
// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// ---------------------
// External definitions.
// ---------------------

#include <hashpwd.h>

TEST(ValidatePwd, IsOk)
{
    // Arrange
    string_t setting[100];
    strcpy(setting, "$y$jET$viVXKyFse4GI5Ki6G3He91$MtSDDCpIBADoctnd41DVq54tU.V8kP258uu59mpaWC5");


    // Act
    int ret = ValidatePwd("kb", setting);

    // Assert    
    EXPECT_EQ(0, ret);
}
