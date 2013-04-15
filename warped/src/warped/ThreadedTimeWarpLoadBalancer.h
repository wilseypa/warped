#ifndef THREADEDTIMEWARPLOADBALANCER_H_
#define THREADEDTIMEWARPLOADBALANCER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpSimulationManager.h"
#include "ThreadedTimeWarpMultiSet.h"
#include "StopWatch.h"

// Why is this needed?
class ThreadedTimeWarpSimulationManager;
class ThreadedTimeWarpMultiSet;

class ThreadedTimeWarpLoadBalancer {
public:
	ThreadedTimeWarpLoadBalancer(
			ThreadedTimeWarpSimulationManager* initSimulationManager,
			ThreadedTimeWarpMultiSet* eventSet,
			int measurementPeriod,
			int quietPeriod);
	~ThreadedTimeWarpLoadBalancer();
	void rollbackBalanceCheck(int LTSFId);
	void poll();

private:
	bool checkMeasurementPeriod();
	bool outsideQuietPeriod();
	void rebalance();
	void updateOffsets();
	double getMetricByLTSF(int LTSFId);
	double getMetricByObj(int objId);
	double getVariance();

	int myMeasurementPeriod;
	int myQuietPeriod;
	StopWatch myStopwatch;
	StopWatch lastRebalance;
	int objectCount;
	int LTSFCount;
	ThreadedTimeWarpMultiSet* myEventSet;
	double prevAvgMetric;

	// Variables (pointers, so they get updated)
	unsigned *committedEventsByLTSF;
	unsigned *rolledBackEventsByLTSF;
	unsigned *committedEventsByObj;
	unsigned *rolledBackEventsByObj;
	int **objectMap;

	// Offsets
	unsigned *LcommittedEventsByLTSF;
	unsigned *LrolledBackEventsByLTSF;
	unsigned *LcommittedEventsByObj;
	unsigned *LrolledBackEventsByObj;
};



#endif /* THREADEDTIMEWARPLOADBALANCER_H_ */
