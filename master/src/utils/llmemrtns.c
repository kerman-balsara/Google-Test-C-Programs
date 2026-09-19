// --------------------------------------------------------------------
// Wrappers to allow mocks.
// --------------------------------------------------------------------

#include <stddef.h>
#include <stdlib.h>

#include <llmemrtns.h>

void * llmalloc(const size_t size)
{
    return(malloc(size));
}

void * llrealloc(void * oldptr, const size_t size)
{
    return(realloc(oldptr, size));
}