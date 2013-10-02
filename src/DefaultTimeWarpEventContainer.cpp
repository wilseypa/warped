
#include "DefaultTimeWarpEventContainer.h"
#include "Event.h"
#include "NegativeEvent.h"
#include "EventFunctors.h"
#include "SimulationObject.h"
#include "TimeWarpSimulationManager.h"
#include "iostream"
using std::cout;
using std::endl;

class reclaimEvent: public std::unary_function<const Event*, void> {
public:
    reclaimEvent(SimulationObject* initObject, const VTime& initCollectTime) :
        reclaimer(initObject), collectTime(initCollectTime) {
    }

    void operator()(const Event*& toReclaim) {
        if (toReclaim->getReceiveTime() < collectTime) {
            reclaimer->reclaimEvent(toReclaim);
            toReclaim = 0;
        }
    }

private:
    SimulationObject* reclaimer;
    const VTime& collectTime;
};

class reclaimEventInt: public std::unary_function<const Event*, void> {
public:
    reclaimEventInt(SimulationObject* initObject, int initCollectTime) :
        reclaimer(initObject), collectTime(initCollectTime) {
    }

    void operator()(const Event*& toReclaim) {
        if (toReclaim->getReceiveTime().getApproximateIntTime() < collectTime) {
            reclaimer->reclaimEvent(toReclaim);
            toReclaim = 0;
        }
    }

private:
    SimulationObject* reclaimer;
    int collectTime;
};

class reclaimEventId: public std::unary_function<const EventId, void> {
public:

    reclaimEventId(SimulationObject* initObject, const EventId initReclaimId) :
        reclaimer(initObject), reclaimId(initReclaimId) {
    }

    void operator()(const Event*& toCheck) {
        if (toCheck->getEventId() == reclaimId) {
            reclaimer->reclaimEvent(toCheck);
            toCheck = 0;
        }
    }

private:
    SimulationObject* reclaimer;
    const EventId reclaimId;
};

void DefaultTimeWarpEventContainer::insert(const Event* newEvent) {
    if (negativeEvents.empty()) {
        unprocessedEvents.push_back(newEvent);
        sortStatus = UNSORTED;
    } else {
        bool wasCancelled=false;
        deque<const NegativeEvent*>::iterator it = negativeEvents.begin();
        while (it != negativeEvents.end()) {
            if (newEvent->getEventId() == (*it)->getEventId()
                    && newEvent->getSender() == (*it)->getSender()) {
                removedEvents.push_back(newEvent);
                negativeEvents.erase(it);
                wasCancelled = true;
                break;
            } else {
                it++;
            }
        }
        //If this new event was not to be cancelled, add it to the queue as normal
        if (!wasCancelled) {
            unprocessedEvents.push_back(newEvent);
            sortStatus = UNSORTED;
        }
    }
}

//Event ID's are unique only to the sender
//so all of the other event info must be checked as well.
//The other event info will match if an event is regenerated
//after a rollback which will give it a different eventID
//So both need to be checked
bool DefaultTimeWarpEventContainer::handleAntiMessage(SimulationObject* reclaimer,
                                                      const NegativeEvent* eventToRemove) {
    ASSERT(reclaimer != 0);
    bool foundMatch = false;
    deque<const Event*>::iterator it = unprocessedEvents.begin();
    while (it != unprocessedEvents.end()) {
        if (eventToRemove->getEventId() == (*it)->getEventId()
                && eventToRemove->getSender() == (*it)->getSender()) {
            removedEvents.push_back(*it);
            unprocessedEvents.erase(it);
            foundMatch = true;
            break;
        } else {
            it++;
        }
    }
    //If we failed to find the event, it is not here yet
    //so put it in this queue so we can catch it when it is inserted
    if (!foundMatch) {
        negativeEvents.push_back(eventToRemove);
    }
    return foundMatch;
}

void DefaultTimeWarpEventContainer::rollback(const VTime& rollbackTime) {
    vector<const Event*>::iterator newEnd = processedEvents.begin();
    while (newEnd != processedEvents.end() && (*newEnd)->getReceiveTime() < rollbackTime) {
        newEnd++;
    }

    unprocessedEvents.insert(unprocessedEvents.end(), newEnd, processedEvents.end());
    processedEvents.erase(newEnd, processedEvents.end());
    sortStatus = UNSORTED;
}

void DefaultTimeWarpEventContainer::debugDump(std::ostream& os) const {
    os << "-------------------------\n";
    os << "EventContainer::debugDump\n";
    os << "Processed Events: ";
    for (vector<const Event*>::const_iterator i = processedEvents.begin();
            i < processedEvents.end(); i++) {
        os << (*i)->getEventId() << " ";
    }
    os << "\n";
    os << "sortStatus: " << sortStatus << "\n";
    os << "Unprocessed Events: ";
    for (deque<const Event*>::const_iterator i = unprocessedEvents.begin();
            i < unprocessedEvents.end(); i++) {
        os << (*i)->getEventId() << " ";
    }
    os << "\n";
    os << "Removed Events: ";
    for (vector<const Event*>::const_iterator i = removedEvents.begin();
            i < removedEvents.end(); i++) {
        os << (*i)->getEventId() << " ";
    }
    os << "-------------------------" << endl;
}

