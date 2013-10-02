#ifndef NEGATIVE_EVENT_MESSAGE_H
#define NEGATIVE_EVENT_MESSAGE_H


/**
   This is the class defining the termination token that gets passed
   around.  Essentially, it has a color and a terminator.

   The color can be WHITE, which means it's on the first cycle,
   RED, which means it's on it's second cycle, or BLACK.
*/

class SerializedInstance;

#include "KernelMessage.h"
#include "EventId.h"
#include "NegativeEvent.h"

/**
   This class represents the token that gets passed between
   TokenPassingTerminationManager instances.

   @see TokenPassingTerminationManager
*/

class NegativeEventMessage : public KernelMessage {
public:
    NegativeEventMessage(unsigned int source,
                         unsigned int dest,
                         const vector<const NegativeEvent*>& events,
                         const string& initGVTInfo):
        KernelMessage(source, dest),
        myEvents(events),
        gVTInfo(initGVTInfo) {}

    ~NegativeEventMessage();

    static Serializable* deserialize(SerializedInstance* data);
    static const string& getNegativeEventMessageType();

    void serialize(SerializedInstance*) const;

    const string& getDataType() const {
        return getNegativeEventMessageType();
    }

    static void registerDeserializer();

    const vector<const NegativeEvent*>& getEvents() { return myEvents; }

    const string getGVTInfo() {
        return gVTInfo;
    }

private:
    vector<const NegativeEvent*> myEvents;
    const string gVTInfo;
};

#endif
