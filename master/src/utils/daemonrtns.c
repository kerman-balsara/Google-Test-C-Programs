#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <errcodes.h>

#include <daemonrtns.h>
#include <sysexit.h>

// Daemons should not have controlling terminals. If a daemon has a controlling terminal, it can
// receive signals from it that might cause it to halt or exit unexpectedly. The call to setsid()
// and the second fork() achieves this.
// setsid() does the following:
// - Creates a new session and makes the child process session leader
//   (i.e., its session ID is made the same as its process ID).
// - Makes the child process group leader (i.e., its process group ID is
//   made the same as its process ID).
// This child process can still acquire a controlling terminal; hence we do
// another fork after which we do not call setsid();

// Remarks                      PID    PGID    SESSID
// Main before first fork()     12939  12939   4916     Process group leader; cannot call setsid()
// After first fork():
// Terminate parent process 12939
// Child 1 after first fork     12940  12939   4916     Not process group leader so can call setsid()
//                                                      and become process group leader and session leader
// Child 1 after setsid()       12940  12940   12940    Get rid of controlling terminal (4916)
//                                                      Child process 12940 can still acquire controlling terminal
//                                                      so do a second fork()
// After second fork():
// Terminate parent process 12940
// Child 2 after second fork    12941  12940   12940    Prevent process from acquiring controlling terminal
void DaemonizeServer(void)
{
    // We use a double fork() to daemonise the process.
    pid_t pid = fork();
    if (pid < 0)
        SysExit(__func__, errno, SYSCALL_FORK);

    if (pid)
        exit(0); // Terminate first parent

    // First child process
    pid = setsid();
    if (pid < 0) 
        SysExit(__func__, errno, SYSCALL_SETSID);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGHUP, &sa, NULL))
        SysExit(__func__, errno, SYSCALL_SIGACTION);

    pid = fork();
    if (pid < 0)
        SysExit(__func__, errno, SYSCALL_FORK);

    if (pid)
        exit(0); // Terminate second parent
}