
#ifndef LadderQueueRelaxed_H_
#define LadderQueueRelaxed_H_

/* Include section */
#include <iostream>
#include <vector>
#include "LockFreeList.h"
#include "EventId.h"

/* Macro section */
#define MAX_RUNG_NUM     8  //ref. sec 2.4 of ladderq paper
#define THRESHOLD        50 //ref. sec 2.3 of ladderq paper
#define MAX_BUCKET_NUM   THRESHOLD //ref. sec 2.4 of ladderq paper
#define MIN_BUCKET_WIDTH 1
#define INVALID          0xFFFF

#define RUNG(x,y) (((x)==0) ? (rung0[(y)]) : (rung1_to_n[(x)-1][(y)]))
#define NUM_BUCKETS(x) (((x)==0) ? (numRung0Buckets) : (MAX_BUCKET_NUM))

/* Ladder Queue class */
class LadderQueueRelaxed {

public:

    /* Default constructor */
    inline LadderQueueRelaxed() {

        maxTS = minTS = topStart = nRung = 0;
        numRung0Buckets = 0;
        std::fill_n(bucketWidth, MAX_RUNG_NUM, 0);
        std::fill_n(numBucket, MAX_RUNG_NUM, 0);
        std::fill_n(rStart, MAX_RUNG_NUM, 0);
        std::fill_n(rCur, MAX_RUNG_NUM, 0);

        /* Create buckets for 2nd to 8th rungs */
        unsigned int rungIndex = 0, bucketIndex = 0;
        for (rungIndex = 0; rungIndex < MAX_RUNG_NUM-1; rungIndex++) {
            for (bucketIndex = 0; bucketIndex < MAX_BUCKET_NUM; bucketIndex++) {
                rung_bucket = NULL;
                if (!(rung_bucket = new LockFreeList())) {
                    std::cout << "Failed to allocate memory for Rung "
                         << rungIndex+2 << ", Bucket " << bucketIndex+1 << "." << std::endl;
                }
                rung1_to_n[rungIndex][bucketIndex] = rung_bucket;
            }
        }
    }

    /* Destructor */
    inline ~LadderQueueRelaxed() {
        clear();
    }

    /* Peek at the available event */
    inline const Event* begin( bool isDequeueReq ) {

        unsigned int bucketIndex = 0;
        const Event *event = NULL;
        bool isBucketWidthStatic = false;
        std::vector<const Event*> topResVec;

        /* Remove from bottom if not empty */
        if(!isDequeueReq) {
            event = bottom.begin();
        } else {
            event = bottom.pop_front();
        }
        if(event) {
            return event;
        }

        /* If rungs exist, remove from rungs */
        if ((nRung > 0) && (INVALID == (bucketIndex = recurse_rung()))) {
            /* Check whether rungs still exist */
            if (nRung > 0) {
                std::cout << "Received invalid Bucket index." << std::endl;
                return NULL;
            }
        }

        if (nRung > 0) { /* Check required because recurse_rung() can affect nRung value */
            for (event = RUNG(nRung-1,bucketIndex)->pop_front();
                    event != NULL; event = RUNG(nRung-1,bucketIndex)->pop_front()) {
                bottom.insert(event);
            }

            /* If bucket returned is the last valid rung of the bucket */
            if (numBucket[nRung-1] == bucketIndex+1) {
                while (nRung > 0) {
                    numBucket[nRung-1] = rStart[nRung-1] =
                                             rCur[nRung-1] = bucketWidth[nRung-1] = 0;
                    __sync_sub_and_fetch(&nRung, 1);
                }
            } else {
                while ((++bucketIndex < numBucket[nRung-1]) &&
                        (true == RUNG(nRung-1,bucketIndex)->empty()));
                if (bucketIndex < numBucket[nRung-1]) {
                    rCur[nRung-1] = rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1];
                } else {
                    std::cout << "numBucket handling needs improvement." << std::endl;
                    return NULL;
                }
            }

            /* Remove from bottom if not empty */
            if(!isDequeueReq) {
                event = bottom.begin();
            } else {
                event = bottom.pop_front();
            }
            if(event) {
                return event;
            }
        }

        /* Move from top to top of empty ladder */
        /* Check if failed to create the first rung */
        if (false == create_new_rung(top.size(), minTS, &isBucketWidthStatic)) {
            std::cout << "Failed to create the required rung." << std::endl;
            return NULL;
        }

