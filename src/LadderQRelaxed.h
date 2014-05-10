#ifndef LadderQueueRelaxed_H_
#define LadderQueueRelaxed_H_

/* Include section */
#include <iostream>
#include <algorithm>
#include <vector>
#include "LockFreeList.h"
#include "Event.h"

/* Macro section */
#define MAX_RUNG_NUM     8  //ref. sec 2.4 of ladderq paper
#define THRESHOLD        50 //ref. sec 2.3 of ladderq paper
#define MAX_BUCKET_NUM   THRESHOLD //ref. sec 2.4 of ladderq paper
#define MIN_BUCKET_WIDTH 1
#define INVALID          0xFFFF
#define TOP_POOL_SIZE    10000

#define RUNG(x,y) (((x)==0) ? (rung0[(y)]) : (rung1_to_n[(x)-1][(y)]))
#define NUM_BUCKETS(x) (((x)==0) ? (numRung0Buckets) : (MAX_BUCKET_NUM))

/* Top class */
class Top {

public:
    inline Top( unsigned int minTS, 
                unsigned int maxTS, 
                unsigned int topStart ) :
        minTS(minTS), maxTS(maxTS), topStart(topStart) {}

    LockFreeList<const Event *> eventList;
    unsigned int minTS;
    unsigned int maxTS;
    unsigned int topStart;
};

/* Ladder Queue class */
class LadderQueueRelaxed {

public:

    /* Default constructor */
    inline LadderQueueRelaxed() {

        unsigned int rungIndex = 0, bucketIndex = 0;

        /* Create Top */
        top = new Top(0,0,0);

        /* Create the Top pool */
        poolCnt = 0;
        for (bucketIndex = 0; bucketIndex < TOP_POOL_SIZE; bucketIndex++) {
            topPool[bucketIndex] = new Top(0,0,0);
        }

        /* Initialize Rung variables */
        nRung = 0;
        isRungLocked = false;
        std::fill_n(bucketWidth, MAX_RUNG_NUM, 0);
        std::fill_n(numBucket, MAX_RUNG_NUM, 0);
        std::fill_n(rStart, MAX_RUNG_NUM, 0);
        std::fill_n(rCur, MAX_RUNG_NUM, 0);

        /* Create buckets for 1st Rung */
        for (bucketIndex = 0; bucketIndex < 2*MAX_BUCKET_NUM; bucketIndex++) {
            LockFreeList<const Event*> *rung_bucket = new LockFreeList<const Event*>();
            rung0.push_back(rung_bucket);
        }
        numRung0Buckets = bucketIndex;

        /* Create buckets for 2nd to 8th Rungs */
        for (rungIndex = 0; rungIndex < MAX_RUNG_NUM-1; rungIndex++) {
            for (bucketIndex = 0; bucketIndex < MAX_BUCKET_NUM; bucketIndex++) {
                LockFreeList<const Event*> *rung_bucket = NULL;
                if( !(rung_bucket = new LockFreeList<const Event*>()) ) {
                    std::cout << "Failed to allocate memory for Rung "
                         << rungIndex+2 << ", Bucket " << bucketIndex+1 << "." << std::endl;
                }
                rung1_to_n[rungIndex][bucketIndex] = rung_bucket;
            }
        }

        /* Create Bottom */
        bottom = new LockFreeList<const Event*>();
    }

    /* Destructor */
    inline ~LadderQueueRelaxed() {
        clear();
    }

