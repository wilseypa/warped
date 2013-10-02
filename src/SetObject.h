#ifndef SET_OBJECT_H
#define SET_OBJECT_H


#include "warped.h"
#include <iostream>
using std::ostream;

class EventId;
/** The SetObject Class.
 This class is used for encapsulating elements such as events and states.
 */
template<class Element>
class SetObject {
public:
    /**@name Public Class Methods of SetObject. */
    //@{

    /** Constructor.

     @param currentTime Main time of the element.
     @param currentElement Element to be encapsulated.
     */
    SetObject(const VTime& currentTime, const Element* currentElement) :
        processed(false), mainTime(currentTime.clone()),
        secondaryTime(currentTime.clone()), receiverID(0),
        element(currentElement), eventNumber(0), senderObjectId(0),
        senderObjectSimId(0) {
    }

    //New Constructor for adding Event Id to SetObject
//  SetObject(const VTime& currentTime, const Element *currentElement,
//          const unsigned int currentEventNumber) :
//      processed(false), mainTime(currentTime.clone()),
//              secondaryTime(currentTime.clone()), receiverID(0),
//              element(currentElement), eventNumber(currentEventNumber),
//              senderObjectId(0), senderObjectSimId(0) {
//  }
    //New Constructor for adding Event Id and SenderId
    SetObject(const VTime& currentTime, const Element* currentElement,
              const unsigned int currentEventNumber,
              const unsigned int currentsenderObjectId,
              const unsigned int currentsenderObjectSimId) :
        processed(false), mainTime(currentTime.clone()),
        secondaryTime(currentTime.clone()), receiverID(0),
        element(currentElement), eventNumber(currentEventNumber),
        senderObjectId(currentsenderObjectId),
        senderObjectSimId(currentsenderObjectSimId) {
    }
    /** Constructor.

     @param currentTime Main time of the element.
     @param currentElement Element to be encapsulated.
     @param curTimeSec Secondary time of the element.
     */
    SetObject(const VTime& currentTime, const VTime& curTimeSec,
              const Element* currentElement) :
        processed(false), mainTime(currentTime.clone()),
        secondaryTime(curTimeSec.clone()), receiverID(0),
        element(currentElement), eventNumber(0) {
    }

    /** Constructor.

     @param currentTime Main time of the element.
     @param currentElement Element to be encapsulated.
     @param curTimeSec Secondary time of the element.
     @param recvId The simulation object id of the receiver.
     */
    SetObject(const VTime& currentTime, const VTime& curTimeSec,
              unsigned int recvId, const Element* currentElement) :
        processed(false), mainTime(currentTime.clone()),
        secondaryTime(curTimeSec.clone()), receiverID(recvId),
        element(currentElement), eventNumber(0), senderObjectId(0),
        senderObjectSimId(0) {
    }

    /** Constructor.

     @param currentTime Main time of the element.
     */
    SetObject(const VTime& currentTime) :
        processed(false), mainTime(currentTime.clone()),
        secondaryTime(currentTime.clone()), receiverID(0),
        element(NULL), eventNumber(0), senderObjectId(0),
        senderObjectSimId(0) {
    }

    SetObject(const SetObject& toCopy) :
        processed(toCopy.processed), mainTime(toCopy.mainTime->clone()),
        secondaryTime(toCopy.secondaryTime->clone()),
        receiverID(toCopy.receiverID), element(toCopy.element),
        eventNumber(toCopy.eventNumber),
        senderObjectId(toCopy.senderObjectId),
        senderObjectSimId(toCopy.senderObjectSimId) {
    }

    /// Destructor.
    ~SetObject() {
        delete mainTime;
        delete secondaryTime;
        mainTime = 0;
        secondaryTime = 0;
    }

    /** Get the main time of element.

     @return Main time of element.
     */
    const VTime& getMainTime() const {
        ASSERT(mainTime != 0);
        return *mainTime;
    }

    /** Get the secondary time of element.

     @return Secondary time of element.
     */
    const VTime& getSecondaryTime() const {
        ASSERT(secondaryTime != 0);
        return *secondaryTime;
    }

    /** Get the simulation object id of the receiver.

     @return unsigned int Object ID of the receiver.
     */
    unsigned int getReceiverID() {
        return receiverID;
    }

    /** Get a pointer to the element.

     @return Pointer to the element.
     */
    const Element* getElement() const {
        return element;
    }

    /// Overloaded operator =
    SetObject<Element>& operator=(const SetObject& rhs) {
        processed = rhs.processed;
        mainTime = rhs.mainTime->clone();
        secondaryTime = rhs.secondaryTime->clone();
        receiverID = rhs.receiverID;
        element = rhs.element;
        eventNumber = rhs.eventNumber;
        senderObjectId = rhs.senderObjectId;
        senderObjectSimId = rhs.senderObjectSimId;
        return *this;
    }

    /// Overloaded operator <
    bool operator<(const SetObject& rhs) const {
        return getMainTime() < rhs.getMainTime();
    }

    /// Overloaded operator ==
    bool operator==(const SetObject& rhs) const {
        return getMainTime() == rhs.getMainTime();
    }

    /// Overloaded operator >
    bool operator>(const SetObject& rhs) const {
        return getMainTime() > rhs.getMainTime();
    }

    void setProcessed() {
        processed = true;
    }

    bool processed;

    const unsigned int getEventNumber() const {
        return (eventNumber);
    }

    const unsigned int getsenderObjectId() const {
        return (senderObjectId);
    }
    const unsigned int getsenderObjectSimId() const {
        return (senderObjectSimId);
    }
    //@} // End of Public Class Methods of SetObject.

protected:

private:
    /**@name Protected Class Attributes of SetObject. */
    //@{

    /// Timestamp of element.
    const VTime* mainTime;

    /// Secondary timestamp of the element if needed (ex: send and receive time of an event).
    const VTime* secondaryTime;

    /// Pointer to the element.
    const Element* element;

    /// The event receiver simulation object id (just an unsigned int).
    unsigned int receiverID;

    const unsigned int eventNumber;

    const unsigned int senderObjectId;

    const unsigned int senderObjectSimId;

    //@} // End of Protected Class Attributes of SetObject.
};

/// Overloaded operator <<
template<class Element>
inline std::ostream&
operator<<(std::ostream& os, const SetObject<Element>& a) {
    os << a.getMainTime();
    return os;
}
#endif
