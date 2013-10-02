
#include "InitializationMessage.h"
#include "DeserializerManager.h"

void
InitializationMessage::serialize(SerializedInstance* addTo) const {
    KernelMessage::serialize(addTo);
    addTo->addInt(objectNames.size());
    for (unsigned int i = 0; i < objectNames.size(); i++) {
        addTo->addString(objectNames[i]);
    }
    addTo->addUnsigned(numSimulationManagers);
}

const vector<string>
InitializationMessage::deserializeStringVector(SerializedInstance* data) {
    vector<string> retval;
    int numStrings = data->getInt();
    for (int i = 0; i < numStrings; i++) {
        retval.push_back(data->getString());
    }
    return retval;
}

Serializable*
InitializationMessage::deserialize(SerializedInstance* data) {
    unsigned int source = data->getUnsigned();
    unsigned int dest = data->getUnsigned();
    unsigned int incNum = data->getUnsigned();
    const vector<string>& objectNames = deserializeStringVector(data);
    unsigned int numLPs = data->getUnsigned();

    InitializationMessage* retval = new InitializationMessage(source,
                                                              dest,
                                                              objectNames,
                                                              numLPs);
    retval->setIncarnationNumber(incNum);

    return retval;
}

const string&
InitializationMessage::getInitializationMessageType() {
    static const string initializationMessage = "InitializationMessage";
    return initializationMessage;
}

void
InitializationMessage::registerDeserializer() {
    DeserializerManager::instance()->registerDeserializer(getInitializationMessageType(),
                                                          &deserialize);
}