        /* Transfer events from Top to 1st rung of Ladder */
        rCur[0] = rStart[0] + NUM_BUCKETS(0)*bucketWidth[0];
        for (event = top.pop_front(); event != NULL; event = top.pop_front()) {
            bucketIndex =
                (unsigned int)(event->getReceiveTime().getApproximateIntTime() -
                               rStart[0]) / bucketWidth[0];

            if (numRung0Buckets <= bucketIndex) {
                topResVec.push_back(event);
            } else {
                RUNG(0,bucketIndex)->insert(event);

                /* Update the numBucket and rCur parameter */
                if (numBucket[0] < bucketIndex+1) {
                    numBucket[0] = bucketIndex+1;
                }
                if (rCur[0] > rStart[0] + bucketIndex*bucketWidth[0]) {
                    rCur[0] = rStart[0] + bucketIndex*bucketWidth[0];
                }
            }
        }
        for(unsigned int index = 0; index < topResVec.size(); index++ ) {
            top.insert(topResVec[index]);
        }

        /* Copy events from bucket_k into Bottom */
        if (INVALID == (bucketIndex = recurse_rung())) {
            std::cout << "Received invalid Bucket index." << std::endl;
            return NULL;
        }

        for ( event = RUNG(0,bucketIndex)->pop_front();
                event != NULL; event = RUNG(0,bucketIndex)->pop_front() ) {
            bottom.insert(event);
        }

        /* If bucket returned is the last valid rung of the bucket */
        if (numBucket[0] == bucketIndex+1) {
            numBucket[0] = rStart[0] = rCur[0] = bucketWidth[0] = 0;
            __sync_sub_and_fetch(&nRung, 1);
        } else {
            while ((++bucketIndex < numBucket[0]) && (true == RUNG(0,bucketIndex)->empty()));
            if (bucketIndex < numBucket[0]) {
                rCur[0] = rStart[0] + bucketIndex*bucketWidth[0];
            } else {
                std::cout << "rung 1 numBucket handling needs improvement." << std::endl;
                return NULL;
            }
        }