    /* Peek at the event with lowest timestamp */
    inline const Event *begin() {

        unsigned int bucketIndex = 0;
        const Event *event = NULL;

        /* Remove from bottom if not empty */
        if( (event = bottom->begin()) ) {
            return event;
        }

        /* Try to acquire rung lock using CAS. If not successful, 
           check if bottom has events. If not, repeat the cycle.  */
        while( !__sync_bool_compare_and_swap(&isRungLocked, false, true) ) {
            if( NULL != (event = bottom->begin()) ) {
                return event;
            }
        }

        /* If bottom has already been filled by some other thread */
        if( NULL != (event = bottom->begin()) ) {
            isRungLocked = false; //release the rung lock
            return event;
        }

        /* If rungs exist, remove from rungs */
        if ((nRung > 0) && (INVALID == (bucketIndex = recurse_rung()))) {
            /* Check whether rungs still exist */
            if (nRung > 0) {
                std::cout << "Received invalid Bucket index." << std::endl;
                isRungLocked = false; //release the rung lock
                return NULL;
            }
        }

        if (nRung > 0) { /* Check required because recurse_rung() can affect nRung value */
            while( (event = RUNG(nRung-1,bucketIndex)->pop_front()) ) {
                bottom->insert(event);
            }

            /* If bucket returned is the last valid rung of the bucket */
            if (numBucket[nRung-1] == bucketIndex+1) {
                do {
                    numBucket[nRung-1] = rStart[nRung-1] =
                        rCur[nRung-1] = bucketWidth[nRung-1] = 0;
                    nRung--;
                } while(nRung && !numBucket[nRung-1]);

            } else {
                while ((++bucketIndex < numBucket[nRung-1]) &&
                        (true == RUNG(nRung-1,bucketIndex)->empty()));
                if (bucketIndex < numBucket[nRung-1]) {
                    rCur[nRung-1] = rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1];
                } else {
                    std::cout << "numBucket handling needs improvement." << std::endl;
                    numBucket[nRung-1] = rStart[nRung-1] =
                        rCur[nRung-1] = bucketWidth[nRung-1] = 0;
                    nRung--;
                    isRungLocked = false; //release the rung lock
                    return NULL;
                }
            }
            isRungLocked = false; //release the rung lock

            return bottom->begin();
        }

        /* Check if top has any events before proceeding further */
        if (top->eventList.empty()) {
            std::cout << "LadderQ is empty." << std::endl;
            isRungLocked = false; //release the rung lock
            return NULL;
        }

        /* Move from top to top of empty ladder */
        /* Note: No need to update rCur[0] since it will be equal to rStart[0] initially. */
        Top *newTop = NULL;
        Top *oldTop = top;
        unsigned int index = __sync_fetch_and_add(&poolCnt, 1);
        if(index < TOP_POOL_SIZE) {
            newTop = topPool[index];
            newTop->topStart = oldTop->maxTS;
        } else {
            newTop = new Top(0, 0, oldTop->maxTS);
        }
        if( !__sync_bool_compare_and_swap(&top, oldTop, newTop) ) ASSERT(false); //condition unlikely

        /* Create the first rung */
        unsigned int numEvents = oldTop->eventList.size();
        if (oldTop->minTS == oldTop->maxTS) {
            bucketWidth[0] = MIN_BUCKET_WIDTH;
        } else {
            bucketWidth[0] = (oldTop->maxTS - oldTop->minTS + numEvents -1) / numEvents;
        }
        rStart[0] = rCur[0] = oldTop->minTS;
        numBucket[0] = 0;
        nRung++;

        /* Create the actual rungs */
        /* Create double of required no of buckets. ref sec 2.4 of ladderq */
        for (bucketIndex = numRung0Buckets; bucketIndex < 2*numEvents; bucketIndex++) {
            LockFreeList<const Event*> *rung_bucket = new LockFreeList<const Event*>();
            rung0.push_back(rung_bucket);
            numRung0Buckets = bucketIndex;
        }

        /* Transfer events from Top to 1st rung of Ladder */
        while( (event = oldTop->eventList.pop_front()) ) {
            ASSERT( event->getReceiveTime().getApproximateIntTime() >= rStart[0] );
            bucketIndex =
                std::min( (unsigned int)(event->getReceiveTime().getApproximateIntTime() -
                              rStart[0]) / bucketWidth[0],
                          NUM_BUCKETS(0)-1 );

            RUNG(0,bucketIndex)->insert(event);

            /* Update the numBucket parameter */
            if (numBucket[0] < bucketIndex+1) {
                numBucket[0] = bucketIndex+1;
            }
        }

