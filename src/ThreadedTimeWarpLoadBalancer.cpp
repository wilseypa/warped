
#include <string.h>                     // for memcpy, memset
#include <iostream>                     // for operator<<, basic_ostream, etc

#include "SetObject.h"                  // for ostream
#include "ThreadedTimeWarpLoadBalancer.h"
#include "ThreadedTimeWarpMultiSet.h"   // for ThreadedTimeWarpMultiSet, etc
#include "ThreadedTimeWarpSimulationManager.h"

ThreadedTimeWarpLoadBalancer::ThreadedTimeWarpLoadBalancer(
    ThreadedTimeWarpSimulationManager* initSimulationManager,
    ThreadedTimeWarpMultiSet* eventSet,
    double iVarianceThresh,
    unsigned int iNormalInterval,
    unsigned int iNormalThresh,
    unsigned int iRelaxedInterval,
    unsigned int iRelaxedThresh) {
    varianceThresh = iVarianceThresh;
    normalInterval = iNormalInterval;
    normalThresh = iNormalThresh;
    relaxedInterval = iRelaxedInterval;
    relaxedThresh = iRelaxedThresh;

    // Setup initial state
    //  Indicates if in normal or relaxed mode
    normalMode = true;
    //  Indicates number of consecutive measurements within current state
    stateCounter = 0;

    objectCount = initSimulationManager->getNumberOfSimulationObjects();
    LTSFCount = initSimulationManager->getScheduleQCount();
    myEventSet = eventSet;
    myStopwatch.start();
    lastRebalance.start();
    prevAvgMetric = 0.8;

    // Initialize the variables
    committedEventsByLTSF = myEventSet->getCommittedEventsByLTSF();
    rolledBackEventsByLTSF = myEventSet->getRolledBackEventsByLTSF();
    committedEventsByObj = myEventSet->getCommittedEventsByObj();
    rolledBackEventsByObj = myEventSet->getRolledBackEventsByObj();
    objectMap = myEventSet->getObjectMapping();

    // Setup offsets
    LcommittedEventsByLTSF = new unsigned[LTSFCount];
    LrolledBackEventsByLTSF = new unsigned[LTSFCount];
    LcommittedEventsByObj = new unsigned[objectCount];
    LrolledBackEventsByObj = new unsigned[objectCount];

    // Set offsets to 0
    memset(LcommittedEventsByLTSF, 0, LTSFCount*sizeof(*LcommittedEventsByLTSF));
    memset(LrolledBackEventsByLTSF, 0, LTSFCount*sizeof(*LrolledBackEventsByLTSF));
    memset(LcommittedEventsByObj, 0, LTSFCount*sizeof(*LcommittedEventsByObj));
    memset(LrolledBackEventsByObj, 0, LTSFCount*sizeof(*LrolledBackEventsByObj));
}
ThreadedTimeWarpLoadBalancer::~ThreadedTimeWarpLoadBalancer() {}

// Updates offset values
void ThreadedTimeWarpLoadBalancer::updateOffsets() {
    memcpy(LcommittedEventsByLTSF, committedEventsByLTSF, LTSFCount*sizeof(unsigned));
    memcpy(LrolledBackEventsByLTSF, rolledBackEventsByLTSF, LTSFCount*sizeof(unsigned));
    memcpy(LcommittedEventsByObj, committedEventsByObj, objectCount*sizeof(unsigned));
    memcpy(LrolledBackEventsByObj, rolledBackEventsByObj, objectCount*sizeof(unsigned));
    cout << "Offsets updated" << endl;
}

double ThreadedTimeWarpLoadBalancer::getMetricByLTSF(int LTSFId) {
    unsigned committedEvents = committedEventsByLTSF[LTSFId] - LcommittedEventsByLTSF[LTSFId];
    unsigned rolledBackEvents = rolledBackEventsByLTSF[LTSFId] - LrolledBackEventsByLTSF[LTSFId];
    cout << "committed = " << committedEvents << endl;
    cout << "rolled back = " << rolledBackEvents << endl;
    if (committedEvents != 0) {
        return ((double)committedEvents - (double)rolledBackEvents)/((double)committedEvents);
    }
    return 0;
}

