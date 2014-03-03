#ifndef NEGATIVE_EVENT_MESSAGE_H
#define NEGATIVE_EVENT_MESSAGE_H


/**
   This is the class defining the termination token that gets passed
   around.  Essentially, it has a color and a terminator.

   The color can be WHITE, which means it's on the first cycle,
   RED, which means it's on it's second cycle, or BLACK.
*/

#include <string>                       // for string, allocator
#include <vector>                       // for vector

#include "DeserializerManager.h"        // for string
#include "EventId.h"
#include "KernelMessage.h"              // for KernelMessage
#include "NegativeEvent.h"
#include "Serializable.h"               // for string, etc

class Serializable;
class NegativeEvent;
class SerializedInstance;

/**
   This class represents the token that gets passed between
   TokenPassingTerminationManager instances.

   @see TokenPassingTerminationManager
*/

class NegativeEventMessage : public KernelMessage {
public:
    NegativeEventMessage(unsigned int source,
                         unsigned int dest,
                         const std::vector<const NegativeEvent*>& events,
                         const std::string& initGVTInfo):
        KernelMessage(source, dest),
        myEvents(events),
        gVTInfo(initGVTInfo) {}

    ~NegativeEventMessage();

    static Serializable* deserialize(SerializedInstance* data);
    static const std::string& getNegativeEventMessageType();

    void serialize(SerializedInstance*) const;

    const std::string& getDataType() const {
        return getNegativeEventMessageType();
    }

    static void registerDeserializer();

    const std::vector<const NegativeEvent*>& getEvents() { return myEvents; }

    const std::string getGVTInfo() {
        return gVTInfo;
    }

private:
    std::vector<const NegativeEvent*> myEvents;
    const std::string gVTInfo;
};

#endif
