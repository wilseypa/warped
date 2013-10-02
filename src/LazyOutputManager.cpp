
#include "LazyOutputManager.h"
#include "TimeWarpSimulationManager.h"
#include "Event.h"
#include "EventFunctors.h"

LazyOutputManager::LazyOutputManager(TimeWarpSimulationManager* simMgr) :
    OutputManagerImplementationBase(simMgr) {
    for (int i = 0; i < simMgr->getNumberOfSimulationObjects(); i++) {
        lazyQueues.push_back(new vector<const Event*>());
        eventsToCancel.push_back(new vector<const Event*>());
        compareAndInsertMode.push_back(true);
        lazyHitCount.push_back(0);
        lazyMissCount.push_back(0);
    }
}

LazyOutputManager::~LazyOutputManager() {
}

bool LazyOutputManager::checkLazyCancelEvent(const Event* event) {

    bool suppressEvent = false;
    int id = getSimulationManager()->getObjectHandle(
                 event->getSender())->getObjectID()->getSimulationObjectID();
    vector<const Event*>* lazyCancelEvents = lazyQueues[id];
    vector<const Event*>* eveToCan = eventsToCancel[id];

    //Perform lazy cancellation when there are still events to be compared to in the
    //lazy cancellation queue.
    if (lazyCancelEvents->size() != 0) {
        vector<const Event*>::iterator LCEvent = lazyCancelEvents->begin();

        //Any events with a timestamp less than the current event time
        //were not regenerated. Add them to the cancel events list.
        while (LCEvent != lazyCancelEvents->end() && (*LCEvent)->getSendTime() < event->getSendTime()) {
            eveToCan->push_back(*LCEvent);
            lazyCancelEvents->erase(LCEvent);
            lazyMissCount[id]++;
        }

        //Compare the events in the lazy cancellation queue to this event.
        //If the queue is empty after checking for past time stamps, end lazy cancellation.
        if (lazyCancelEvents->size() > 0) {
            LCEvent = lazyCancelEvents->begin();

            while (suppressEvent == false && LCEvent != lazyCancelEvents->end()) {

                if (const_cast<Event*>((*LCEvent))->eventCompare(event)) {
                    if (compareAndInsertMode[id]) {
                        OutputManagerImplementationBase::insert(*LCEvent);
                    }
                    suppressEvent = true;
                    lazyHitCount[id]++;
                    LCEvent = lazyCancelEvents->erase(LCEvent);
                } else {
                    LCEvent++;
                }
            }

            if (lazyCancelEvents->size() <= 0) {
                //End lazy cancellation phase.
                debug::debugout << "Lazy Cancellation Phase Complete For Object: " << event->getSender()
                                << " Lazy Hits: " << lazyHitCount[id] << " Lazy Misses: " << lazyMissCount[id] << "\n";
                lazyHitCount[id] = 0;
                lazyMissCount[id] = 0;
            }
        } else {
            //End lazy cancellation phase.
            debug::debugout << "Lazy Cancellation Phase Complete For Object: " << event->getSender()
                            << " Lazy Hits: " << lazyHitCount[id] << " Lazy Misses: " << lazyMissCount[id] << "\n";
            lazyHitCount[id] = 0;
            lazyMissCount[id] = 0;
        }
    } else {
        // Not in lazy cancellation.
        lazyHitCount[id] = 0;
        lazyMissCount[id] = 0;
    }

    return suppressEvent;
}

void LazyOutputManager::handleCancelEvents() {
    for (int t = 0; t < eventsToCancel.size(); t++) {
        if ((eventsToCancel[t])->size() > 0) {
            getSimulationManager()->cancelEvents(*(eventsToCancel[t]));
            (eventsToCancel[t])->clear();
        }
    }
}

void LazyOutputManager::handleCancelEvents(SimulationObject* object) {
    int id = object->getObjectID()->getSimulationObjectID();
    if ((eventsToCancel[id])->size() > 0) {
        getSimulationManager()->cancelEvents(*(eventsToCancel[id]));
        (eventsToCancel[id])->clear();
    }
}

