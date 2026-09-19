// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::HasSubstr;

using ::testing::_;
using ::testing::InSequence;
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

#include <wrappathstatus.h>

TEST(PathExistsDeathTest, Diesstat)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            struct stat pathStatus;
            string_t path[SZ_FULL_PATH_NAME + 1];

            // Debugging option 1:
            // On a terminal window
            // - cd testmock/build
            // - gdb ./mysystest
            // - layout split
            // - set follow-fork-mode child
            // - set detach-on-fork off
            // - set breakpoint in pathstatus.c in function PathExists() on the line that calls llstat().
            // - run
            // - refresh (if screen is jumbled)
            // Debugging option 2:
            // We will need two gdb sessions: one from a terminal window and the other via vscode. The vscode gdb will
            // track the child process. We could use another terminal window instead of vscode but using gdb is easier
            // in vscode.
            // - In vscode, uncomment the 4 lines and rebuild testmock
            // - In a terminal window, from the project root (say ~/projects/GoogleTestC), run gdb testmock/build/mysystest
            //   and type "run". This is the terminal window gdb.
            // - You should see the message
            //   [Detaching after fork from child process <pid>]
            //   Sleeping <pid> ...
            // - In vscode, use the above pid to change the "processId" property in the launch.json name
            //   "testmock (gdb) Attach". Save launch.json and run it. You will need to enter y and specify the
            //   superuser password. This is the vscode gdb.
            // - Set breakpoint in pathstatus.c in function PathExists() on the line that calls llstat().
            // - After the number of sleep seconds has elapsed, the breakpoint in the vscode gdb should be activated.
            // NOTE: Setting breakpoints in this function will not work since it is a macro. Hence we can only debug AFTER this
            // function calls PathExists(). Hence the breakpoint is set in pathstatus.c PathExists().
            // NOTE: 120 seconds should be enough to change the pid in launch.json, run "testmock (gdb) Attach", and set
            // required breakpoints. If the child process completes before this can be done, change 120 to 180.
            // pid_t pid = getpid();
            // printf("Sleeping %d ...\n", pid);
            // sleep(120);
            // printf("Waking up ...\n");

            MockedFnsObj = std::make_shared<MockedFns>();

            InSequence seq;

            int sysErrno = EACCES;
        
            EXPECT_CALL(*MockedFnsObj, llstat)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            PathExists(path, &pathStatus);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_STAT)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}
