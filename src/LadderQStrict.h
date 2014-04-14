
#ifndef LadderQueueStrict_H_
#define LadderQueueStrict_H_

/* Include section */
#include <iostream>
#include <algorithm>
#include <list>
#include <set>
#include <vector>

#include "EventFunctors.h"

/* Macro section */
#define MAX_RUNG_NUM     8  //ref. sec 2.4 of ladderq paper
#define THRESHOLD        50 //ref. sec 2.3 of ladderq paper
#define MAX_BUCKET_NUM   THRESHOLD //ref. sec 2.4 of ladderq paper
#define MIN_BUCKET_WIDTH 1
#define INVALID          0xFFFF

#define RUNG(x,y) (((x)==0) ? (rung0[(y)]) : (rung1_to_n[(x)-1][(y)]))
#define NUM_BUCKETS(x) (((x)==0) ? (numRung0Buckets) : (MAX_BUCKET_NUM))

/* Ladder Queue class */
class LadderQueueStrict {

public:

    /* Default constructor */
    inline LadderQueueStrict() {
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
                if (!(rung_bucket = new std::list<const Event*>)) {
                    std::cout << "Failed to allocate memory for Rung "
                         << rungIndex+2 << ", Bucket " << bucketIndex+1 << "." << std::endl;
                }
                rung1_to_n[rungIndex][bucketIndex] = rung_bucket;
            }
        }
    }

    /* Destructor */
    inline ~LadderQueueStrict() {
        clear();
    }

    /* Peek at the event with lowest timestamp */
    inline const Event* begin() {

        unsigned int bucketIndex = 0;
        std::list<const Event*>::iterator lIterate;
        bool isBucketWidthStatic = false;

        /* Remove from bottom if not empty */
        if (!bottomEmpty()) {
            const Event* event = bottomBegin();
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
            for (lIterate = RUNG(nRung-1,bucketIndex)->begin();
                    lIterate != RUNG(nRung-1,bucketIndex)->end(); lIterate++) {
                bottomInsert(*lIterate);
            }
            RUNG(nRung-1,bucketIndex)->clear();

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
                    return NULL;
                }
            }

            if (true == bottomEmpty()) {
                std::cout << "Bottom empty 1" << std::endl;
                return NULL;
            }

            const Event* event = bottomBegin();
            return event;
        }

        /* Check if top has any events before proceeding further */
        if (true == top.empty()) {
            std::cout << "LadderQ is empty." << std::endl;
            return NULL;
        }

        /* Move from top to top of empty ladder */
        /* Check if failed to create the first rung */
        if (false == create_new_rung(top.size(), minTS, &isBucketWidthStatic)) {
            std::cout << "Failed to create the required rung." << std::endl;
            return NULL;
        }

        /* Transfer events from Top to 1st rung of Ladder */
        rCur[0] = rStart[0] + NUM_BUCKETS(0)*bucketWidth[0];
        for (lIterate = top.begin(); lIterate != top.end();) {
            ASSERT( (*lIterate)->getReceiveTime().getApproximateIntTime() >= rStart[0] );
            bucketIndex =
                std::min( (unsigned int)((*lIterate)->getReceiveTime().getApproximateIntTime() -
                              rStart[0]) / bucketWidth[0],
                          NUM_BUCKETS(0)-1 );

            RUNG(0,bucketIndex)->push_front(*lIterate);
            lIterate = top.erase(lIterate);

            /* Update the numBucket and rCur parameter */
            if (numBucket[0] < bucketIndex+1) {
                numBucket[0] = bucketIndex+1;
            }
            if (rCur[0] > rStart[0] + bucketIndex*bucketWidth[0]) {
                rCur[0] = rStart[0] + bucketIndex*bucketWidth[0];
            }
        }

        /* Copy events from bucket_k into Bottom */
        if (INVALID == (bucketIndex = recurse_rung())) {
            std::cout << "Received invalid Bucket index." << std::endl;
            return NULL;
        }

        for (lIterate = RUNG(nRung-1,bucketIndex)->begin();
                lIterate != RUNG(nRung-1,bucketIndex)->end(); lIterate++) {
            bottomInsert(*lIterate);
        }

        /* Clear that bucket */
        RUNG(nRung-1,bucketIndex)->clear();

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

        if (true == bottomEmpty()) {
            std::cout << "Bottom empty 2" << std::endl;
            return NULL;
        }

        const Event* event = bottomBegin();
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
        nRung = 0;

        /* Purge bottom */
        bottomClear();
    }

    /* Dequeue the event with lowest timestamp */
    inline const Event* dequeue() {

        const Event* retVal = NULL;
        if (NULL != (retVal = begin())) {
            bottom.erase(bottom.begin());
        }
        return retVal;
    }

    /* Check whether the LadderQ has any events or not */
    inline bool empty() {

        return ((0==nRung) & top.empty() & bottomEmpty());
    }

    /* Refers to the end of LadderQ; always returns NULL */
    inline const Event* end() {
        return NULL;
    }

    /* Delete the specified event from LadderQ (if found) */
    inline void erase(const Event* delEvent) {

        std::list<const Event*>::iterator lIterate;
        unsigned int rungIndex = 0, bucketIndex = 0;

        /* Check whether valid event received */
        if (NULL == delEvent) {
            std::cout << "Invalid event erase request received." << std::endl;
            return;
        }

        /* Check and erase in top, if found */
        if ((false == top.empty()) && (topStart < delEvent->getReceiveTime().getApproximateIntTime())) {
            for (lIterate = top.begin(); lIterate != top.end();) {
                if (((*lIterate)->getReceiveTime().getApproximateIntTime() ==
                        delEvent->getReceiveTime().getApproximateIntTime()) &&
                        ((*lIterate)->getEventId() == delEvent->getEventId()) &&
                        ((*lIterate)->getSender() == delEvent->getSender())) {

                    lIterate = top.erase(lIterate);
                } else {
                    lIterate++;
                }
            }
            return;
        }

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

            rung_bucket = RUNG(rungIndex,bucketIndex);
            if (false == rung_bucket->empty()) {
                for (lIterate = rung_bucket->begin(); lIterate != rung_bucket->end();) {
                    if (((*lIterate)->getReceiveTime().getApproximateIntTime() ==
                            delEvent->getReceiveTime().getApproximateIntTime()) &&
                            ((*lIterate)->getEventId() == delEvent->getEventId()) &&
                            ((*lIterate)->getSender() == delEvent->getSender())) {

                        lIterate = rung_bucket->erase(lIterate);
                    } else {
                        lIterate++;
                    }
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
        if (false == bottomEmpty()) {
            bottomErase(delEvent);
        }
    }

    /* Inserts the specified event into LadderQ (if already not present) */
    inline const Event* insert(const Event* newEvent) {

        bool isBucketWidthStatic = false;
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

            top.push_front(newEvent);

            return newEvent;
        }

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

            RUNG(rungIndex,bucketIndex)->push_front(newEvent);

            return newEvent;
        }

        /* If rung not found */
        if (THRESHOLD < bottomSize()) {
            if (MAX_RUNG_NUM <= nRung) {
                isBucketWidthStatic = true;

            } else { /* Check if failed to create a rung */

                /* Check if new event to be inserted is smaller than what is present in BOTTOM */
                unsigned int uiBucketStartVal = bottomBegin()->getReceiveTime().getApproximateIntTime();
                if (uiBucketStartVal > newEvent->getReceiveTime().getApproximateIntTime()) {
                    uiBucketStartVal = newEvent->getReceiveTime().getApproximateIntTime();
                }

                if ((false == create_new_rung(bottomSize(), uiBucketStartVal, &isBucketWidthStatic)) &&
                        (false == isBucketWidthStatic)) {
                    std::cout << "Failed to create the required rung." << std::endl;
                    return NULL;
                }
            }

            /* Intentionally let the bottom continue to overflow */
            //ref sec 2.4 of ladderq + when bucket width becomes static
            if (true == isBucketWidthStatic) {
                bottomInsert(newEvent);
                return newEvent;
            }

            /* Transfer bottom to new rung */

            std::multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator mIterate;
            for (mIterate = bottom.begin(); mIterate != bottom.end(); mIterate++) {
                ASSERT( (*mIterate)->getReceiveTime().getApproximateIntTime() >= rStart[nRung-1] );
                bucketIndex =
                    std::min( (unsigned int)(((*mIterate)->getReceiveTime().getApproximateIntTime() -
                                  rStart[nRung-1]) / bucketWidth[nRung-1]),
                              NUM_BUCKETS(nRung-1)-1 );

                /* Adjust the numBucket and rCur parameters */
                if (numBucket[nRung-1] < bucketIndex+1) {
                    numBucket[nRung-1] = bucketIndex+1;
                }
                if (mIterate == bottom.begin()) {
                    rCur[nRung-1] = rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1];
                }

                RUNG(nRung-1,bucketIndex)->push_front(*mIterate);
            }
            bottomClear();

            /* Insert new element in the new and populated rung */
            ASSERT( newEvent->getReceiveTime().getApproximateIntTime() >= rStart[nRung-1] );
            bucketIndex =
                std::min( (unsigned int)((newEvent->getReceiveTime().getApproximateIntTime() -
                              rStart[nRung-1]) / bucketWidth[nRung-1]),
                          NUM_BUCKETS(nRung-1)-1 );

            if (numBucket[nRung-1] < bucketIndex+1) {
                numBucket[nRung-1] = bucketIndex+1;
            }
            if (rCur[nRung-1] > rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1]) {
                rCur[nRung-1] = rStart[nRung-1] + bucketIndex*bucketWidth[nRung-1];
            }

            RUNG(nRung-1,bucketIndex)->push_front(newEvent);

        } else { /* If BOTTOM is within threshold */
            bottomInsert(newEvent);
        }

        return newEvent;
    }