void LazyOutputManager::emptyLazyQueues(const VTime& time) {
    for (int t = 0; t < lazyQueues.size(); t++) {
        if ((lazyQueues[t])->size() > 0) {
            vector<const Event*>::iterator LCEvent = (lazyQueues[t])->begin();
            while (LCEvent != (lazyQueues[t])->end() && (*LCEvent)->getSendTime() < time) {
                (eventsToCancel[t])->push_back(*LCEvent);
                LCEvent = (lazyQueues[t])->erase(LCEvent);
                lazyMissCount[t]++;
            }
        }
    }
    handleCancelEvents();
}

void LazyOutputManager::emptyLazyQueue(SimulationObject* object, const VTime& time) {
    int id = object->getObjectID()->getSimulationObjectID();
    if ((lazyQueues[id])->size() > 0) {
        vector<const Event*>::iterator LCEvent = (lazyQueues[id])->begin();
        while (LCEvent != (lazyQueues[id])->end() && (*LCEvent)->getSendTime() < time) {
            (eventsToCancel[id])->push_back(*LCEvent);
            LCEvent = (lazyQueues[id])->erase(LCEvent);
            lazyMissCount[id]++;
        }
    }
    handleCancelEvents(object);
}

const VTime& LazyOutputManager::getLazyQMinTime() {
    const VTime* minTime = &getSimulationManager()->getPositiveInfinity();
    const VTime* curTime = NULL;
    for (int objectID = 0; objectID < lazyQueues.size(); objectID++) {
        curTime = &getLazyQMinTime(objectID);
        if (*curTime < *minTime) {
            minTime = curTime;
        }
    }
    return (*minTime);
}

const VTime& LazyOutputManager::getLazyQMinTime(const unsigned int objectID) {
    const VTime* minTime = &getSimulationManager()->getPositiveInfinity();
    if ((lazyQueues[objectID])->size() > 0) {
        minTime = &(lazyQueues[objectID])->front()->getReceiveTime();
    }
    return (*minTime);
}

bool LazyOutputManager::lazyCancel(const Event* event) {
    SimulationObject* sender = getSimulationManager()->getObjectHandle(event->getSender());
    int id = sender->getObjectID()->getSimulationObjectID();
    bool retval = checkLazyCancelEvent(event);
    if (retval && compareAndInsertMode[id]) {
        //Inserting the original event, reclaim this one.
        sender->reclaimEvent(event);
    } else {
        OutputManagerImplementationBase::insert(event);
    }

    handleCancelEvents(sender);
    return retval;
}

void LazyOutputManager::setCompareMode(SimulationObject* obj, bool mode) {
    int id = obj->getObjectID()->getSimulationObjectID();
    compareAndInsertMode[id] = mode;
}

bool LazyOutputManager::getCompareMode(SimulationObject* obj) {
    int id = obj->getObjectID()->getSimulationObjectID();
    return compareAndInsertMode[id];
}

void LazyOutputManager::rollback(SimulationObject* object, const VTime& rollbackTime) {

    //Put the events that have a timestamp greater than rollback event's
    //timestamp into the Lazy Cancellation Queue to be compared to regenerated output.
    OutputEvents& outputEvents = getOutputEventsFor(*(object->getObjectID()));
    vector<const Event*>* tempOutEvents = outputEvents.getEventsSentAtOrAfterAndRemove(rollbackTime);
    vector<const Event*>* lazyCancelEvents = lazyQueues[object->getObjectID()->getSimulationObjectID()];

    //These output events need to be added to the lazy cancel queue. There may already be
    //events in the queue, so the new ones need to be added.
    lazyCancelEvents->insert(lazyCancelEvents->end(), tempOutEvents->begin(), tempOutEvents->end());
    std::sort(lazyCancelEvents->begin(), lazyCancelEvents->end(), receiveTimeLessThan());

//  outputEvents.remove(*tempOutEvents);

    delete tempOutEvents;
}

void LazyOutputManager::ofcPurge() {
    // Reset all queues and values.
    for (int i = 0; i < getSimulationManager()->getNumberOfSimulationObjects(); i++) {
        lazyQueues[i]->clear();
        eventsToCancel[i]->clear();
        compareAndInsertMode[i] = true;
        lazyHitCount[i] = 0;
        lazyMissCount[i] = 0;
    }

    OutputManagerImplementationBase::ofcPurge();
}