double ThreadedTimeWarpLoadBalancer::getMetricByObj(int objId) {
    unsigned committedEvents = committedEventsByObj[objId] - LcommittedEventsByObj[objId];
    unsigned rolledBackEvents = rolledBackEventsByObj[objId] - LrolledBackEventsByObj[objId];
    if (committedEvents != 0) {
        return ((double)committedEvents - (double)rolledBackEvents)/((double)committedEvents);
    }
    return 0;
}

// Returns the variance between the largest and smallest 'metric'
double ThreadedTimeWarpLoadBalancer::getVariance() {
    double minVal = 1000000;
    double maxVal = 0;
    for (int i=0; i<LTSFCount; i++) {
        double workMetric = getMetricByLTSF(i);
        // Update max
        if (workMetric > maxVal) {
            maxVal = workMetric;
        }
        // Update min
        if (workMetric < minVal) {
            minVal = workMetric;
        }
    }
    return maxVal - minVal;
}

// Checks the current LTSF to see if a load balance should be performed
void ThreadedTimeWarpLoadBalancer::balanceCheck() {
    if (outsideQuietPeriod()) {
        double variance = getVariance();
        cout << "variance = " << variance << endl;
        //if (workMetric <= prevAvgMetric) {
        if (variance >= varianceThresh) {
            rebalance();
            stateCounter = normalMode ? 0 : stateCounter+1;
        } else {
            updateOffsets();
            stateCounter = normalMode ? stateCounter+1 : 0;
        }
        cout << (normalMode?"Normal : ":"Relaxed : ") << stateCounter << endl;
        // Set new state
        if (normalMode) {
            if (stateCounter >= normalThresh) {
                normalMode = false;
                stateCounter = 0;
                cout << "Entering relaxed mode" << endl;
            }
        } else {
            if (stateCounter >= relaxedThresh) {
                normalMode = true;
                stateCounter = 0;
                cout << "Entering normal mode" << endl;
            }
        }
    }
}

// Check if a rebalance was performed within the given period
bool ThreadedTimeWarpLoadBalancer::outsideQuietPeriod() {
    //return (lastRebalance.elapsed() > normalInterval);
    unsigned int interval = normalMode ? normalInterval : relaxedInterval;
    if (lastRebalance.elapsed() > interval) {
        lastRebalance.reset();
        lastRebalance.start();
        return true;
    }
    return false;
}

void ThreadedTimeWarpLoadBalancer::rebalance() {

    // Calculate metrics
    // Find slowest and fastest LTSF queues, and update average metric
    int minLTSF = -1;
    int maxLTSF = -1;
    double minVal = 1000000;
    double maxVal = 0;
    prevAvgMetric = 0;
    for (int i=0; i<LTSFCount; i++) {
        double workMetric = getMetricByLTSF(i);
        cout << "WorkMetric = " << workMetric << endl;
        prevAvgMetric += workMetric;
        // Update max
        if (workMetric > maxVal) {
            maxVal = workMetric;
            maxLTSF = i;
        }
        // Update min
        if (workMetric < minVal) {
            minVal = workMetric;
            minLTSF = i;
        }
    }
    prevAvgMetric = prevAvgMetric / LTSFCount;
    // Quit load balancing if min and max are the same.
    // In the future we might make sure these differ by a
    // threshold before doing a load balance
    if (minLTSF == maxLTSF) {
        cout << "Load balance cancelled. Min and max are equal." << endl;
        return;
    }
    // Find max metric object in high metric LTSF queue
    double maxMetric = 0;
    int maxObjId;
    for (int i = 0; i<objectCount; i++) {
        // Check if object in high metric queue
        if (objectMap[i][LTSFOWNER] == maxLTSF) {
            double workMetric = getMetricByObj(i);
            if (workMetric > maxMetric) {
                maxMetric = workMetric;
                maxObjId = i;
            }
        }
    }

    // Move the max metric object to the low metric LTSF queue
    // Possibly move more than one in the future?
    cout << "LB moving objId " << maxObjId << " from LTSF "
         << maxLTSF << " to LTSF " << minLTSF << endl;
    myEventSet->moveLP(maxObjId, minLTSF);

    updateOffsets();
}
