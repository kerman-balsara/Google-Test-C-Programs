#include <stddef.h>

#include <lnklstdefs.h>
#include <typedefs.h>

#include <lnklstrtns.h>
#include <logrtns.h>
#include <memrtns.h>

// --------------------------------------------------------------------
// Append curr element at end of list. Head amd tail of list is updated.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/lnklstrtns.cpp
// --------------------------------------------------------------------

void LnkLstAppend(const void * const curr,
                        void * head,
        		        void * tail)
{
    lnklst_t  * currPtr = (lnklst_t *) curr;
    lnklst_t ** headPtr	= (lnklst_t **) head;
    lnklst_t ** tailPtr = (lnklst_t **) tail;

    if (*headPtr)
    {
	    (*tailPtr)->next = currPtr;
    }
    else
    {
	    *headPtr = currPtr;
    }

    *tailPtr = currPtr;
}

// --------------------------------------------------------------------
// Remove curr element from list. Head and tail of list is updated.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/lnklstrtns.cpp
// --------------------------------------------------------------------

void LnkLstRemove(constr_t   caller,
                      void * head,
                      void * curr,
                      void * tail)
{
    lnklst_t ** currPtr = (lnklst_t **) curr;
    lnklst_t ** headPtr	= (lnklst_t **) head;

    lnklst_t *  prevPtr = LnkLstPrev(headPtr, *currPtr);

    prevPtr->next = (*currPtr)->next;

	MemFree(caller, currPtr);

    *(lnklst_t **) tail = (prevPtr == head && prevPtr->next == NULL) ? NULL : LnkLstTail(head);
    // This enables the next iteration to get the next element
    *currPtr = prevPtr;
}

// --------------------------------------------------------------------
// Return element prior to curr element from the list.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/lnklstrtns.cpp
// --------------------------------------------------------------------

void * LnkLstPrev(const void * const head,
		          const void * const curr)
{
    lnklst_t * ptr;
    lnklst_t * prevPtr = NULL;

    for (ptr = (lnklst_t *) head; ptr && ptr != curr; ptr = ptr->next)
	    prevPtr = ptr;

    return(ptr ? prevPtr : NULL);
}

// --------------------------------------------------------------------
// Return tail element of list.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/lnklstrtns.cpp
// --------------------------------------------------------------------

void * LnkLstTail(const void * const head)
{
    lnklst_t * ptr;
    lnklst_t * prevPtr = NULL;

    for (ptr = (lnklst_t *) head; ptr; ptr = ptr->next)
	    prevPtr = ptr;

    return(prevPtr);
}

// --------------------------------------------------------------------
// Return number of elements in the list.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/lnklstrtns.cpp
// --------------------------------------------------------------------

int LnkLstCount(const void * const head)
{
    lnklst_t * ptr;

    int i = 0;

    for (ptr = (lnklst_t *) head; ptr; ptr = ptr->next)
        i++;

    return(i);
}

// --------------------------------------------------------------------
// Free the list. As each element is freed, adjust head in case the
// list is reprocessed. 
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/lnklstrtns.cpp
// --------------------------------------------------------------------

void LnkLstFree(const string_t * const caller,
                      void     * head)
{
    lnklst_t *p = * (lnklst_t **) head;

    if (!p) return;

    lnklst_t *next;
    while (p)
    {
    	next = p->next;

        *(lnklst_t **) head = next;

        MemFree(caller, &p);

        p = next;
    }

    *(lnklst_t **) head = NULL;
}

