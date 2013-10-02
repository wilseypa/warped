#ifndef KERNEL_MESSAGE_H
#define KERNEL_MESSAGE_H


#include "warped.h"
#include "Serializable.h"
#include "SerializedInstance.h"
#include <string>

using std::string;

/** The KernelMessage class.

Communication between the communication manager and the simulation kernel
is accomplished by exchanging "KernelMessages". A KernelMessage may be
any one of the following: Initialization Message, Event Message,
TerminateSimulation Message, or any other registered (messages types must
be registered with the communication manager before use) event type.

The "data" field in the KernelMessage represents the payload of
the message. If the message type is an "InitializationMessage",
the data field contains a list of object names present on the
simulation manager that is sending this initialization message.
If the message type is "EventMessage", then the data field
contains an event. Similarly, if the message type is
"AbortSimulationMessage", the data field contains an error
message.  The "kernelData" field contains kernel specific
information that different kernel modules may use to exchange
information. For example, the GVTManager uses the kernelData field
to embed a color with every EventMessage that is sent across the
network.

*/
class KernelMessage : public Serializable {
public:
    /**@name Public Class Methods of KernelMessage. */
    //@{

    /** Constructor.

    @param myType The type of the message.
    @param source Source of the message.
    @param dest Destination of the message.
    @param serializedData Serialized message data ptr (default = NULL).
    @param kernelInfo Serialized message kernel data ptr (default = NULL).

    Memory you pass in here will be deleted at destruction time.
    (SerializedInstances only, of course.)
    */
    KernelMessage(unsigned int source,
                  unsigned int dest) :
        senderSimulationManagerID(source),
        destSimulationManagerID(dest),
        incarnationNumber(0) {}


    /// Destructor.
    virtual ~KernelMessage() {}

    /** Get the Id of the sender of the message.

    @return Id of the sender of the message.
    */
    unsigned int getSender() const {
        return senderSimulationManagerID;
    }

    /** Get the Id of the receiver of the message.

    @return Id of the receiver of the message.
    */
    unsigned int getReceiver() const {
        return destSimulationManagerID;
    }

    /**
       Unhide Serializable#serialize
    */
    SerializedInstance* serialize() const {
        return Serializable::serialize();
    }

    void serialize(SerializedInstance*) const;

    /**
       Derived classes call this method to deserialize the KernelMessage
       bits.
    */
    static void deserialize(SerializedInstance* data,
                            KernelMessage* instance);

    /** Sets the number of recoveries that have occurred.
        Used exclusively for optimistic fossil collection.

        @param unsigned int The number of recoveries that have occurred.
    */
    void setIncarnationNumber(unsigned int setIncNum) {
        incarnationNumber = setIncNum;
    }

    /** Returns the number of recoveries that have occurred before
        message sending. Used for optimistic fossil collection.

        @return The number of recoveries that have occurred.
    */
    unsigned int getIncarnationNumber() const {
        return incarnationNumber;
    }

    //@} // End of Public Class Methods of KernelMessage.

protected:
    /**@name Protected Class Attributes of KernelMessage. */
    //@{

    /**
       Default constructor - used by some deserializers.
    */
    KernelMessage():
        incarnationNumber(0) {}

    /// Id of the sender of the message.
    unsigned int senderSimulationManagerID;

    /// Id of the receiver of the message.
    unsigned int destSimulationManagerID;

    /// The number of recoveries that have occurred before the
    /// sending of this message. Only used for optimistic fossil collection.
    unsigned int incarnationNumber;

    //@} // End of Protected Class Attributes of KernelMessage.
};

#endif
