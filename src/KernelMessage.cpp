
#include "KernelMessage.h"
#include "SerializedInstance.h"         // for SerializedInstance

void
KernelMessage::serialize(SerializedInstance* addTo) const {
    addTo->addUnsigned(getSender());
    addTo->addUnsigned(getReceiver());
    addTo->addUnsigned(incarnationNumber);
}