void DefaultTimeWarpEventContainer::fossilCollect(SimulationObject* object,
                                                  const VTime& collectTime) {
    if (sortStatus == UNSORTED) {
        sortByReceiveTimes();
    }

    vector<const Event*>::iterator it = processedEvents.begin();
    while (it != processedEvents.end() && (*it)->getReceiveTime() < collectTime) {
        object->reclaimEvent(*it);
        it++;
    }
    processedEvents.erase(processedEvents.begin(), it);

    // Fossil collect the removed events. Remove any events that may have already been
    // reclaimed first.
    removedEvents.erase(std::remove(removedEvents.begin(), removedEvents.end(),
                                    static_cast<Event*>(0)),
                        removedEvents.end());
    for_each(removedEvents.begin(), removedEvents.end(), reclaimEvent(object, collectTime));
    removedEvents.erase(std::remove(removedEvents.begin(), removedEvents.end(),
                                    static_cast<Event*>(0)),
                        removedEvents.end());
}

void
DefaultTimeWarpEventContainer::fossilCollect(SimulationObject* object,
                                             int collectTime) {
    if (sortStatus == UNSORTED) {
        sortByReceiveTimes();
    }

    vector<const Event*>::iterator it = processedEvents.begin();
    while (it != processedEvents.end()
            && (*it)->getReceiveTime().getApproximateIntTime() < collectTime) {
        object->reclaimEvent(*it);
        it++;
    }
    processedEvents.erase(processedEvents.begin(), it);

    // Fossil collect the removed events. Remove any events that may have already been
    // reclaimed first.
    removedEvents.erase(std::remove(removedEvents.begin(), removedEvents.end(),
                                    static_cast<Event*>(0)),
                        removedEvents.end());
    for_each(removedEvents.begin(), removedEvents.end(), reclaimEventInt(object, collectTime));
    removedEvents.erase(std::remove(removedEvents.begin(), removedEvents.end(),
                                    static_cast<Event*>(0)),
                        removedEvents.end());
}

void
DefaultTimeWarpEventContainer::fossilCollect(const Event* toRemove) {
    bool found = false;
    vector<const Event*>::iterator it = processedEvents.begin();
    while (it != processedEvents.end()) {
        if (toRemove->getEventId() == (*it)->getEventId() && toRemove->getSender() == (*it)->getSender()) {
            processedEvents.erase(it);
            found = true;
            break;
        } else {
            it++;
        }
    }

    it = removedEvents.begin();
    while (it != removedEvents.end() && !found) {
        if (toRemove->getEventId() == (*it)->getEventId() && toRemove->getSender() == (*it)->getSender()) {
            removedEvents.erase(it);
            found = true;
            break;
        } else {
            it++;
        }
    }
}

const Event*
DefaultTimeWarpEventContainer::nextEvent() {
    const Event* retval = 0;
    if (!unprocessedEvents.empty()) {
        if (sortStatus == UNSORTED) {
            sortByReceiveTimes();
        }
        retval = unprocessedEvents.front();
    }
    return retval;
}

const Event*
DefaultTimeWarpEventContainer::getNextEvent() {
    const Event* retval = nextEvent();
    if (retval != 0) {
        unprocessedEvents.pop_front();
        processedEvents.push_back(retval);
    }
    return retval;
}

bool
DefaultTimeWarpEventContainer::isInThePast(const Event* toCheck) {
    bool retval = false;
    if (!processedEvents.empty()) {
        // Events are pushed on to the back of the processed events vector.
        // Because they can only be pushed on in order, the vector is always sorted.
        const Event* lastProc = processedEvents.back();

        if (lastProc != NULL) {
            if (toCheck->getReceiveTime() != lastProc->getReceiveTime()) {
                retval = toCheck->getReceiveTime() < lastProc->getReceiveTime();
            } else {
                if (toCheck->getEventId() != lastProc->getEventId()) {
                    retval = toCheck->getEventId() < lastProc->getEventId();
                } else {
                    retval = toCheck->getSender() < lastProc->getSender();
                }
            }
        }
    }
    return retval;
}

void
DefaultTimeWarpEventContainer::ofcPurge(SimulationObject* obj) {

    deque<const Event*>::iterator iunp = unprocessedEvents.begin();
    while (iunp != unprocessedEvents.end()) {
        obj->reclaimEvent(*iunp);
        iunp++;
    }
    unprocessedEvents.clear();

    vector<const Event*>::iterator ip = processedEvents.begin();
    while (ip != processedEvents.end()) {
        obj->reclaimEvent(*ip);
        ip++;
    }
    processedEvents.clear();

    vector<const Event*>::iterator ir = removedEvents.begin();
    while (ir != removedEvents.end()) {
        obj->reclaimEvent(*ir);
        ir++;
    }
    removedEvents.clear();

    deque<const NegativeEvent*>::iterator in = negativeEvents.begin();
    while (in != negativeEvents.end()) {
        obj->reclaimEvent(*in);
        in++;
    }
    negativeEvents.clear();
}
