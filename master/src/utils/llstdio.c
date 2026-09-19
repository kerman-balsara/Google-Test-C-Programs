// --------------------------------------------------------------------
// Wrappers to allow mocks.
// --------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include <typedefs.h>

#include <llstdio.h>

FILE * llfopen(constr_t fileName, constr_t mode)
{
    return(fopen(fileName, mode));
}

int llfclose(FILE * fPtr)
{
    return(fclose(fPtr));
}

FILE * llpopen(constr_t cmd, constr_t type)
{
    return(popen(cmd, type));
}

int llpclose(FILE * fPtr)
{
    return(pclose(fPtr));
}

string_t * llfgets(string_t * str, const int len, FILE * fPtr)
{
    return(fgets(str, len, fPtr));
}

int llfputs(constr_t str, FILE *fPtr)
{
    return(fputs(str, fPtr));
}

int llfputc(int c, FILE *fPtr)
{
    return(fputc(c, fPtr));
}

int llvfprintf(FILE *fPtr, constr_t format, va_list arg)
{
    return(vfprintf(fPtr,format, arg));
}

int llferror(FILE * fPtr)
{
    return(ferror(fPtr));
}

void llclearerr(FILE * fPtr)
{
    return(clearerr(fPtr));
}

int llunlink(constr_t fileName)
{
    return(unlink(fileName));
}

int llstat(constr_t path, struct stat * const pathStatus)
{
    return(stat(path, pathStatus));
}
