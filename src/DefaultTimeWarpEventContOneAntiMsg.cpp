
#include <deque>                        // for _Deque_iterator, deque, etc
#include <vector>                       // for vector

#include "DefaultTimeWarpEventContOneAntiMsg.h"
#include "Event.h"                      // for Event
#include "NegativeEvent.h"              // for NegativeEvent
#include "ObjectID.h"                   // for ObjectID
#include "VTime.h"                      // for VTime
#include "warped.h"                     // for ASSERT

class SimulationObject;

bool
DefaultTimeWarpEventContOneAntiMsg::handleAntiMessage(SimulationObject* reclaimer,
                                                      const NegativeEvent* eventToRemove) {

    ASSERT(reclaimer != 0);
    bool foundMatch = false;
    deque<const Event*>::iterator it = unprocessedEvents.begin();

    while (it != unprocessedEvents.end()) {
        if ((*it)->getSender() == eventToRemove->getSender() &&
                (*it)->getSendTime() >= eventToRemove->getSendTime()) {
            removedEvents.push_back(*it);
            it = unprocessedEvents.erase(it);
            foundMatch = true;
        } else {
            it++;
        }
    }

    return foundMatch;
}
