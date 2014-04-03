
#include "Event.h"                      // for Event
#include "EventMessageBase.h"
#include "SerializedInstance.h"         // for SerializedInstance

void
EventMessageBase::serialize(SerializedInstance* addTo) const {
    KernelMessage::serialize(addTo);
    addTo->addSerializable(event);
    addTo->addString(gVTInfo);
}
