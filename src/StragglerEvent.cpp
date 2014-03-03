
#include <ostream>                      // for operator<<, ostream, etc
#include <vector>

#include "EventId.h"                    // for operator<<, EventId
#include "ObjectID.h"                   // for operator<<, ObjectID
#include "StragglerEvent.h"
#include "VTime.h"                      // for operator<<

std::ostream&
operator<<(std::ostream& os, const StragglerEvent& event) {
    os << "sender: " << event.getSender() << " receiver: "
       << event.getReceiver() << " sendTime: " << event.getSendTime()
       << " receiveTime: " << event.getReceiveTime() << " eventId: "
       << event.getEventId();
    return (os);
}

bool StragglerEvent::eventCompare(const Event* a) {
    bool retval = true;

    if (a->getSender() != this->getSender() || a->getEventId()
            != this->getEventId()) {
        retval = false;
    }

    return retval;
}

const Event* StragglerEvent::getPositiveEvent() const {
    return positiveEvent;
}

bool StragglerEvent::getStragglerType() const {
    return stragglerType;
}

void StragglerEvent::setStragglerType(bool stragglerType) {
    this->stragglerType = stragglerType;
}

const std::vector<const NegativeEvent*> StragglerEvent::getEventsToCancel() const {
    return eventsToCancel;
}