        /* Remove from bottom if not empty */
        if( !isDequeueReq ) {
            event = bottom.begin();
        } else {
            event = bottom.pop_front();
        }
        return event;
    }

    /* Purge the entire LadderQ data */
    inline void clear() {

        unsigned int rungIndex = 0, bucketIndex = 0;

        /* Top variables */
        maxTS = minTS = topStart = 0;
        top.clear();

        /* Rungs */
        for (rungIndex = 0; rungIndex < MAX_RUNG_NUM; rungIndex++) {
            bucketWidth[rungIndex] = rStart[rungIndex] = rCur[rungIndex] = numBucket[rungIndex]   = 0;

            for (bucketIndex = 0; bucketIndex < NUM_BUCKETS(rungIndex); bucketIndex++) {
                RUNG(rungIndex,bucketIndex)->clear();
            }
        }
        while( !__sync_bool_compare_and_swap(&nRung, nRung, 0) );

        /* Purge bottom */
        bottom.clear();
    }

    /* Dequeue the event with lowest timestamp */
    inline const Event* dequeue() {

        const Event *retVal = begin(true);
        return retVal;
    }

    /* Check whether the LadderQ has any events or not */
    inline bool empty() {

        return ((0==nRung) & top.empty() & bottom.empty());
    }

    /* Refers to the end of LadderQ; always returns NULL */
    inline const Event* end() {
        return NULL;
    }

    /* Delete the specified event from LadderQ (if found) */
    inline void erase(const Event* delEvent) {

        unsigned int rungIndex = 0, bucketIndex = 0;
        const Event *event = NULL;
        std::vector<const Event*> eventVec;

        /* Check whether valid event received */
        if (NULL == delEvent) {
            std::cout << "Invalid event erase request received." << std::endl;
            return;
        }

        /* Check and erase in top, if found */
        if( (topStart < delEvent->getReceiveTime().getApproximateIntTime()) && 
                                                        top.erase(delEvent) ) {
            return;
        }

        /* Step through rungs */
        while ((rungIndex < nRung)
                && (delEvent->getReceiveTime().getApproximateIntTime() < rCur[rungIndex])) {
            rungIndex++;
        }

        if (rungIndex < nRung) {  /* found a rung */
            bucketIndex =
                (unsigned int)(delEvent->getReceiveTime().getApproximateIntTime() -
                               rStart[rungIndex]) / bucketWidth[rungIndex];

            if (NUM_BUCKETS(rungIndex) <= bucketIndex) {
                std::cout << "Incorrect calculation of bucket index." << std::endl;
                return;
            }

            rung_bucket = RUNG(rungIndex,bucketIndex);

            if (false == rung_bucket->empty()) {
                for (event = rung_bucket->pop_front(); event != NULL; event = rung_bucket->pop_front()) {
                    if ( (event->getReceiveTime().getApproximateIntTime() !=
                            delEvent->getReceiveTime().getApproximateIntTime()) ||
                         (event->getEventId() != delEvent->getEventId()) ||
                         (event->getSender() != delEvent->getSender())) {

                        eventVec.push_back(event);
                    }
                }
                for(unsigned int index = 0; index < eventVec.size(); index++ ) {
                    rung_bucket->insert(eventVec[index]);
                }

                /* If bucket is empty after deletion */
                if (true == rung_bucket->empty()) {
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
                        } while (true == rung_bucket->empty());

                        if (INVALID != bucketIndex) {
                            numBucket[rungIndex] = bucketIndex+1;
                        }
                    }
                }
            }
            return;
        }

        /* Check and erase from bottom, if present */
        bottom.erase(delEvent);
    }

    /* Inserts the specified event into LadderQ (if already not present) */
    inline const Event* insert(const Event* newEvent) {

        unsigned int rungIndex = 0, bucketIndex = 0;

        /* Check whether valid event received */
        if (NULL == newEvent) {
            std::cout << "Invalid event insertion request received." << std::endl;
            return NULL;
        }

        /* Insert into top, if valid */
        if (newEvent->getReceiveTime().getApproximateIntTime() >
                topStart) {  //deviation from APPENDIX of ladderq
            if (minTS > newEvent->getReceiveTime().getApproximateIntTime()) {
                minTS = newEvent->getReceiveTime().getApproximateIntTime();
            }
            if (maxTS < newEvent->getReceiveTime().getApproximateIntTime()) {
                maxTS = newEvent->getReceiveTime().getApproximateIntTime();
            }

            top.insert(newEvent);
            return newEvent;
        }

        /* Step through rungs */
        while ((rungIndex < nRung)
                && (newEvent->getReceiveTime().getApproximateIntTime() < rCur[rungIndex])) {
            rungIndex++;
        }

        if (rungIndex < nRung) {  /* found a rung */
            bucketIndex =
                (unsigned int)(newEvent->getReceiveTime().getApproximateIntTime() -
                               rStart[rungIndex]) / bucketWidth[rungIndex];

            if (NUM_BUCKETS(rungIndex) <= bucketIndex) {
                if (rungIndex > 0) {
                    std::cout << "Ran out of bucket space." << std::endl;
                } else {
                    std::cout << "Rung 1 ran out of space." << std::endl;
                }
                return NULL;
            }

            /* Adjust the numBucket and rCur parameters */
            if (numBucket[rungIndex] < bucketIndex+1) {
                numBucket[rungIndex] = bucketIndex+1;
            }
            if (rCur[rungIndex] > rStart[rungIndex] + bucketIndex*bucketWidth[rungIndex]) {
                rCur[rungIndex] = rStart[rungIndex] + bucketIndex*bucketWidth[rungIndex];
            }

            RUNG(rungIndex,bucketIndex)->insert(newEvent);
            return newEvent;
        }

        /* If rung not found */
        /* Note: In regular Ladder Queue, if bottom exceeds 
           threshold, it is transferred to the lowest available 
           rung. Reason discussed in Sec 2.4 of ladder queue 
           paper. Here, since the bottom is an unsorted queue, 
           that design is an over-kill.                         */
        bottom.insert(newEvent);

        return newEvent;
    }

