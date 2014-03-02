#ifndef EVENT_MESSAGE_BASE_H
#define EVENT_MESSAGE_BASE_H


/**
   This is the class defining the termination token that gets passed
   around.  Essentially, it has a color and a terminator.

   The color can be WHITE, which means it's on the first cycle,
   RED, which means it's on it's second cycle, or BLACK.
*/

#include <string>                       // for string

#include "KernelMessage.h"              // for KernelMessage
#include "Serializable.h"               // for string, etc
using std::string;

class Event;
class SerializedInstance;

/**
   This class represents the token that gets passed between
   TokenPassingTerminationManager instances.

   @see TokenPassingTerminationManager
*/

class EventMessageBase : public KernelMessage {
public:
    inline EventMessageBase(unsigned int source,
                            unsigned int dest,
                            const Event* initEvent,
                            const string& initGVTInfo) :
        KernelMessage(source, dest),
        event(initEvent),
        gVTInfo(initGVTInfo) {}

    void serialize(SerializedInstance*) const;
    static Serializable* deserialize(const SerializedInstance* data);


    const Event* getEvent() const {
        return event;
    }

    const string getGVTInfo() {
        return gVTInfo;
    }

private:
    const Event* event;
    const string gVTInfo;
};

#endif
