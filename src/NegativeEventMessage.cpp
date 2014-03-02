
#include "DeserializerManager.h"        // for DeserializerManager
#include "NegativeEvent.h"              // for NegativeEvent
#include "NegativeEventMessage.h"
#include "SerializedInstance.h"         // for SerializedInstance

NegativeEventMessage::~NegativeEventMessage() {
    vector<const NegativeEvent*>::const_iterator it(myEvents.begin());
    while (it != myEvents.end())
    { delete(*it++); }
}

void
NegativeEventMessage::serialize(SerializedInstance* serializeInto) const {
    KernelMessage::serialize(serializeInto);
    serializeInto->addUnsigned(myEvents.size());
    for (int i = 0; i < myEvents.size(); i++) {
        serializeInto->addSerializable(myEvents[i]);
    }
    serializeInto->addString(gVTInfo);
}

Serializable*
NegativeEventMessage::deserialize(SerializedInstance* data) {
    unsigned int source = data->getUnsigned();
    unsigned int dest = data->getUnsigned();
    unsigned int incNum = data->getUnsigned();
    unsigned int numEvents = data->getUnsigned();

    vector<const NegativeEvent*> events;
    for (int i = 1; i <= numEvents; i++) {
        events.push_back(dynamic_cast<NegativeEvent*>(data->getSerializable()));
    }
    string gvtIn = data->getString();

    NegativeEventMessage* retval =  new NegativeEventMessage(source,
                                                             dest,
                                                             events,
                                                             gvtIn);
    retval->setIncarnationNumber(incNum);
    return retval;
}

const string&
NegativeEventMessage::getNegativeEventMessageType() {
    static string negativeEventMessageType = "NegativeEventMessage";
    return negativeEventMessageType;
}

void
NegativeEventMessage::registerDeserializer() {
    DeserializerManager::instance()->registerDeserializer(getNegativeEventMessageType(),
                                                          &NegativeEventMessage::deserialize);
}

