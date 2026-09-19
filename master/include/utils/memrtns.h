#ifndef MEMRTNS_H
#define MEMRTNS_H

#include <stddef.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void * MemAlloc(const string_t * const caller, const size_t size);
// Passed ptr is address of pointer
void MemRealloc(const string_t * const caller, void * ptr, const size_t size);
// Passed ptr is address of pointer
void MemFree(const string_t * const caller, void * const ptr);

#ifdef __cplusplus
}
#endif

#endif // MEMRTNS_H