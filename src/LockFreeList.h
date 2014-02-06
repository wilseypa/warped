#ifndef LockFreeList_H_
#define LockFreeList_H_

/* Include section */
#include "ListNode.h"

class LockFreeList {

public:
    LockFreeList() {
        head = new ListNode();
        tail = new ListNode();
        head->next = tail;
        listSize = 0;
    }

    ~LockFreeList() {}

    bool push_front( const Event *insEvent ) {
        ListNode *newNode = new ListNode(insEvent);
        ListNode *rightNode = NULL;

        while(true) {
            rightNode = head->next;
            newNode->next = rightNode;
            if( !__sync_bool_compare_and_swap(&(head->bIsMarked), false, true) ) continue;
            if( __sync_bool_compare_and_swap(&(head->next), rightNode, newNode) ) {
                (void) __sync_add_and_fetch(&listSize, 1);
                head->bIsMarked = false;
                return true;
            } else {
                head->bIsMarked = false;
            }
        }
    }

    bool erase( const Event *delEvent ) {
        ListNode *delNode = NULL, *leftNode = NULL;

        while(true) {
            delNode = head->next;
            leftNode = head;
            while( delNode->key != delEvent ) {
                leftNode = delNode;
                delNode = delNode->next;
                if(!delNode) return false;
            }
            if( !__sync_bool_compare_and_swap(&(leftNode->bIsMarked), false, true) ) continue;
            if( !__sync_bool_compare_and_swap(&(delNode->bIsMarked), false, true) ) {
                leftNode->bIsMarked = false;
                continue;
            }
            if( __sync_bool_compare_and_swap(&(leftNode->next), delNode, delNode->next) ) {
                (void) __sync_sub_and_fetch(&listSize, 1);
                leftNode->bIsMarked = false;
                delete delNode;
                return true;
            } else {
                leftNode->bIsMarked = false;
                delNode->bIsMarked = false;
            }
        }
    }

    const Event *pop_front() {
        ListNode *popNode = NULL, *leftNode = NULL;

        while(true) {
            popNode = head->next;
            if(popNode == tail) return NULL;

            if( !__sync_bool_compare_and_swap(&(head->bIsMarked), false, true) ) continue;
            if( !__sync_bool_compare_and_swap(&(popNode->bIsMarked), false, true) ) {
                head->bIsMarked = false;
                continue;
            }
            if( __sync_bool_compare_and_swap(&(head->next), popNode, popNode->next) ) {
                (void) __sync_sub_and_fetch(&listSize, 1);
                leftNode->bIsMarked = false;
                const Event *event = popNode->key;
                delete popNode;
                return event;
            } else {
                head->bIsMarked = false;
                popNode->bIsMarked = false;
            }
        }
    }

    ListNode *end() { return tail; }

    bool empty() { return( (!listSize) ? true : false ); }

    int size() { return listSize; }

    void clear() {
        head->next = tail;
        /* memory loss might need handling */
        listSize = 0;
    }

private:
    ListNode *head, *tail;
    int      listSize;

};

#endif /* LockFreeList_H_ */

