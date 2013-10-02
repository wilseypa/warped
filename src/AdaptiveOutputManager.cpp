
#include "AdaptiveOutputManager.h"
#include "TimeWarpSimulationManager.h"
#include "Event.h"
#include "EventFunctors.h"

DynamicOutputManager::DynamicOutputManager(TimeWarpSimulationManager* simMgr,
                                           bool useThirdThreshold) :
    LazyOutputManager(simMgr),
    curCancelMode(simMgr->getNumberOfSimulationObjects(), AGGRESSIVE),
    hitCount(simMgr->getNumberOfSimulationObjects(), 0),
    hitRatio(simMgr->getNumberOfSimulationObjects(), 0),
    curMeasured(simMgr->getNumberOfSimulationObjects(), 0),
    permanentlyAggressive(simMgr->getNumberOfSimulationObjects(), false),
    comparisonResults(simMgr->getNumberOfSimulationObjects(), new vector<int>(filterDepth,0)),
    filterDepth(FILTER_DEPTH),
    thirdThreshold(useThirdThreshold) {
    for (int t = 0; t< compareAndInsertMode.size(); t++) {
        compareAndInsertMode[t] = false;
    }
}

DynamicOutputManager::~DynamicOutputManager() {
}

bool DynamicOutputManager::checkDynamicCancel(const Event* event) {
    //This method will just perform the LazyOutputManager::checkLazyCancelEvent.
    //Based on the bool result, a hit or miss will be recorded.
    //Messages are only suppressed when the current mode is Lazy and there is a hit.
    //The curMeasured will be kept track of using the filterdepth.
    //The event will not be inserted and events will not be cancelled.

    bool suppressMessage = false;
    bool lazyCancelHit = false;
    int numCancelledEvents = 0;
    SimulationObject* sender = getSimulationManager()->getObjectHandle(event->getSender());
    int objID = sender->getObjectID()->getSimulationObjectID();

    // Only do the check if there are any events to check.
    if ((lazyQueues[objID])->size() > 0) {
        if (!permanentlyAggressive[objID]) {
            setCompareMode(sender, curCancelMode[objID] == LAZY);
            lazyCancelHit = LazyOutputManager::checkLazyCancelEvent(event);
            numCancelledEvents = (eventsToCancel[objID])->size();

            if (curCancelMode[objID] == LAZY) {
                suppressMessage = lazyCancelHit;
                if (lazyCancelHit && getCompareMode(sender)) {
                    //Inserting the original event, reclaim this one.
                    sender->reclaimEvent(event);
                } else {
                    OutputManagerImplementationBase::insert(event);
                }
                LazyOutputManager::handleCancelEvents(sender);
            } else {
                eventsToCancel[objID]->clear();
                OutputManagerImplementationBase::insert(event);
            }

            // Record a lazy hit.
            if (lazyCancelHit) {
                (*(comparisonResults[objID]))[curMeasured[objID] % filterDepth] = 1;
                curMeasured[objID] = curMeasured[objID] + 1;
            }

            // Record a lazy miss.
            // Misses may have to be recorded as the size of the cancellation queue eventsToCancel.
            for (int i = 0; i < numCancelledEvents ; i++) {
                (*(comparisonResults[objID]))[curMeasured[objID] % filterDepth] = 0;
                curMeasured[objID] = curMeasured[objID] + 1;;
            }

            if (curMeasured[objID] >= filterDepth) {
                determineCancellationMode(objID);
                curMeasured[objID] = 0;
            }
        } else {
            OutputManagerImplementationBase::insert(event);
        }
    } else {
        OutputManagerImplementationBase::insert(event);
    }

    return suppressMessage;
}

cancellationMode DynamicOutputManager::determineCancellationMode(int objID) {
    // Calculate the hit ratio. Based on the hitRatio, determine which
    // mode should be used. Return that mode.

    SimulationObject* object = getSimulationManager()->getObjectHandle(objID);
    bool lazyToAggr = false;
    bool aggrToLazy = false;

    hitCount[objID] = 0;
    vector<int>* compRes = comparisonResults[objID];

    for (int i = 0; i < filterDepth; i++) {
        if ((*compRes)[i] == 1) {
            hitCount[objID] = hitCount[objID] + 1;
        }
    }

    hitRatio[objID] = (float)hitCount[objID] / filterDepth;

    // If the hit ratio is between the AGGRESSIVE_TO_LAZY and LAZY_TO_AGGRESSIVE
    // values, then do not change the mode.
    if (thirdThreshold && hitRatio[objID] < THIRD_THRESHOLD) {
        if (curCancelMode[objID] == LAZY) {
            debug::debugout << "Switching from Lazy to Aggressive Output Manager PERMANENTLY.\n";
            lazyToAggr = true;
        }
        curCancelMode[objID] = AGGRESSIVE;
        setCompareMode(object, false);
        permanentlyAggressive[objID] = true;
    } else if (curCancelMode[objID] == LAZY && hitRatio[objID] < LAZY_TO_AGGRESSIVE) {
        debug::debugout << "Switching from Lazy to Aggressive Output Manager.\n";
        curCancelMode[objID] = AGGRESSIVE;
        setCompareMode(object, false);
        lazyToAggr = true;
    } else if (curCancelMode[objID] == AGGRESSIVE && hitRatio[objID] > AGGRESSIVE_TO_LAZY) {
        debug::debugout << "Switching from Aggressive to Lazy Output Manager.\n";
        curCancelMode[objID] = LAZY;
        setCompareMode(object, true);
        aggrToLazy = true;
    }

    // Any possible regenerated events remaining in the lazy cancel queue have
    // already been cancelled and need to be sent, so it is essentially remaining
    // in aggressive mode until the next rollback. End this current cancellation phase
    // by emptying the lazy cancel queue.
    if (aggrToLazy) {
        (lazyQueues[objID])->clear();
    }

    // Cancel the rest of the events in the lazy cancel queue. No proceeding
    // events will be suppressed.
    if (lazyToAggr) {
        (eventsToCancel[objID])->insert((eventsToCancel[objID])->end(),
                                        (lazyQueues[objID])->begin(),
                                        (lazyQueues[objID])->end());
        handleCancelEvents(object);
        (lazyQueues[objID])->clear();
    }

    curMeasured[objID] = 0;

    return curCancelMode[objID];
}

