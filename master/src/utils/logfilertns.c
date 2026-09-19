#include <errno.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <llunistd.h>
#include <logfilertns.h>
#include <datmutils.h>
#include <fmtstr.h>
#include <sysexit.h>

void LogfileOpen(constr_t LogfileNamePrefix, string_t LogfileName[SZ_FULL_PATH_NAME], int * const LogfileFd)
{
    strlcpy(LogfileName, FmtStr("%s.%s", LogfileNamePrefix, DatmNowHmsFilename()), SZ_FULL_PATH_NAME);

    *LogfileFd = open(LogfileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (*LogfileFd < 0)
        SysExit(__func__, errno, SYSCALL_OPEN);

    // Redirect stderr to log file
    if (dup2(*LogfileFd, STDERR_FILENO) < 0)
        SysExit(__func__, errno, SYSCALL_DUP2);
}

void LogfileClose(int * const LogfileFd)
{
    (void) llclose(*LogfileFd);
    *LogfileFd = -1;
}
