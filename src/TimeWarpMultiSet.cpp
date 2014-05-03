#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, endl, etc

#include "Event.h"                      // for Event, operator<<, etc
#include "EventId.h"                    // for EventId
#include "NegativeEvent.h"              // for NegativeEvent
#include "ObjectID.h"                   // for ObjectID, operator<<
#include "SimulationObject.h"           // for SimulationObject
#include "TimeWarpMultiSet.h"
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "VTime.h"                      // for VTime
#include "warped.h"                     // for ASSERT

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

TimeWarpMultiSet::TimeWarpMultiSet(TimeWarpSimulationManager* initSimulationManager)
    :myNumCommittedEvents(0)
    ,myNumRolledBackEvents(0)
    ,myNumExecutedEvents(0) {
    for (int i = 0; i < initSimulationManager->getNumberOfSimulationObjects(); i++) {
        multiset<const Event*, receiveTimeLessThanEventIdLessThan>* objSet =
            new multiset<const Event*, receiveTimeLessThanEventIdLessThan>;

        unprocessedObjEvents.push_back(objSet);
        lowObjPos.push_back(lowestObjEvents.end());
        insertObjPos.push_back(objSet->begin());
        processedObjEvents.push_back(new vector<const Event*>);
        removedEvents.push_back(new vector<const Event*>);
    }

    mySimulationManager = initSimulationManager;
}

TimeWarpMultiSet::~TimeWarpMultiSet() {
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>* toDeleteMultiSet = NULL;

    while (!unprocessedObjEvents.empty()) {
        toDeleteMultiSet = unprocessedObjEvents.back();
        unprocessedObjEvents.pop_back();
        delete toDeleteMultiSet;
    }

    vector<const Event*>* toDeleteVector = NULL;

    while (!processedObjEvents.empty()) {
        toDeleteVector = processedObjEvents.back();
        processedObjEvents.pop_back();
        delete toDeleteVector;
    }

    while (!removedEvents.empty()) {
        toDeleteVector = removedEvents.back();
        removedEvents.pop_back();
        delete toDeleteVector;
    }
}

bool
TimeWarpMultiSet::insert(const Event* event) {
    ASSERT(event != NULL);
    unsigned int objId = event->getReceiver().getSimulationObjectID();

    // Insert the event into the object's multiset. The iterator for the event
    // inserted is returned and will be used for future insertions to increase efficiency.
    insertObjPos[objId] = unprocessedObjEvents[objId]->insert(insertObjPos[objId], event);

    // The event was just inserted at the beginning, so update the lowest
    // object event set.
    if (*event == **(unprocessedObjEvents[objId]->begin())) {
        // Do not erase the first time.
        if (lowObjPos[objId] != lowestObjEvents.end()) {
            lowestObjEvents.erase(lowObjPos[objId]);
        }

        lowObjPos[objId] = lowestObjEvents.insert(event);
    }

    // Returns false because the event is not in the object's past.
    return false;
}

bool
TimeWarpMultiSet::handleAntiMessage(SimulationObject* object, const NegativeEvent* event) {

    ASSERT(event != NULL);
    ASSERT(object != NULL);

    unsigned int objId = object->getObjectID()->getSimulationObjectID();

    bool eventWasRemoved = false;
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator it;
    it = unprocessedObjEvents[objId]->begin();

    while (it != unprocessedObjEvents[objId]->end() && !eventWasRemoved) {
        if ((*it)->getSender() == event->getSender() &&
                (*it)->getEventId() == event->getEventId()) {

            const Event* eventToRemove = *it;

            // Update the insertion iterator to point to the event preceding the
            // event to be removed. If the event is at the beginning, set the insertion
            // iterator to the beginning.
            if (it == insertObjPos[objId]) {
                if (insertObjPos[objId] == unprocessedObjEvents[objId]->begin()) {
                    insertObjPos[objId]++;
                } else {
                    insertObjPos[objId]--;
                }
            }
            unprocessedObjEvents[objId]->erase(it);

            // Update the lowest object event if it was just removed.
            if (lowObjPos[objId] != lowestObjEvents.end()) {
                if (*eventToRemove == **(lowObjPos[objId])) {
                    lowestObjEvents.erase(lowObjPos[objId]);
                    if (!(unprocessedObjEvents[objId]->empty())) {
                        lowObjPos[objId] = lowestObjEvents.insert(*(unprocessedObjEvents[objId]->begin()));
                    } else {
                        lowObjPos[objId] = lowestObjEvents.end();
                    }
                }
            }

            // Put the removed event here in case it needs to be used for comparisons in
            // lazy cancellation.
            removedEvents[objId]->push_back(eventToRemove);
            eventWasRemoved = true;
        } else {
            it++;
        }
    }

    return eventWasRemoved;
}

