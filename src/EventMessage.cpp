
#include "DeserializerManager.h"        // for DeserializerManager
#include "Event.h"                      // for Event
#include "EventMessage.h"
#include "SerializedInstance.h"         // for SerializedInstance

Serializable*
EventMessage::deserialize(SerializedInstance* data) {
    unsigned int source = data->getUnsigned();
    unsigned int dest = data->getUnsigned();
    unsigned int incNum = data->getUnsigned();
    Event* event = dynamic_cast<Event*>(data->getSerializable());
    const string gvtInfo = data->getString();

    EventMessage* retval = new EventMessage(source, dest, event, gvtInfo);
    retval->setIncarnationNumber(incNum);
    return retval;
}

const string&
EventMessage::getEventMessageType() {
    static string eventMessageType = "EventMessage";
    return eventMessageType;
}

void
EventMessage::registerDeserializer() {
    DeserializerManager::instance()->registerDeserializer(getEventMessageType(),
                                                          &EventMessage::deserialize);
}
