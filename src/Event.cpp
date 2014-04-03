
#include <ostream>                      // for operator<<, ostream, etc

#include "Event.h"
#include "EventId.h"                    // for EventId, operator<<
#include "ObjectID.h"                   // for ObjectID, operator<<
#include "SerializedInstance.h"         // for SerializedInstance
#include "VTime.h"                      // for VTime, operator<<

Event::~Event() {}

std::ostream&
operator<<(std::ostream& os, const Event& event) {
    os << "sender: " << event.getSender()
       << " receiver: " << event.getReceiver()
       << " sendTime: " << event.getSendTime()
       << " receiveTime: " << event.getReceiveTime()
       << " eventId: " << event.getEventId();
    return (os);
}

bool
operator==(const Event& eve1, const Event& eve2) {
    // NOTE: This is used to uniquely identify events for use
    //       in STL containers. Do not use this for comparing
    //       events in lazy cancellation or similar areas. Use
    //       compareEvents instead.
    bool retval = eve1.getSender() == eve2.getSender() &&
                  eve1.getEventId() == eve2.getEventId();
    return retval;
}

bool
Event::compareEvents(const Event* a, const Event* b) {
    bool retval = true;

    if (a->getSender() != b->getSender() ||
            a->getSendTime() != b->getSendTime() ||
            a->getReceiver() != a->getReceiver() ||
            a->getReceiveTime() != b->getReceiveTime()) {
        retval = false;
    }

    return retval;
}

void
Event::serialize(SerializedInstance* addTo) const {
    addTo->addSerializable(&getSendTime());
    addTo->addSerializable(&getReceiveTime());
    addTo->addUnsigned(getSender().getSimulationManagerID());
    addTo->addUnsigned(getSender().getSimulationObjectID());
    addTo->addUnsigned(getReceiver().getSimulationManagerID());
    addTo->addUnsigned(getReceiver().getSimulationObjectID());
    addTo->addUnsigned(getEventId().val);
}