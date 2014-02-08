/* Unsorted lock-free singly-linked list */

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
        cursor = head;
        listSize = 0;
    }

    ~LockFreeList() {
        delete head;
        delete tail;
    }

    /* Function for inserting after curser */
    /* Cursor is marked (atomic). Then new node (pre-marked) is inserted (atomic). 
       New node becomes the cursor. New cursor unmarked, then old one unmarked    */
    bool insert( const Event *insEvent ) {

        ListNode *rightNode = NULL, *leftNode = NULL;
        ListNode *newNode = new ListNode(insEvent);
        newNode->bIsMarked = true;

        while(true) {
            if( !__sync_bool_compare_and_swap(&(cursor->bIsMarked), false, true) ) continue;
            leftNode = cursor;
            rightNode = cursor->next;
            newNode->next = rightNode;
            if( !__sync_bool_compare_and_swap(&(cursor->next), rightNode, newNode) ) {
                cursor->bIsMarked = false;
                continue;
            }
            cursor = newNode;
            cursor->bIsMarked = false;
            leftNode->bIsMarked = false;
            (void) __sync_add_and_fetch(&listSize, 1);
            return true;
        }
    }

    /* Function to erase a specific node */
    /* The list is traversed while marking the node (atomic). If node matches, it 
       is deleted (atomic). If the deleted node is cursor, it is assigned to the 
       left node (atomic).                                                         */
    bool erase( const Event *delEvent ) {
        ListNode *delNode = head, *leftNode = NULL; 

        while(delNode->next != tail) {
            leftNode = delNode;
            delNode = leftNode->next;
            if( !__sync_bool_compare_and_swap(&(delNode->bIsMarked), false, true) ) return false; // might be changed to while
            (void) __sync_bool_compare_and_swap(&(leftNode->bIsMarked), true, false);
            if(tail == delNode) { // list empty. delNode is tail.
                tail->bIsMarked = false;
                return false;
            }
            if(delNode->key != delEvent) continue;
            if( !__sync_bool_compare_and_swap(&(leftNode->next), delNode, delNode->next) ) {
                delNode->bIsMarked = false;
                return false;
            }
            (void) __sync_bool_compare_and_swap(&cursor, delNode, leftNode);
            (void) __sync_sub_and_fetch(&listSize, 1);
            delete delNode;
            return true;
        }
        return false;
    }

    /* Function for popping from front of list */
    /* The node to be popped is marked (atomic). Return NULL if that node is the tail. 
       Delete that node (atomic). If the popped node is curser, move curser to head (atomic). */
    const Event *pop_front() {
        ListNode *popNode = NULL;

        while(true) {
            popNode = head->next;
            if( !__sync_bool_compare_and_swap(&(popNode->bIsMarked), false, true) ) continue;
            if(tail->bIsMarked) { // list empty. popNode is tail.
                tail->bIsMarked = false;
                return NULL;
            }
            if( !__sync_bool_compare_and_swap(&(head->next), popNode, popNode->next) ) {
                popNode->bIsMarked = false;
                continue;
            }
            (void) __sync_bool_compare_and_swap(&cursor, popNode, head);
            (void) __sync_sub_and_fetch(&listSize, 1);
            const Event *event = popNode->key;
            delete popNode;
            return event;
        }
    }

    /* Funtion to read the first available key */
    const Event *begin() {
        return (head->next)->key;
    }

    /* Function to check for empty list */
    bool empty() { return( (!listSize) ? true : false ); }

    /* Function to check list size */
    int size() { return listSize; }

    /* Function to clear the list */
    /* Note: Function leaks memory */
    void clear() {
        cursor = head;
        head->next = tail;
        listSize = 0;
    }

private:

    ListNode    *head;
    ListNode    *tail;
    ListNode    *cursor;
    int         listSize;

};

#endif /* LockFreeList_H_ */

