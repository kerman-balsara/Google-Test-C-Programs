#ifndef FILERTNS_H
#define FILERTNS_H

// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void FileOpen(FILE **stream, constr_t fileName, constr_t modes);
void FileWriteStr(FILE *stream, const string_t * const str);
void FileWriteStrF(FILE *stream, const string_t * const format, ...);
void FileClose(FILE **stream);
void FileDelete(constr_t fileName);

#ifdef __cplusplus
}
#endif

#endif // FILERTNS_H