#ifndef ListNode_H_
#define ListNode_H_

/* Include section */
#include "Event.h"

/* Node of a lock-free list */
class ListNode {

public:
    ListNode(): key(NULL), next(NULL), bIsMarked(false) {}

    ListNode(const Event *keyVal): key(keyVal), next(NULL), bIsMarked(false) {}

    ~ListNode() {}

    const Event *key;
    ListNode *next;
    bool bIsMarked;
};

#endif /* ListNode_H_ */

