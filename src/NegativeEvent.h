#ifndef NEGATIVE_EVENT_H
#define NEGATIVE_EVENT_H


#include "warped.h"
#include "Serializable.h"
#include "Event.h"
#include "EventId.h"

/** The NegativeEvent class.

    This class implements the Negative Event. It attempts to contain the minimum
    amount of information needed to cancel events. All that is necessary to identify
    the positive event is the sender id and the event id. The receiver id is used so
    that the proper object handle can be easily obtained. The receive time is used
    to trigger a rollback if the corresponding positive message has already been
    executed. The send time is used for GVT information when sending remote events.

*/
class NegativeEvent : public Event {
public:
    /**@name Public Class Methods of NegativeEvent */
    //@{

    /** Constructor.

        @param sendTime The send time of the corresponding positive event.
        @param recvTime The receive time of the corresponding positive event.
        @param senderObj The ObjectID of the sender.
        @param receiverObj The ObjectID of the receiver.
        @param eveId The EventId of the corresponding positive event.
    */
    NegativeEvent(const VTime& senderTime,
                  const VTime& recvTime,
                  const ObjectID& senderObj,
                  const ObjectID& receiverObj,
                  const EventId& eveId):
        sender(new ObjectID(senderObj.getSimulationObjectID(),senderObj.getSimulationManagerID())),
        receiver(new ObjectID(receiverObj.getSimulationObjectID(),receiverObj.getSimulationManagerID())),
        sendTime(senderTime.clone()),
        receiveTime(recvTime.clone()),
        id(new EventId(eveId)) {}


    /// Destructor.
    ~NegativeEvent() {
        delete sender;
        delete sendTime;
        delete receiver;
        delete receiveTime;
        delete id;
    }

    /** Returns the source (sender) of the negative event.

        @return Sender id of event.
    */
    inline const ObjectID& getSender() const { return *sender; }

    /** Returns the destination (receiver) of the negative event.

        @return Receiver id of event.

    */
    inline const ObjectID& getReceiver() const { return *receiver; }

    /** Get the send time of the corresponding positive event.
        Used for GVT information.

        @return Positive event send time.
    */
    inline const VTime& getSendTime() const { return *sendTime; }

    /** Get the receive time of the corresponding positive event.
        Necessary to induce rollbacks.

        @return Positive event receive time.
    */
    inline const VTime& getReceiveTime() const { return *receiveTime; }

    /**
       Return the EventId of the corresponding positive event.

       @return Positive event EventId
    */
    inline const EventId& getEventId() const { return *id; }

    inline unsigned int getEventSize() const { return sizeof(NegativeEvent); }

    void serialize(SerializedInstance* addTo) const;

    static Serializable* deserialize(SerializedInstance* instance);

    const string& getDataType() const {
        return getNegativeEventDataType();
    }

    static const string& getNegativeEventDataType() {
        static string negativeEventDataType = "NegativeEvent";
        return negativeEventDataType;
    }

    static void registerDeserializer();

    friend std::ostream& operator<<(std::ostream& os, const NegativeEvent& event);

    bool eventCompare(const Event*);

    // No longer using these for optimistic fossil collection.
    /// Overload operator new.
    //void *operator new(size_t);

    /// Overload operator delete.
    //void operator delete(void *);

    //@} // End of Public Class Methods of NegativeEvent.

private:

    /**@name private Class Methods of NegativeEvent. */
    //@{

    /// The object id of the object sending this event.
    const ObjectID* sender;

    /// The object id of the object receiving this event.
    const ObjectID* receiver;

    /// The event send time.
    const VTime* sendTime;

    /// The event received time.
    const VTime* receiveTime;

    /// The event id of the corresponding positive event.
    const EventId* id;

    //@} // End of Private Class Methods of NegativeEvent.
};

#endif