        /* Copy events from bucket_k into Bottom */
        if (INVALID == (bucketIndex = recurse_rung())) {
            std::cout << "Received invalid Bucket index." << std::endl;
            isRungLocked = false; //release the rung lock
            return NULL;
        }

        while( (event = RUNG(nRung-1,bucketIndex)->pop_front()) ) {
            bottom->insert(event);
        }

        /* If bucket returned is the last valid rung of the bucket */
        if (numBucket[nRung-1] == bucketIndex+1) {
            numBucket[nRung-1] = rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = 0;
            nRung--;
        } else {
            while ((++bucketIndex < numBucket[nRung-1]) && (true == RUNG(nRung-1,bucketIndex)->empty()));
            if (bucketIndex < numBucket[nRung-1]) {
                rCur[nRung-1] = rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1];
            } else {
                std::cout << "rung 1 numBucket handling needs improvement." << std::endl;
                numBucket[nRung-1] = rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = 0;
                nRung--;
            }
        }
        isRungLocked = false; //release the rung lock

        return bottom->begin();
    }

    /* Purge the entire LadderQ data */
    inline void clear() {

        unsigned int rungIndex = 0, bucketIndex = 0;

        /* Purge Rungs */
        /* Note: Purge rungs first by acquiring the lock so as 
           not to keep operations of other threads consistent  */
        while( !__sync_bool_compare_and_swap(&isRungLocked, false, true) );

        for (rungIndex = 0; rungIndex < MAX_RUNG_NUM; rungIndex++) {
            bucketWidth[rungIndex] = rStart[rungIndex] = rCur[rungIndex] = numBucket[rungIndex] = 0;
            for (bucketIndex = 0; bucketIndex < NUM_BUCKETS(rungIndex); bucketIndex++) {
                RUNG(rungIndex,bucketIndex)->clear();
            }
        }
        nRung = 0;

        /* Purge Top */
        Top *newTop = new Top(0,0,0);
        if( !__sync_bool_compare_and_swap(&top, top, newTop) ) ASSERT(false); //condition unlikely

        isRungLocked = false; //release rung lock after purging Top to prevent event transfer from Top

        /* Purge bottom */
        LockFreeList<const Event*> *newBottom = new LockFreeList<const Event*>();
        if( !__sync_bool_compare_and_swap(&bottom, bottom, newBottom) ) ASSERT(false); //condition unlikely
    }

    /* Dequeue the event with lowest timestamp */
    inline const Event *dequeue() {

        unsigned int bucketIndex = 0;
        const Event *event = NULL;

        /* Remove from bottom if not empty */
        if( (event = bottom->pop_front()) ) {
            return event;
        }

        /* Try to acquire rung lock using CAS. If not successful, 
           check if bottom has events. If not, repeat the cycle.  */
        while( !__sync_bool_compare_and_swap(&isRungLocked, false, true) ) {
            if( NULL != (event = bottom->pop_front()) ) {
                return event;
            }
        }

        /* If bottom has already been filled by some other thread */
        if( NULL != (event = bottom->pop_front()) ) {
            isRungLocked = false; //release the rung lock
            return event;
        }

        /* If rungs exist, remove from rungs */
        if ((nRung > 0) && (INVALID == (bucketIndex = recurse_rung()))) {
            /* Check whether rungs still exist */
            if (nRung > 0) {
                std::cout << "Received invalid Bucket index." << std::endl;
                isRungLocked = false; //release the rung lock
                return NULL;
            }
        }

        if (nRung > 0) { /* Check required because recurse_rung() can affect nRung value */
            while( (event = RUNG(nRung-1,bucketIndex)->pop_front()) ) {
                bottom->insert(event);
            }

            /* If bucket returned is the last valid rung of the bucket */
            if (numBucket[nRung-1] == bucketIndex+1) {
                do {
                    numBucket[nRung-1] = rStart[nRung-1] =
                        rCur[nRung-1] = bucketWidth[nRung-1] = 0;
                    nRung--;
                } while(nRung && !numBucket[nRung-1]);

            } else {
                while ((++bucketIndex < numBucket[nRung-1]) &&
                        (true == RUNG(nRung-1,bucketIndex)->empty()));
                if (bucketIndex < numBucket[nRung-1]) {
                    rCur[nRung-1] = rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1];
                } else {
                    std::cout << "numBucket handling needs improvement." << std::endl;
                    numBucket[nRung-1] = rStart[nRung-1] =
                        rCur[nRung-1] = bucketWidth[nRung-1] = 0;
                    nRung--;
                    isRungLocked = false; //release the rung lock
                    return NULL;
                }
            }
            isRungLocked = false; //release the rung lock

            return bottom->pop_front();
        }

        /* Check if top has any events before proceeding further */
        if (top->eventList.empty()) {
            std::cout << "LadderQ is empty." << std::endl;
            isRungLocked = false; //release the rung lock
            return NULL;
        }

        /* Move from top to top of empty ladder */
        /* Note: No need to update rCur[0] since it will be equal to rStart[0] initially. */
        Top *newTop = NULL;
        Top *oldTop = top;
        unsigned int index = __sync_fetch_and_add(&poolCnt, 1);
        if(index < TOP_POOL_SIZE) {
            newTop = topPool[index];
            newTop->topStart = oldTop->maxTS;
        } else {
            newTop = new Top(0, 0, oldTop->maxTS);
        }
        if( !__sync_bool_compare_and_swap(&top, oldTop, newTop) ) ASSERT(false); //condition unlikely

        /* Create the first rung */
        unsigned int numEvents = oldTop->eventList.size();
        if (oldTop->minTS == oldTop->maxTS) {
            bucketWidth[0] = MIN_BUCKET_WIDTH;
        } else {
            bucketWidth[0] = (oldTop->maxTS - oldTop->minTS + numEvents -1) / numEvents;
        }
        rStart[0] = rCur[0] = oldTop->minTS;
        numBucket[0] = 0;
        nRung++;

        /* Create the actual rungs */
        /* create double of required no of buckets. ref sec 2.4 of ladderq */
        for (bucketIndex = numRung0Buckets; bucketIndex < 2*numEvents; bucketIndex++) {
            LockFreeList<const Event*> *rung_bucket = new LockFreeList<const Event*>();
            rung0.push_back(rung_bucket);
            numRung0Buckets = bucketIndex;
        }

        /* Transfer events from Top to 1st rung of Ladder */
        while( (event = oldTop->eventList.pop_front()) ) {
            ASSERT( event->getReceiveTime().getApproximateIntTime() >= rStart[0] );
            bucketIndex =
                std::min( (unsigned int)(event->getReceiveTime().getApproximateIntTime() -
                              rStart[0]) / bucketWidth[0],
                          NUM_BUCKETS(0)-1 );

            RUNG(0,bucketIndex)->insert(event);

            /* Update the numBucket parameter */
            if (numBucket[0] < bucketIndex+1) {
                numBucket[0] = bucketIndex+1;
            }
        }
        //delete oldTop;

        /* Copy events from bucket_k into Bottom */
        if (INVALID == (bucketIndex = recurse_rung())) {
            std::cout << "Received invalid Bucket index." << std::endl;
            isRungLocked = false; //release the rung lock
            return NULL;
        }

        while( (event = RUNG(nRung-1,bucketIndex)->pop_front()) ) {
            bottom->insert(event);
        }

        /* If bucket returned is the last valid rung of the bucket */
        if (numBucket[nRung-1] == bucketIndex+1) {
            numBucket[nRung-1] = rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = 0;
            nRung--;
        } else {
            while ((++bucketIndex < numBucket[nRung-1]) && (true == RUNG(nRung-1,bucketIndex)->empty()));
            if (bucketIndex < numBucket[nRung-1]) {
                rCur[nRung-1] = rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1];
            } else {
                std::cout << "rung 1 numBucket handling needs improvement." << std::endl;
                numBucket[nRung-1] = rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = 0;
                nRung--;
            }
        }
        isRungLocked = false; //release the rung lock

        return bottom->pop_front();
    }

    /* Check whether the LadderQ has any events or not */
    inline bool empty() {

        return ((0==nRung) & top->eventList.empty() & bottom->empty());
    }

    /* Refers to the end of LadderQ; always returns NULL */
    inline const Event *end() {
        return NULL;
    }

    /* Delete the specified event from LadderQ (if found) */
    inline void erase(const Event* delEvent) {

        unsigned int rungIndex = 0, bucketIndex = 0;

        /* Check whether valid event received */
        if (NULL == delEvent) {
            std::cout << "Invalid event erase request received." << std::endl;
            return;
        }

        /* Check and erase in top, if found */
        if( top->topStart < delEvent->getReceiveTime().getApproximateIntTime() ) {
            top->eventList.erase(delEvent);
            return;
        }

        /* Try to acquire rung lock using CAS. If not successful, keep on trying */
        while( !__sync_bool_compare_and_swap(&isRungLocked, false, true) );

        /* Step through rungs */
        while ((rungIndex < nRung)
                && (delEvent->getReceiveTime().getApproximateIntTime() < rCur[rungIndex])) {
            rungIndex++;
        }

        if (rungIndex < nRung) {  /* found a rung */
            bucketIndex =
                std::min( (unsigned int)(delEvent->getReceiveTime().getApproximateIntTime() -
                               rStart[rungIndex]) / bucketWidth[rungIndex],
                          NUM_BUCKETS(rungIndex)-1 );

            LockFreeList<const Event*> *rung_bucket = RUNG(rungIndex,bucketIndex);
            if( !rung_bucket->empty() ) {
                rung_bucket->erase(delEvent);

                /* If bucket is empty after deletion */
                if( rung_bucket->empty() ) {
                    /* Check whether numBucket needs adjustment */
                    if (numBucket[rungIndex] == bucketIndex+1) {
                        do {
                            if (0 == bucketIndex) {
                                bucketIndex = INVALID;
                                numBucket[rungIndex] = 0;
                                rCur[rungIndex] = rStart[rungIndex];
                                break;
                            }
                            bucketIndex--;
                            rung_bucket = RUNG(rungIndex,bucketIndex);
                        } while (rung_bucket->empty());

                        if (INVALID != bucketIndex) {
                            numBucket[rungIndex] = bucketIndex+1;
                        }
                    }
                }
            }
            isRungLocked = false; //release the rung lock
            return;
        }
        isRungLocked = false; //release the rung lock

        /* Check and erase from bottom, if present */
        if ( !bottom->empty() ) {
            bottom->erase(delEvent);
        }
    }

    /* Inserts the specified event into LadderQ (if already not present) */
    inline const Event *insert(const Event* newEvent) {

        unsigned int rungIndex = 0, bucketIndex = 0;

        /* Check whether valid event received */
        if (NULL == newEvent) {
            std::cout << "Invalid event insertion request received." << std::endl;
            return NULL;
        }

        /* Insert into top, if valid */
        if (newEvent->getReceiveTime().getApproximateIntTime() > top->topStart) {  //deviation from APPENDIX of ladderq
            if( top->eventList.empty() ) {
                if( !__sync_bool_compare_and_swap( &(top->minTS), top->minTS, 
                                                   newEvent->getReceiveTime().getApproximateIntTime() )) {
                    ASSERT(false); //condition unlikely
                }
            } else {
                if (top->minTS > newEvent->getReceiveTime().getApproximateIntTime()) {
                    if( !__sync_bool_compare_and_swap( &top->minTS, top->minTS,
                                                       newEvent->getReceiveTime().getApproximateIntTime() )) {
                        ASSERT(false); //condition unlikely
                    }
                }
            }
            if (top->maxTS < newEvent->getReceiveTime().getApproximateIntTime()) {
                if( !__sync_bool_compare_and_swap( &top->maxTS, top->maxTS,
                                                   newEvent->getReceiveTime().getApproximateIntTime() )) {
                    ASSERT(false); //condition unlikely
                }
            }
            while( !(top->eventList.insert(newEvent)) );
            return newEvent;
        }

        /* Try to acquire rung lock using CAS. If not successful, keep on trying. */
        while( !__sync_bool_compare_and_swap(&isRungLocked, false, true) );

        /* Step through rungs */
        while ((rungIndex < nRung)
                && (newEvent->getReceiveTime().getApproximateIntTime() < rCur[rungIndex])) {
            rungIndex++;
        }

        if (rungIndex < nRung) {  /* found a rung */
            ASSERT( newEvent->getReceiveTime().getApproximateIntTime() >= rStart[rungIndex] );
            bucketIndex = 
                std::min( (unsigned int)(newEvent->getReceiveTime().getApproximateIntTime() -
                              rStart[rungIndex]) / bucketWidth[rungIndex],
                          NUM_BUCKETS(rungIndex)-1 );

            /* Adjust the numBucket and rCur parameters */
            if (numBucket[rungIndex] < bucketIndex+1) {
                numBucket[rungIndex] = bucketIndex+1;
            }
            if (rCur[rungIndex] > rStart[rungIndex] + bucketIndex*bucketWidth[rungIndex]) {
                rCur[rungIndex] = rStart[rungIndex] + bucketIndex*bucketWidth[rungIndex];
            }

            RUNG(rungIndex,bucketIndex)->insert(newEvent);
            isRungLocked = false; //release the rung lock
            return newEvent;
        }
        isRungLocked = false; //release the rung lock

        while( !bottom->insert(newEvent) );
        return newEvent;
    }

