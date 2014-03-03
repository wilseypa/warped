#ifndef MATTERN_GVT_MESSAGE_H
#define MATTERN_GVT_MESSAGE_H


/**
   This is the class defining the termination token that gets passed
   around.  Essentially, it has a color and a terminator.

   The color can be WHITE, which means it's on the first cycle,
   RED, which means it's on it's second cycle, or BLACK.
*/

#include <string>                       // for string

#include "DeserializerManager.h"        // for string
#include "KernelMessage.h"              // for KernelMessage
#include "Serializable.h"               // for string, etc
#include "VTime.h"                      // for VTime

class Serializable;

using std::string;

class SerializedInstance;

/**
   This class represents the token that gets passed between
   communication managers at matternGVT time.
*/

class MatternGVTMessage : public KernelMessage {
public:
    inline MatternGVTMessage(unsigned int source,
                             unsigned int dest,
                             const VTime& initLastScheduledEventTime,
                             const VTime& initMinimumTimeStamp,
                             const int initNumMessagesInTransit) :
        KernelMessage(source, dest),
        lastScheduledEventTime(initLastScheduledEventTime.clone()),
        minimumTimeStamp(initMinimumTimeStamp.clone()),
        numMessagesInTransit(initNumMessagesInTransit) {}

    void serialize(SerializedInstance*) const;
    static Serializable* deserialize(SerializedInstance* data);

    static const string& getMatternGVTMessageType();

    const string& getDataType() const {
        return getMatternGVTMessageType();
    }

    static void registerDeserializer();

    const VTime& getLastScheduledEventTime() const {
        return *lastScheduledEventTime;
    }

    const VTime& getMinimumTimeStamp() const {
        return *minimumTimeStamp;
    }

    const int getNumMessagesInTransit() const {
        return numMessagesInTransit;
    }

    ~MatternGVTMessage() {
        delete lastScheduledEventTime;
        delete minimumTimeStamp;
    }

private:
    /**
       Default constructor - to be used only by the deserializer.
    */
    MatternGVTMessage() :
        lastScheduledEventTime(0),
        minimumTimeStamp(0),
        numMessagesInTransit(0) {}


    const VTime* lastScheduledEventTime;
    const VTime* minimumTimeStamp;
    const int numMessagesInTransit;
};

#endif
