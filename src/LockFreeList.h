/* Unsorted lock-free singly-linked list */
/* Please refer to Sec. 3 of "Practical Non-blocking Unordered Lists" by Zhang et al. */

#ifndef LockFreeList_H_
#define LockFreeList_H_

/* Include section */
#include "ListNode.h"

class LockFreeList {

public:

    LockFreeList(): head(NULL), listSize(0) {}

    ~LockFreeList() {}

    bool insert( const Event *k ) {
        if(!k) return false;
        ListNode *h = new ListNode(k, INS);
        enlist(h);
        bool b = helpInsert(h, k);
        if(!__sync_bool_compare_and_swap( &(h->state), INS, (b ? DAT:INV) )) {
            helpRemove(h,k);
            h->state = INV;
        }
        (void) __sync_add_and_fetch(&listSize, (b ? 1:0));
        return b;
    }

    bool erase( const Event *k ) {
        if(!k) return false;
        ListNode *h = new ListNode(k, REM);
        enlist(h);
        bool b = helpRemove(h, k);
        h->state = INV;
        (void) __sync_sub_and_fetch(&listSize, (b ? 1:0));
        return b;
    }

    const Event *pop_front() {
        const Event *k = begin();
        if( !erase(k) ) return NULL;
        return k;
    }

    const Event *begin() {
        ListNode *curr = head;
        while( curr != NULL ) {
            state_t s = curr->state;
            if( s == DAT) break;
            curr = curr->next;
        }
        return (curr ? curr->key : NULL );
    }

    bool empty() {
        return( (listSize) ? false : true );
    }

    int size() {
        return listSize;
    }

    void clear() {
        head = NULL;
        listSize = 0;
    }

private:

    ListNode *head;
    int      listSize;

    void enlist( ListNode *h ) {
        ListNode *old = NULL;
        while(true) {
            old = head;
            h->next = old;
            if(__sync_bool_compare_and_swap(&head, old, h)) return;
        }
    }

    bool helpInsert( ListNode *h, const Event *k ) {
        ListNode *pred = h;
        ListNode *curr = pred->next;

        while( curr != NULL ) {
            state_t s = curr->state;
            if( s == INV ) {
                ListNode *succ = curr->next;
                pred->next = succ;
                curr = succ;
            } else if( curr->key != k ) {
                pred = curr;
                curr = curr->next;
            } else if( s == REM ) {
                return true;
            } else if( (s == INS) || (s == DAT) ) {
                return false;
            }
        }
        return true;
    }

    bool helpRemove( ListNode *h, const Event *k ) {
        ListNode *pred = h;
        ListNode *curr = pred->next;

        while( curr != NULL ) {
            state_t s = curr->state;
            if( s == INV ) {
                ListNode *succ = curr->next;
                pred->next = succ;
                curr = succ;
            } else if( curr->key != k ) {
                pred = curr;
                curr = curr->next;
            } else if( s == REM ) {
                return false;
            } else if( s == INS ) {
                if(__sync_bool_compare_and_swap( &(curr->state), INS, REM)) return true;
            } else if( s == DAT ) {
                curr->state = INV;
                return true;
            }
        }
        return false;
    }

};

#endif /* LockFreeList_H_ */