const Event*
TimeWarpMultiSet::getEvent(SimulationObject* object) {
    const Event* retval = NULL;

    unsigned int objId = object->getObjectID()->getSimulationObjectID();

    if (!(unprocessedObjEvents[objId]->empty())) {
        // Get the lowest timestamp event for the object.
        retval = *(unprocessedObjEvents[objId]->begin());
        bool updateInsert = false;

        if (*retval == **(insertObjPos[objId])) {
            updateInsert = true;
        }

        unprocessedObjEvents[objId]->erase(unprocessedObjEvents[objId]->begin());

        // Update the insert position iterator.
        if (updateInsert) {
            insertObjPos[objId] = unprocessedObjEvents[objId]->begin();
        }

        // Remove from unprocessed queue and put in processed queue.
        processedObjEvents[objId]->push_back(retval);
        myNumExecutedEvents++;

        // Update the lowest object set.
        if (lowObjPos[objId] != lowestObjEvents.end()) {
            lowestObjEvents.erase(lowObjPos[objId]);
        }

        if (!(unprocessedObjEvents[objId]->empty())) {
            lowObjPos[objId] = lowestObjEvents.insert(*(unprocessedObjEvents[objId]->begin()));
        } else {
            lowObjPos[objId] = lowestObjEvents.end();
        }
    } else {
        insertObjPos[objId] = unprocessedObjEvents[objId]->begin();
    }

    return retval;
}

const Event*
TimeWarpMultiSet::getEvent(SimulationObject* object, const VTime& minimumTime) {
    const Event* retval = NULL;
    ASSERT(object != NULL);
    const Event* peeked = peekEvent(object, minimumTime);
    if (peeked != NULL) {
        retval = getEvent(object);
        ASSERT(peeked == retval);
    }
    return retval;
}

const Event*
TimeWarpMultiSet::peekEvent(SimulationObject* object) {

    const Event* retval = NULL;

    // If no object is designated, then pick the lowest event available.
    if (object == NULL && lowestObjEvents.begin() != lowestObjEvents.end()) {
        retval = *(lowestObjEvents.begin());
    } else if (object != NULL) {
        // Otherwise, just take the lowest event from the object's multiset.
        unsigned int objId = object->getObjectID()->getSimulationObjectID();
        if (!(unprocessedObjEvents[objId]->empty())) {
            retval = *(unprocessedObjEvents[objId]->begin());
        }
    }

    return retval;
}

const Event*
TimeWarpMultiSet::peekEvent(SimulationObject* object, const VTime& minimumTime) {
    const Event* retval = peekEvent(object);
    if (retval != NULL) {
        if (!(retval->getReceiveTime() < minimumTime)) {
            retval = NULL;
        }
    }
    return retval;
}

void
TimeWarpMultiSet::fossilCollect(SimulationObject* object,
                                const VTime& fossilCollectTime) {
    unsigned int objId = object->getObjectID()->getSimulationObjectID();

    // Removed the processed events with time less than the collect time.
    vector<const Event*>::iterator it = processedObjEvents[objId]->begin();
    while (it != processedObjEvents[objId]->end() && (*it)->getReceiveTime() < fossilCollectTime) {
        object->reclaimEvent(*it);
        it++;
    }

    myNumCommittedEvents += it - processedObjEvents[objId]->begin();
    processedObjEvents[objId]->erase(processedObjEvents[objId]->begin(), it);

    // Also remove the processed events that have been removed.
    it = removedEvents[objId]->begin();
    while (it != removedEvents[objId]->end()) {
        if ((*it)->getReceiveTime() < fossilCollectTime) {
            const Event* eventToReclaim = *it;
            it = removedEvents[objId]->erase(it);
            object->reclaimEvent(eventToReclaim);
        } else {
            it++;
        }
    }
}

void
TimeWarpMultiSet::fossilCollect(SimulationObject* object,
                                int fossilCollectTime) {
    unsigned int objId = object->getObjectID()->getSimulationObjectID();

    // Removed the processed events with time less than the collect time.
    vector<const Event*>::iterator it = processedObjEvents[objId]->begin();
    while (it != processedObjEvents[objId]->end() &&
            (*it)->getReceiveTime().getApproximateIntTime() < fossilCollectTime) {
        object->reclaimEvent(*it);
        it++;
    }
    processedObjEvents[objId]->erase(processedObjEvents[objId]->begin(), it);

    // Also remove the processed events that have been removed.
    it = removedEvents[objId]->begin();
    while (it != removedEvents[objId]->end()) {
        if ((*it)->getReceiveTime().getApproximateIntTime() < fossilCollectTime) {
            const Event* eventToReclaim = *it;
            it = removedEvents[objId]->erase(it);
            object->reclaimEvent(eventToReclaim);
        } else {
            it++;
        }
    }
}

