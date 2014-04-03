
#include "CirculateInitializationMessage.h"
#include "DeserializerManager.h"        // for DeserializerManager
#include "SerializedInstance.h"         // for SerializedInstance, string

class Serializable;

void
CirculateInitializationMessage::serialize(SerializedInstance* addTo) const {
    KernelMessage::serialize(addTo);
}

Serializable*
CirculateInitializationMessage::deserialize(SerializedInstance* data) {
    unsigned int sender = data->getUnsigned();
    unsigned int dest = data->getUnsigned();
    unsigned int incNum = data->getUnsigned();

    CirculateInitializationMessage* retval = new CirculateInitializationMessage(sender, dest);
    retval->setIncarnationNumber(incNum);
    return retval;
}

const string&
CirculateInitializationMessage::getCirculateInitializationMessageType() {
    static const string circulateInitializationMessageType = "CirculateInitializationMessage";
    return circulateInitializationMessageType;
}

void
CirculateInitializationMessage::registerDeserializer() {
    DeserializerManager::instance()->registerDeserializer(getCirculateInitializationMessageType(),
                                                          &deserialize);
}
