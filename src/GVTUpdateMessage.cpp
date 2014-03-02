
#include "DeserializerManager.h"        // for DeserializerManager
#include "GVTUpdateMessage.h"
#include "SerializedInstance.h"         // for SerializedInstance

void
GVTUpdateMessage::serialize(SerializedInstance* addTo) const {
    KernelMessage::serialize(addTo);
    addTo->addSerializable(newGVT);
}

Serializable*
GVTUpdateMessage::deserialize(SerializedInstance* data) {
    unsigned int source = data->getUnsigned();
    unsigned int dest = data->getUnsigned();
    unsigned int incNum = data->getUnsigned();
    VTime* newGVT = dynamic_cast<VTime*>(data->getSerializable());

    GVTUpdateMessage* retval = new GVTUpdateMessage(source, dest, *newGVT);
    retval->setIncarnationNumber(incNum);
    delete newGVT;

    return retval;
}

const string&
GVTUpdateMessage::getGVTUpdateMessageType() {
    static const string gVTUpdateMessageType = "GVTUpdateMessageType";
    return gVTUpdateMessageType;
}


void GVTUpdateMessage::registerDeserializer() {
    DeserializerManager::instance()->registerDeserializer(getGVTUpdateMessageType(),
                                                          &deserialize);

}