void DynamicOutputManager::emptyLazyQueues(const VTime& time) {
    for (int t = 0; t < lazyQueues.size(); t++) {
        if ((lazyQueues[t])->size() > 0) {
            vector<const Event*>::iterator LCEvent = (lazyQueues[t])->begin();
            while (LCEvent != (lazyQueues[t])->end() && (*LCEvent)->getSendTime() < time) {
                // Only cancel when in lazy mode. The events have already been cancelled
                // when in aggressive mode. Cancelling again can cause errors.
                if (curCancelMode[t] == LAZY) {
                    (eventsToCancel[t])->push_back(*LCEvent);
                }
                LCEvent = (lazyQueues[t])->erase(LCEvent);
                (*(comparisonResults[t]))[curMeasured[t] % filterDepth] = 0;
                curMeasured[t] = curMeasured[t] + 1;
            }
        }
    }

    handleCancelEvents();

    for (int t = 0; t < curMeasured.size(); t++) {
        if (curMeasured[t] >= filterDepth) {
            determineCancellationMode(t);
            curMeasured[t] = 0;
        }
    }
}

void DynamicOutputManager::emptyLazyQueue(SimulationObject* object, const VTime& time) {
    int id = object->getObjectID()->getSimulationObjectID();
    if ((lazyQueues[id])->size() > 0) {
        vector<const Event*>::iterator LCEvent = (lazyQueues[id])->begin();
        while (LCEvent != (lazyQueues[id])->end() && (*LCEvent)->getSendTime() < time) {
            if (curCancelMode[id] == LAZY) {
                (eventsToCancel[id])->push_back(*LCEvent);
            }
            LCEvent = (lazyQueues[id])->erase(LCEvent);
            (*(comparisonResults[id]))[curMeasured[id] % filterDepth] = 0;
            curMeasured[id] = curMeasured[id] + 1;
        }
    }

    handleCancelEvents(object);

    if (curMeasured[id] >= filterDepth) {
        determineCancellationMode(id);
        curMeasured[id] = 0;
    }
}

void DynamicOutputManager::rollback(SimulationObject* object, const VTime& rollbackTime) {
    int objID = object->getObjectID()->getSimulationObjectID();

    OutputEvents& outputEvents = getOutputEventsFor(*(object->getObjectID()));
    vector<const Event*>* tempOutEvents = outputEvents.getEventsSentAtOrAfterAndRemove(rollbackTime);

    if (!permanentlyAggressive[objID]) {
        //These output events need to be added to the lazy cancel queue. There may already be
        //events in the queue, so the new ones need to be added.
        vector<const Event*>* lazyCancelEvents = lazyQueues[objID];
        lazyCancelEvents->insert(lazyCancelEvents->end(), tempOutEvents->begin(), tempOutEvents->end());
        std::sort(lazyCancelEvents->begin(), lazyCancelEvents->end(), receiveTimeLessThan());

        //Send out the anti-messages if in aggressive mode.
        if (curCancelMode[objID] == AGGRESSIVE) {
            if (tempOutEvents->size() > 0) {
                getSimulationManager()->cancelEvents(*tempOutEvents);
            }
        }
    } else {
        if (tempOutEvents->size() > 0) {
            getSimulationManager()->cancelEvents(*tempOutEvents);
        }
    }
    delete tempOutEvents;
}

void DynamicOutputManager::ofcPurge() {
    // Reset all of the values.
    for (int t = 0; t < getSimulationManager()->getNumberOfSimulationObjects(); t++) {
        // Take care of the LazyOutputManager members.
        lazyQueues[t]->clear();
        eventsToCancel[t]->clear();
        lazyHitCount[t] = 0;
        lazyMissCount[t] = 0;

        // Take care of the DynamicOutputManager members.
        hitCount[t] = 0;
        hitRatio[t] = 0;
        curMeasured[t] = 0;
        for (int i = 0; i < comparisonResults[t]->size(); i++) {
            (*comparisonResults[t])[i] = 0;
        }
    }

    // Clear the output events.
    OutputManagerImplementationBase::ofcPurge();
}