private:

    /* Top variables */
    std::list<const Event*>  top;
    unsigned int        maxTS;
    unsigned int        minTS;
    unsigned int        topStart;

    /* Rungs */
    std::vector<std::list<const Event*> *> rung0;  //first rung. ref. sec 2.4 of ladderq paper
    std::list<const Event*>* rung_bucket;
    unsigned int        numRung0Buckets;
    std::list<const Event*>* rung1_to_n[MAX_RUNG_NUM-1][MAX_BUCKET_NUM];  //2nd to 8th rungs
    unsigned int        nRung;
    unsigned int        bucketWidth[MAX_RUNG_NUM];
    unsigned int        numBucket[MAX_RUNG_NUM];
    unsigned int        rStart[MAX_RUNG_NUM];
    unsigned int        rCur[MAX_RUNG_NUM];

    /* Bottom */
    std::multiset<const Event*, receiveTimeLessThanEventIdLessThan> bottom;

    /** BOTTOM Functionalities */
    /* Bottom erase */
    void bottomErase(const Event* delEvent) {
        (void) bottom.erase(delEvent);
    }

    /* Bottom insert */
    void bottomInsert(const Event* newEvent) {
        bottom.insert(newEvent);
    }

    /* Bottom empty */
    bool bottomEmpty() {
        return bottom.empty();
    }

    /* Bottom begin */
    const Event* bottomBegin() {
        return (*bottom.begin());
    }

    /* Bottom clear */
    void bottomClear() {
        bottom.clear();
    }

    /* Bottom size */
    unsigned int bottomSize() {
        return bottom.size();
    }

    /* Create (here implicitly allocate) a new rung */
    inline bool create_new_rung(unsigned int numEvents, unsigned int initStartAndCurVal,
                                bool* isBucketWidthStatic) {

        unsigned int bucketIndex = 0;

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
            nRung++;

            /* Create the actual rungs */
            //create double of required no of buckets. ref sec 2.4 of ladderq
            unsigned int numBucketsReq = (maxTS - minTS + bucketWidth[0] -1) / bucketWidth[0];

            for (bucketIndex = numRung0Buckets; bucketIndex < 2*numBucketsReq; bucketIndex++) {
                rung_bucket = NULL;
                if (!(rung_bucket = new std::list<const Event*>)) {
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
            nRung++;
            return true;
        }
    }

    /* Recurse rung */
    inline unsigned int recurse_rung() {

        bool isBucketNotFound = false, isBucketWidthStatic = false, isRungNotEmpty = false;
        unsigned int bucketIndex = 0, newBucketIndex = 0;
        std::list<const Event*>::iterator lIterate;

        /* find_bucket label */
        do {
            isBucketNotFound = false;
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
                        if (false == isBucketWidthStatic) {
                            std::cout << "Failed to create a new rung." << std::endl;
                            return INVALID;
                        }
                        break;
                    }

                    for (lIterate = RUNG(nRung-2,bucketIndex)->begin();
                            lIterate != RUNG(nRung-2,bucketIndex)->end();) {
                        ASSERT( (*lIterate)->getReceiveTime().getApproximateIntTime() >= rStart[nRung-1] );
                        newBucketIndex = 
                            std::min( (unsigned int) ((*lIterate)->getReceiveTime().getApproximateIntTime() - 
                                          rStart[nRung-1]) / bucketWidth[nRung-1],
                                      NUM_BUCKETS(nRung-1)-1 );

                        RUNG(nRung-1,newBucketIndex)->push_front(*lIterate);
                        lIterate = RUNG(nRung-2,bucketIndex)->erase(lIterate);

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

#endif /* LadderQueueStrict_H_ */

