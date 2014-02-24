#ifndef ListNode_H_
#define ListNode_H_

/* Include section */
#include "Event.h"

/* State of node */
typedef enum state_e {
    INS = 0,
    REM,
    DAT,
    INV
} state_t;

/* Node of a lock-free list */
class ListNode {

public:

    ListNode(const Event *keyVal, state_t stateVal): 
                key(keyVal), state(stateVal), next(NULL) {}

    ~ListNode() {}

    const Event *key;
    state_t state;
    ListNode *next;
};

#endif /* ListNode_H_ */

