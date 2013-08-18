#ifndef STATE_SET_OBJECT_H
#define STATE_SET_OBJECT_H


#include "warped.h"
#include "State.h"
#include "SetObject.h"
#include "EventId.h"
#include "ObjectID.h"

/** The StateSetObject Class.

    This class is used for encapsulating events in the State
    queue. This class is derived from the SetObject base class and
    redefines the comparison operators. Used exclusively in the state
    saving classes.
*/
template <class Element>
class StateSetObject : public SetObject< Element > {
public:
   /**@name Public Class Methods of StateSetObject. */
   //@{

   /** Constructor.

       @param currentTime Current time  of event.
       @param currentElement Element to be encapsulated.
       @param eventID sender object if of the element.
   */
   StateSetObject(const VTime& currentTime, Element* currentElement,
                  const EventId *id, const ObjectID *sender) :
      SetObject< Element >(currentTime, currentElement), eventID(id), eventSender(sender){}

   /** Constructor.

       @param currentTime Current time  of event.
       @param currentElement Element to be encapsulated.
   */
   StateSetObject(const VTime& currentTime, Element* currentElement):
      SetObject< Element >(currentTime, currentElement){}

   /** Constructor.

       @param currentTime Current time  of event.
   */
   StateSetObject(const VTime& currentTime):
      SetObject< Element >(currentTime, NULL){}

   /** Constructor.

       @param eventID sender Object id of the event.
   */
   StateSetObject(const EventId& id) :
      SetObject< Element >(0, NULL), eventID(&id){}

   /// Destructor.
   ~StateSetObject(){}

   // Returns the event ID associated with the state save.
   const EventId *getEventId() const{
     return eventID;
   }

   // Returns the object sender of the event associated with the state save.
   const ObjectID *getEventSender() const{
     return eventSender;
   }

   /// Overloaded operator <
   //friend bool operator<(const StateSetObject& a, const StateSetObject& b){
   //  return ((a.time < b.time) ? true : ((a.time == b.time) ? (a.eventID < b.eventID) : false));
   //}
  
   /// Overloaded operator ==
   friend bool operator==(const StateSetObject& a, const StateSetObject& b){
      return (a.time == b.time) && (a->eventID == b->eventID) && (a->eventSender == b->eventSender);
   }
  
   /// Overloaded operator >
   //friend bool operator>(const StateSetObject& a, const StateSetObject& b){
   //  return ((a.time > b.time) ? true : ((a.time == b.time) ? (a.eventID > b.eventID) : false));
   //}
  
   /// Overloaded operator <<
   friend ostream& operator<<(ostream& os, const StateSetObject& a){
      return os << a.getTime();
   }

   //@} // End of Public Class Methods of StateSetObject.

protected:
   /**@name Protected Class Attributes of StateSetObject. */
   //@{

   ///EventId of the process created this element(eventID)
   const EventId *eventID;

   ///Sender of the event of this point.
   const ObjectID *eventSender;

   //@} // End of Protected Class Attributes of StateSetObject.

};

#endif
