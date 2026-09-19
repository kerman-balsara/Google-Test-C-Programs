// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

// ---------------------
// Internal definitions.
// ---------------------

#include <errmsgrtns.h>

#include <receipt.h>

TEST(ReceiptMain, IsOk)
{
    // Arrange
    errmsg_t errmsg;

    // Act
    int ret = ReceiptMain(2, 4, "RECEIPT", &errmsg);

    // Assert
    ASSERT_EQ(ret, 0) << " ret:[" << ret << "] code:[" << errmsg.code << "] message:[" << errmsg.message << "]";
}