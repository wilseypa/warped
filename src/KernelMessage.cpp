
#include "KernelMessage.h"

void
KernelMessage::serialize(SerializedInstance* addTo) const {
    addTo->addUnsigned(getSender());
    addTo->addUnsigned(getReceiver());
    addTo->addUnsigned(incarnationNumber);
}
