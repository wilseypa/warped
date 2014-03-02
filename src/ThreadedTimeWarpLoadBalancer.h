#ifndef THREADEDTIMEWARPLOADBALANCER_H_
#define THREADEDTIMEWARPLOADBALANCER_H_


#include "StopWatch.h"                  // for StopWatch
#include "ThreadedTimeWarpMultiSet.h"
#include "ThreadedTimeWarpSimulationManager.h"

class ThreadedTimeWarpMultiSet;
// Why is this needed?
class ThreadedTimeWarpSimulationManager;

class ThreadedTimeWarpLoadBalancer {
public:
    ThreadedTimeWarpLoadBalancer(
        ThreadedTimeWarpSimulationManager* initSimulationManager,
        ThreadedTimeWarpMultiSet* eventSet,
        double iVarianceThresh,
        unsigned int iNormalInterval,
        unsigned int iNormalThresh,
        unsigned int iRelaxedInterval,
        unsigned int iRelaxedThresh);
    ~ThreadedTimeWarpLoadBalancer();
    void balanceCheck();

private:
    bool checkMeasurementPeriod();
    bool outsideQuietPeriod();
    void rebalance();
    void updateOffsets();
    double getMetricByLTSF(int LTSFId);
    double getMetricByObj(int objId);
    double getVariance();

    // Parameters
    double varianceThresh;
    unsigned int normalInterval;
    unsigned int normalThresh;
    unsigned int relaxedInterval;
    unsigned int relaxedThresh;

    // States
    bool normalMode;
    unsigned int stateCounter;

    StopWatch myStopwatch;
    StopWatch lastRebalance;
    int objectCount;
    int LTSFCount;
    ThreadedTimeWarpMultiSet* myEventSet;
    double prevAvgMetric;

    // Variables (pointers, so they get updated)
    unsigned* committedEventsByLTSF;
    unsigned* rolledBackEventsByLTSF;
    unsigned* committedEventsByObj;
    unsigned* rolledBackEventsByObj;
    int** objectMap;

    // Offsets
    unsigned* LcommittedEventsByLTSF;
    unsigned* LrolledBackEventsByLTSF;
    unsigned* LcommittedEventsByObj;
    unsigned* LrolledBackEventsByObj;
};



#endif /* THREADEDTIMEWARPLOADBALANCER_H_ */
