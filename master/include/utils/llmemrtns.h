#ifndef LLMEMRTNS_H
#define LLMEMRTNS_H

#include <stddef.h>

// ---------------------
// Internal definitions.
// ---------------------

#ifdef __cplusplus
extern "C" {
#endif

void * llmalloc(const size_t size);
void * llrealloc(void * oldptr, const size_t size);

#ifdef __cplusplus
}
#endif

#endif // LLMEMRTNS_H