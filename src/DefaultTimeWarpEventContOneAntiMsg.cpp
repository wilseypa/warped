
#include "DefaultTimeWarpEventContOneAntiMsg.h"
#include "NegativeEvent.h"

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
