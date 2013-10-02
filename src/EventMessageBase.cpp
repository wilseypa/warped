
#include "EventMessageBase.h"
#include "Event.h"

void
EventMessageBase::serialize(SerializedInstance* addTo) const {
    KernelMessage::serialize(addTo);
    addTo->addSerializable(event);
    addTo->addString(gVTInfo);
}