void
TimeWarpMultiSet::fossilCollect(const Event* toRemove) {
    unsigned int objId = toRemove->getReceiver().getSimulationObjectID();
    bool foundMatch = false;

    vector<const Event*>::iterator it = processedObjEvents[objId]->begin();
    while (it != processedObjEvents[objId]->end()) {
        if ((*it)->getEventId() == toRemove->getEventId() &&
                (*it)->getSender() == toRemove->getSender()) {
            processedObjEvents[objId]->erase(it);
            foundMatch = true;
            break;
        } else {
            it++;
        }
    }

    if (!foundMatch) {
        it = removedEvents[objId]->begin();
        while (it != removedEvents[objId]->end()) {
            if ((*it)->getEventId() == toRemove->getEventId() &&
                    (*it)->getSender() == toRemove->getSender()) {
                it = removedEvents[objId]->erase(it);
                foundMatch = true;
                break;
            } else {
                it++;
            }
        }
    }
}

void
TimeWarpMultiSet::rollback(SimulationObject* object,
                           const VTime& rollbackTime) {

    unsigned int objId = object->getObjectID()->getSimulationObjectID();
    vector<const Event*>::iterator it = processedObjEvents[objId]->begin();

    // Go through the entire processed events queue and put any events with
    // a receive time greater than or equal to the rollback time back in the
    // unprocessed queue
    while (it != processedObjEvents[objId]->end() && (*it)->getReceiveTime() < rollbackTime) {
        it++;
    }

    myNumRolledBackEvents += processedObjEvents[objId]->end() - it;

    unprocessedObjEvents[objId]->insert(it, processedObjEvents[objId]->end());
    processedObjEvents[objId]->erase(it, processedObjEvents[objId]->end());

    // Update the low object event.
    if (lowObjPos[objId] != lowestObjEvents.end()) {
        if (!(**(lowObjPos[objId]) == **(unprocessedObjEvents[objId]->begin()))) {
            lowestObjEvents.erase(lowObjPos[objId]);
            if (!(unprocessedObjEvents[objId]->empty())) {
                lowObjPos[objId] = lowestObjEvents.insert(*(unprocessedObjEvents[objId]->begin()));
            }
        }
    } else if (!unprocessedObjEvents[objId]->empty()) {
        lowObjPos[objId] = lowestObjEvents.insert(*(unprocessedObjEvents[objId]->begin()));
    }

    // After a rollback, the first event to be inserted should be at the rollback time, so just
    // put the iterator at the beginning.
    insertObjPos[objId] = unprocessedObjEvents[objId]->begin();
}

void
TimeWarpMultiSet::ofcPurge() {
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator msit;

    lowestObjEvents.clear();
    for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
        msit = unprocessedObjEvents[i]->begin();
        while (msit != unprocessedObjEvents[i]->end()) {
            (*msit)->~Event();
            ::operator delete((void*)(*msit));
            unprocessedObjEvents[i]->erase(msit++);
        }
        lowObjPos[i] = lowestObjEvents.end();
        insertObjPos[i] = unprocessedObjEvents[i]->end();
    }

    for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
        vector<const Event*>::iterator ip = processedObjEvents[i]->begin();
        while (ip != processedObjEvents[i]->end()) {
            (*ip)->~Event();
            ::operator delete((void*)(*ip));
            ip++;
        }
        processedObjEvents[i]->clear();
    }

    for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
        vector<const Event*>::iterator ir = removedEvents[i]->begin();
        while (ir != removedEvents[i]->end()) {
            (*ir)->~Event();
            ::operator delete((void*)(*ir));
            ir++;
        }
        removedEvents[i]->clear();
    }
}

void
TimeWarpMultiSet::print(std::ostream& out) {
    // Print the processed queues.
    for (int i = 0; i < processedObjEvents.size(); i++) {
        vector<const Event*>::iterator it = processedObjEvents[i]->begin();

        if (it != processedObjEvents[i]->end()) {
            out << "Processed Queue of " << (*(processedObjEvents[i]->begin()))->getReceiver() << endl;

            while (it != processedObjEvents[i]->end()) {
                out << **it << endl;
                it++;
            }
        }
    }

    // Print the unprocessed queues.
    for (int i = 0; i < unprocessedObjEvents.size(); i++) {
        multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator it;
        it = unprocessedObjEvents[i]->begin();

        if (it != unprocessedObjEvents[i]->end()) {
            out << "Unprocessed Queue of " << (*(unprocessedObjEvents[i]->begin()))->getReceiver() << endl;

            while (it != unprocessedObjEvents[i]->end()) {
                out << **it << endl;
                it++;
            }
        }

        // Print the current low event for each object in the lowest object set.
        if (lowObjPos[i] != lowestObjEvents.end()) {
            out << "Low Object Position: " << **(lowObjPos[i]) << endl;
        }

        // Print the current low event for each object in the lowest object set.
        if (insertObjPos[i] != unprocessedObjEvents[i]->end()) {
            out << "Insert Position: " << **(insertObjPos[i]) << endl;
        }
    }
}
