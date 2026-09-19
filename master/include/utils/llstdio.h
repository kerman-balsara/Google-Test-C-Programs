#ifndef LLSTDIO_H
#define LLSTDIO_H

#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE * llfopen(constr_t fileName, constr_t mode);
int llfclose(FILE *fPtr);

FILE * llpopen(constr_t cmd, constr_t type);
int llpclose(FILE *fPtr);

string_t * llfgets(string_t *str, const int len, FILE *fPtr);
int llfputs(constr_t str, FILE *fPtr);
int llfputc(int c, FILE *fPtr);
int llvfprintf(FILE *fPtr, constr_t format, va_list arg);

int llferror(FILE *fPtr);
void llclearerr(FILE *fPtr);

int llunlink(constr_t fileName);

int llstat(constr_t path, struct stat * const pathStatus);

#ifdef __cplusplus
}
#endif

#endif // LLSTDIO_H