private:

    /* Top variables */
    Top          *top;
    Top          *topPool[TOP_POOL_SIZE];
    unsigned int poolCnt;

    /* Rungs */
    std::vector<LockFreeList<const Event*> *> rung0;  //first rung. ref. sec 2.4 of ladderq paper
    LockFreeList<const Event*> *rung1_to_n[MAX_RUNG_NUM-1][MAX_BUCKET_NUM];  //2nd to 8th rungs
    unsigned int numRung0Buckets;
    unsigned int nRung;
    unsigned int bucketWidth[MAX_RUNG_NUM];
    unsigned int numBucket[MAX_RUNG_NUM];
    unsigned int rStart[MAX_RUNG_NUM];
    unsigned int rCur[MAX_RUNG_NUM];
    bool         isRungLocked;

    /* Bottom */
    LockFreeList<const Event*> *bottom;



    /* Create (here implicitly allocate) a new rung */
    inline bool create_new_rung(unsigned int numEvents, unsigned int initStartAndCurVal,
                                bool* isBucketWidthStatic) {

        /* Check the arguements */
        if (NULL == isBucketWidthStatic) {
            std::cout << "Invalid memory address for monitoring change in bucketWidth" << std::endl;
            return false;
        }
        *isBucketWidthStatic = false;
        if (0 == numEvents) {
            std::cout << "Rung creation request comes without presence of events." << std::endl;
            return false;
        }

        /* Check if this is the first rung creation */
        if (0 == nRung) {
            ASSERT(false); //handled in begin() and dequeue() separately

        } else { // When rungs already exist

            /* Check if bucket width has reached the min limit */
            if (MIN_BUCKET_WIDTH >= bucketWidth[nRung-1]) {
                *isBucketWidthStatic = true;
                return false;
            }

            /* Check whether new rungs can be created */
            if (MAX_RUNG_NUM <= nRung) {
                std::cout << "Overflow error for no. of rungs." << std::endl;
                return false;
            }
            nRung++;
            bucketWidth[nRung-1] = (bucketWidth[nRung-2] + numEvents - 1) / numEvents;
            rStart[nRung-1] = rCur[nRung-1] = initStartAndCurVal;
            numBucket[nRung-1] = 0;
            return true;
        }
    }

    /* Recurse rung */
    inline unsigned int recurse_rung() {

        bool isBucketNotFound = false, isBucketWidthStatic = false, isRungNotEmpty = false;
        unsigned int bucketIndex = 0, newBucketIndex = 0;
        const Event *event = NULL;

        /* find_bucket label */
        do {
            isBucketNotFound = false;
            isBucketWidthStatic = false;
            isRungNotEmpty = false;
            bucketIndex = 0;

            if ((0 == nRung) || (MAX_RUNG_NUM < nRung)) {
                if (MAX_RUNG_NUM < nRung) {
                    std::cout << "Invalid number of rungs available for recurse_rung." << std::endl;
                }
                return INVALID;
            }

            rCur[nRung-1] = rStart[nRung-1];

            while ((NUM_BUCKETS(nRung-1) > bucketIndex) &&
                    (true == RUNG(nRung-1,bucketIndex)->empty())) {
                bucketIndex++;
                rCur[nRung-1] += bucketWidth[nRung-1];
            }

            if (NUM_BUCKETS(nRung-1) == bucketIndex) {
                rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = numBucket[nRung-1] = 0;
                nRung--;
                isBucketNotFound = true;

            } else {
                if (THRESHOLD < RUNG(nRung-1,bucketIndex)->size()) {
                    if (false == create_new_rung(RUNG(nRung-1,bucketIndex)->size(),
                                                 rCur[nRung-1], &isBucketWidthStatic)) {
                        if (!isBucketWidthStatic) {
                            std::cout << "Failed to create a new rung." << std::endl;
                            return INVALID;
                        }
                        break; //failed to create a new rung
                    }

                    while( (event = RUNG(nRung-2,bucketIndex)->pop_front()) ) {
                        ASSERT( event->getReceiveTime().getApproximateIntTime() >= rStart[nRung-1] );
                        newBucketIndex = 
                            std::min( (unsigned int) (event->getReceiveTime().getApproximateIntTime() - 
                                          rStart[nRung-1]) / bucketWidth[nRung-1],
                                      NUM_BUCKETS(nRung-1)-1 );

                        RUNG(nRung-1,newBucketIndex)->insert(event);

                        /* Calculate numBucket of new rung */
                        if (numBucket[nRung-1] < newBucketIndex+1) {
                            numBucket[nRung-1] = newBucketIndex+1;
                        }
                    }

                    /* Re-calculate rCur and numBucket of old rung */
                    newBucketIndex = bucketIndex+1;
                    while (newBucketIndex < NUM_BUCKETS(nRung-2)) {
                        if (false == RUNG(nRung-2,newBucketIndex)->empty()) {
                            if (false == isRungNotEmpty) {
                                isRungNotEmpty = true;
                                rCur[nRung-2] = rStart[nRung-2] + newBucketIndex*bucketWidth[nRung-2];
                            }
                            numBucket[nRung-2] = newBucketIndex+1;
                        }
                        newBucketIndex++;
                    }
                    if (false == isRungNotEmpty) {
                        numBucket[nRung-2] = 0;
                        rCur[nRung-2] = rStart[nRung-2];
                    }
                    isBucketNotFound = true;
                }
            }
        } while (isBucketNotFound);

        return bucketIndex;
    }
};

#endif /* LadderQueueRelaxed_H_ */

