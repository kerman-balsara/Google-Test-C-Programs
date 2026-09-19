#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <loglvldefs.h>
#include <logpkgdefs.h>
#include <errcodes.h>
#include <typedefs.h>

#include <llmemrtns.h>
#include <logpkgrtns.h>
#include <logrtns.h>
#include <memrtns.h>
#include <sysexit.h>

// --------------------------------------------------------------------
// Allocate memory.
//
// Return:
// Pointer to allocated memory
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/memrtns.cpp
// --------------------------------------------------------------------

void * MemAlloc(const string_t * const caller, const size_t size)
{
    void * ptr = llmalloc(size);

    if (ptr == NULL)
    {
        int sysErrNo = errno;
    	LogErrorF("%s:%s malloc ptr: NULL (%zu bytes)", caller, __func__, size);
        SysExit(__func__, sysErrNo, SYSCALL_MALLOC);
    }
    else
    {
    	LogPkgTraceF(LOGPKG_MEMORY, "%s malloc ptr:%p (%zu bytes)", caller, ptr, size);
    	memset(ptr, 0, size);
    }

    return(ptr);
}

// --------------------------------------------------------------------
// Reallocate memory.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/memrtns.cpp
// --------------------------------------------------------------------

void MemRealloc(const string_t * const caller, void * ptr, const size_t size)
{
    void **p = (void **) ptr;

    void * newptr = llrealloc(*p, size);

    if (newptr == NULL)
    {
        int sysErrNo = errno;
        MemFree(caller, ptr);
    	LogErrorF("%s:%s realloc ptr: NULL (%zu bytes)", caller, __func__, size);
        SysExit(__func__, sysErrNo, SYSCALL_REALLOC);
    }
    else
    {
    	LogPkgTraceF(LOGPKG_MEMORY, "%s realloc ptr:%p (%zu bytes)", caller, newptr, size);
    }

    *p = newptr;
}

// --------------------------------------------------------------------
// Free memory.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/memrtns.cpp
// --------------------------------------------------------------------

void MemFree(const string_t * const caller, void * const ptr)
{
    void **p = (void **) ptr;

    if (*p)
    {
    	LogPkgTraceF(LOGPKG_MEMORY, "%s free ptr:%p", caller, *p);
    	free(*p);
    	*p = NULL;
    }
}

