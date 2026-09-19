// ---------------------
// External definitions.
// ---------------------

#include <errno.h>

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <typedefs.h>

#include <filertns.h>
#include <llstdio.h>
#include <logrtns.h>
#include <sysexit.h>

// --------------------------------------------------------------------
// Do required file i/o.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/testfake/misc/fakedfns.cpp
// file:///home/kerman/projects/GoogleTestC/test/library/filertns.cpp
// file:///home/kerman/projects/GoogleTestC/testmock/library/filertns.cpp
// --------------------------------------------------------------------

void FileOpen(FILE **stream, constr_t fileName, constr_t modes)
{
    *stream = llfopen(fileName, modes);
    if (*stream == NULL)
        SysExit(__func__, errno, SYSCALL_FOPEN);
}

void FileWriteStr(FILE *stream, const string_t * const str)
{
    int ret;
    int errCode = 0;
    if (str)
    {
        errCode = SYSCALL_FPUTS;
        ret = llfputs(str, stream);
    }
    else
    {
        errCode = SYSCALL_FPUTC;
        ret = llfputc('\n', stream);

    }
    if (ret < 0)
    {
        LogErrorF("Error writing [%s]", str ? str : "new line");
        SysExit(__func__, 0, errCode);
    }
}

void FileWriteStrF(FILE *stream, const string_t * const format, ...)
{
    va_list arg;

    va_start(arg, format);
    int ret = llvfprintf(stream, format, arg);
    va_end(arg);

    if (ret < 0)
        SysExit(__func__, 0, SYSCALL_VFPRINTF);
}

void FileClose(FILE **stream)
{
    int ret = llfclose(*stream);
    *stream = NULL;
    if (ret)
        SysExit(__func__, errno, SYSCALL_FCLOSE);
}

void FileDelete(constr_t fileName)
{
    int ret = llunlink(fileName);
    if (ret != 0 &&
        errno != ENOENT)
        SysExit(__func__, errno, SYSCALL_UNLINK);
}
