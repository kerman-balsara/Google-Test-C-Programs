#ifndef LNKLSTRTNS_H
#define LNKLSTRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

// Passed curr is a pointer, head and tail are addresses of pointers
void   LnkLstAppend(const void * const curr,
                          void * head,
        		          void * tail);
// Passed head, curr and tail are addresses of pointers
void   LnkLstRemove(constr_t   caller,
                        void * head,
                        void * curr,
                        void * tail);
// Passed head is a pointer
void * LnkLstPrev(const void * const head,
		          const void * const curr);
// Passed head is a pointer
void * LnkLstTail(const void * const head);
// Passed head is a pointer
int    LnkLstCount(const void * const head);
// Passed head is address of pointer
void   LnkLstFree(const string_t * const caller,
                        void     * head);

#ifdef __cplusplus
}
#endif

#endif // LNKLSTRTNS_H