private:

    /* Top variables */
    LockFreeList        top;
    unsigned int        maxTS;
    unsigned int        minTS;
    unsigned int        topStart;

    /* Rungs */
    std::vector<LockFreeList *>  rung0;  //first rung. ref. sec 2.4 of ladderq paper
    LockFreeList            *rung_bucket;
    unsigned int            numRung0Buckets;
    LockFreeList            *rung1_to_n[MAX_RUNG_NUM-1][MAX_BUCKET_NUM];  //2nd to 8th rungs
    unsigned int            nRung;
    unsigned int            bucketWidth[MAX_RUNG_NUM];
    unsigned int            numBucket[MAX_RUNG_NUM];
    unsigned int            rStart[MAX_RUNG_NUM];
    unsigned int            rCur[MAX_RUNG_NUM];

    /* Bottom */
    LockFreeList        bottom;

    /* Create (here implicitly allocate) a new rung */
    inline bool create_new_rung(unsigned int numEvents, unsigned int initStartAndCurVal,
                                bool* isBucketWidthStatic) {

        unsigned int bucketIndex = 0;

        /* Check the arguements */
        if (0 == numEvents) {
            return false;
        }

        if (NULL == isBucketWidthStatic) {
            std::cout << "Invalid memory address for monitoring change in bucketWidth" << std::endl;
            return false;
        }

        *isBucketWidthStatic = false;

        /* Check if this is the first rung creation */
        if (0 == nRung) {
            if (maxTS < minTS) {
                std::cout << "Max TS less than min TS." << std::endl;
                return false;
            } else if (minTS == maxTS) {
                bucketWidth[0] = MIN_BUCKET_WIDTH;
            } else {
                bucketWidth[0] = (maxTS - minTS + numEvents -1) / numEvents;
            }

            topStart = maxTS;
            rStart[0] = rCur[0] = minTS;
            numBucket[0] = 0;
            __sync_add_and_fetch(&nRung, 1);

            /* Create the actual rungs */
            //create double of required no of buckets. ref sec 2.4 of ladderq
            unsigned int numBucketsReq = (maxTS - minTS + bucketWidth[0] -1) / bucketWidth[0];

            for (bucketIndex = numRung0Buckets; bucketIndex < 2*numBucketsReq; bucketIndex++) {
                rung_bucket = NULL;
                if (!(rung_bucket = new LockFreeList())) {
                    std::cout << "Failed to allocate memory for rung 0 bucket." << std::endl;
                    return false;
                }
                rung0.push_back(rung_bucket);
            }
            numRung0Buckets = bucketIndex;

            return true;

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
            bucketWidth[nRung] = (bucketWidth[nRung-1] + numEvents - 1) / numEvents;
            rStart[nRung] = rCur[nRung] = initStartAndCurVal;
            numBucket[nRung] = 0;
            __sync_add_and_fetch(&nRung, 1);
            return true;
        }
    }

    /* Recurse rung */
    inline unsigned int recurse_rung() {

        bool isBucketNotFound = false, isBucketWidthStatic = false, isRungNotEmpty = false;
        unsigned int bucketIndex = 0, newBucketIndex = 0;
        const Event *event = NULL;
        std::vector<const Event*> rungResVec;

        /* find_bucket label */
        do {
            isBucketNotFound = false;
            isRungNotEmpty = false;
            bucketIndex = 0;

            if ((0 == nRung) || (MAX_RUNG_NUM < nRung)) {
                if (MAX_RUNG_NUM <= nRung) {
                    std::cout << "Invalid number of rungs available for recurse_rung." << std::endl;
                }
                return INVALID;
            }

            rCur[nRung-1] = rStart[nRung-1]; //caution

            while ((NUM_BUCKETS(nRung-1) > bucketIndex) &&
                    (true == RUNG(nRung-1,bucketIndex)->empty())) {
                bucketIndex++;
                rCur[nRung-1] += bucketWidth[nRung-1];
            }

            if (NUM_BUCKETS(nRung-1) == bucketIndex) {
                rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = numBucket[nRung-1] = 0;
                __sync_sub_and_fetch(&nRung, 1);
                isBucketNotFound = true;

            } else {
                if (THRESHOLD < RUNG(nRung-1,bucketIndex)->size()) {
                    if (false == create_new_rung(RUNG(nRung-1,bucketIndex)->size(),
                                                 rCur[nRung-1], &isBucketWidthStatic)) {
                        if (false == isBucketWidthStatic) {
                            std::cout << "Failed to create a new rung." << std::endl;
                            return INVALID;
                        }
                        break;
                    }

                    for (event = RUNG(nRung-2,bucketIndex)->pop_front();
                            event != NULL; event = RUNG(nRung-2,bucketIndex)->pop_front()) {

                        newBucketIndex = (event->getReceiveTime().getApproximateIntTime() - rStart[nRung-1]) /
                                         bucketWidth[nRung-1];
                        if (NUM_BUCKETS(nRung-1) <= newBucketIndex) {
                            rungResVec.push_back(event);

                        } else {
                            RUNG(nRung-1,newBucketIndex)->insert(event);

                            /* Calculate numBucket of new rung */
                            if (numBucket[nRung-1] < newBucketIndex+1) {
                                numBucket[nRung-1] = newBucketIndex+1;
                            }
                        }
                    }

                    for(unsigned int index = 0; index < rungResVec.size(); index++ ) {
                        RUNG(nRung-2,bucketIndex)->insert(rungResVec[index]);
                    }
                    rungResVec.clear();

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

