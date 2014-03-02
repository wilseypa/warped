#include <math.h>                       // for sqrt
#include <pthread.h>                    // for pthread_getspecific
#include <iostream>                     // for operator<<, etc

#include "LockState.h"                  // for LockState
#include "ObjectID.h"                   // for ObjectID
#include "SetObject.h"                  // for ostream
#include "SimulationObject.h"           // for SimulationObject
#include "ThreadedChebyFossilCollManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "VTime.h"                      // for VTime
#include "WarpedDebug.h"                // for debugout
#include "warped.h"                     // for ASSERT

using std::cout;

ThreadedChebyFossilCollManager::ThreadedChebyFossilCollManager(ThreadedTimeWarpSimulationManager*
                                                               sim,
                                                               int checkPeriod,
                                                               int minimumSamples,
                                                               int maximumSamples,
                                                               int defaultLen,
                                                               double risk):
    ThreadedOptFossilCollManager(sim, checkPeriod, minimumSamples, maximumSamples, defaultLen, risk),
    errorTerm(2.576) {

    for (int i = 0; i < sim->getNumberOfSimulationObjects(); i++) {
        samples.push_back(vector<unsigned int>(maxSamples, 0));
        total.push_back(0);
        sampleIndex.push_back(0);
    }
    ofcChebyLock = new LockState();
}

ThreadedChebyFossilCollManager::~ThreadedChebyFossilCollManager() {
  delete ofcChebyLock;
}

void
ThreadedChebyFossilCollManager::sampleRollback(SimulationObject* object, const VTime& rollVTime) {
    int rollbackTime = rollVTime.getApproximateIntTime();
    int rollbackDistance = object->getSimulationTime().getApproximateIntTime() - rollbackTime;
    unsigned int objId = object->getObjectID()->getSimulationObjectID();
    
    int threadId = *((int*) pthread_getspecific(threadKey)); 
    getOfcChebyLock(threadId,mySimManager->getSyncMechanism()); 
    
    if (numSamples[objId] < maxSamples) {
        // Sample the rollback.
        total[objId] = total[objId] - samples[objId][sampleIndex[objId]];
        samples[objId][sampleIndex[objId]] = rollbackDistance;
        total[objId] = total[objId] + rollbackDistance;

        sampleIndex[objId]++;
        numSamples[objId]++;

        // If there are enough samples, then calculate the mean, variance
        // and new active history length.
        if (numSamples[objId] > minSamples) {
            double sampleVariance = 0;
            double mean1, mean2;
            double variance1, variance2;
            mean1 = mean2 = variance1 = variance2 = 0;

            double sampleMean = total[objId] / double(numSamples[objId]);

            for (int i = 0; i < numSamples[objId]; i++) {
                sampleVariance += ((double(samples[objId][i]) - sampleMean) *
                                   (double(samples[objId][i]) - sampleMean));
            }

            sampleVariance = sampleVariance / numSamples[objId];

            mean1 = sampleMean - 1.96*sqrt(sampleVariance/(double)numSamples[objId]);
            mean2 = sampleMean + 1.96*sqrt(sampleVariance/(double)numSamples[objId]);

            for (int i = 0; i < numSamples[objId]; i++) {
                variance1 += ((double(samples[objId][i]) - mean1) *
                              (double(samples[objId][i]) - mean1));
                variance2 += ((double(samples[objId][i]) - mean2) *
                              (double(samples[objId][i]) - mean2));
            }

            variance1 = variance1 / numSamples[objId];
            variance2 = variance2 / numSamples[objId];

            if (variance1 < variance2) {
                sampleVariance = variance2;
            } else {
                sampleVariance = variance1;
            }

            activeHistoryLength[objId] = sampleMean + errorTerm * sqrt(sampleVariance/(1.0-riskFactor));
            debug::debugout << objId << " - NEW LENGTH: " << activeHistoryLength[objId] << endl;
        }
    }

    if (lastCollectTimes[objId] >= 0 && !recovering && rollbackTime <= lastCollectTimes[objId]) {
        debug::debugout << mySimManager->getSimulationManagerID()
                        << " - Catastrophic Rollback: Last collection time: " << lastCollectTimes[objId]
                        << ", Rollback Time: " << rollbackTime << ", Starting Recovery." << endl;

        setRecovery(objId, rollbackTime);
    }
    releaseOfcChebyLock(threadId,mySimManager->getSyncMechanism()); 
}


void ThreadedChebyFossilCollManager::getOfcChebyLock(int threadId, const string syncMech ) {
    ofcFlagLock->setLock(threadId,syncMech);
    ASSERT(ofcFlagLock->hasLock(threadId,syncMech));
}

void ThreadedChebyFossilCollManager ::releaseOfcChebyLock(int threadId, const string syncMech){
    ASSERT(ofcFlagLock->hasLock(threadId,syncMech));
    ofcFlagLock->releaseLock(threadId,syncMech);
}

