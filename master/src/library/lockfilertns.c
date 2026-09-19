#include <errno.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errcodes.h>
#include <typedefs.h>

#include <filertns.h>
#include <llunistd.h>
#include <sysexit.h>

void LockfileCheck(constr_t LockfileName)
{
    int lockFileFd = open(LockfileName, O_EXCL | O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (lockFileFd < 0)
    {
        fprintf(stderr, "Server is already running\n");
        fprintf(stderr, "%s exists\n", LockfileName);
        exit(1);
    }
    if (llclose(lockFileFd))
        SysExit(__func__, errno, SYSCALL_CLOSE);
}

void LockfileWritePid(constr_t LockfileName, const pid_t pid)
{
    FILE *lockFileFptr;
    FileOpen(&lockFileFptr, LockfileName, "w");
    FileWriteStrF(lockFileFptr, "%d", pid);
    FileClose(&lockFileFptr);
}

void LockfileDelete(constr_t LockfileName)
{
    (void) unlink(LockfileName);
}
