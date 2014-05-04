/* Unsorted lock-free singly-linked list */
/* Please refer to Sec. 3 of "Practical Non-blocking Unordered Lists" by Zhang et al. */

#ifndef LockFreeList_H_
#define LockFreeList_H_

/* Include section */
#include "ListNode.h"

template <class T>
class LockFreeList {

public:

    LockFreeList(): head(NULL), listSize(0) {}

    ~LockFreeList() {}

    bool insert( T k ) {
        if(!k) return false;
        ListNode<T> *h = new ListNode<T>(k, INS);
        enlist(h);
        bool b = helpInsert(h, k);
        if(!__sync_bool_compare_and_swap( &(h->state), INS, (b ? DAT:INV) )) {
            helpRemove(h,k);
            h->state = INV;
        }
        (void) __sync_add_and_fetch(&listSize, (b ? 1:0));
        return b;
    }

    bool erase( T k ) {
        if(!k) return false;
        ListNode<T> *h = new ListNode<T>(k, REM);
        enlist(h);
        bool b = helpRemove(h, k);
        h->state = INV;
        (void) __sync_sub_and_fetch(&listSize, (b ? 1:0));
        return b;
    }

    T pop_front() {
        while(listSize) {
            T k = begin();
            if( erase(k) ) return k;
        }
        return NULL;
    }

    T begin() {
        ListNode<T> *curr = head;
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

    ListNode<T> *head;
    int         listSize;

    void enlist( ListNode<T> *h ) {
        ListNode<T> *old = NULL;
        while(true) {
            old = head;
            h->next = old;
            if(__sync_bool_compare_and_swap(&head, old, h)) return;
        }
    }

    bool helpInsert( ListNode<T> *h, T k ) {
        ListNode<T> *pred = h;
        ListNode<T> *curr = pred->next;

        while( curr != NULL ) {
            state_t s = curr->state;
            if( s == INV ) {
                ListNode<T> *succ = curr->next;
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

    bool helpRemove( ListNode<T> *h, T k ) {
        ListNode<T> *pred = h;
        ListNode<T> *curr = pred->next;

        while( curr != NULL ) {
            state_t s = curr->state;
            if( s == INV ) {
                ListNode<T> *succ = curr->next;
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

