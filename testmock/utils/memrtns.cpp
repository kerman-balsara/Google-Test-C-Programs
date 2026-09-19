// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::HasSubstr;

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetErrnoAndReturn;

#include <sys/stat.h>
#include <errno.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <wrapmemrtns.h>

TEST(MemAllocDeathTest, Diesmalloc)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            int size = 10;

            MockedFnsObj = std::make_shared<MockedFns>();

            InSequence seq;

            int sysErrno = ENOMEM;

            EXPECT_CALL(*MockedFnsObj, llmalloc)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, nullptr));

            // Act
            MemAlloc("memallocDeathTest", size);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_MALLOC)),
              HasSubstr("errno: " + std::to_string(ENOMEM) + " [" + strerror(ENOMEM) + "]")));
 
}
TEST(MemReallocDeathTest, Diesrealloc)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            int size = 10;
            char * ptr = NULL;

            MockedFnsObj = std::make_shared<MockedFns>();

            InSequence seq;

            int sysErrno = ENOMEM;
        
            EXPECT_CALL(*MockedFnsObj, llrealloc)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, nullptr));

            // Act
            MemRealloc("memreallocDeathTest", &ptr, size);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_REALLOC)),
              HasSubstr("errno: " + std::to_string(ENOMEM) + " [" + strerror(ENOMEM) + "]")));
 
}
