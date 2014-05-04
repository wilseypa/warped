#ifndef ListNode_H_
#define ListNode_H_

/* State of node */
typedef enum state_e {
    INS = 0,
    REM,
    DAT,
    INV
} state_t;

/* Node of a lock-free list */
template<class T>
class ListNode {

public:

    ListNode(T keyVal, state_t stateVal): 
                key(keyVal), state(stateVal), next(NULL) {}

    ~ListNode() {}

    T key;
    state_t state;
    ListNode *next;
};

#endif /* ListNode_H_ */

