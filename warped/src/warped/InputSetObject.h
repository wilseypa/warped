#ifndef INPUT_SET_OBJECT_H
#define INPUT_SET_OBJECT_H


#include "warped.h"
#include "State.h"
#include "SetObject.h"

/** The InputSetObject Class.

    This class is used for encapsulating events in the Input
    queue. This class is derived from the SetObject base class and
    redefines the comparison operators. Used exclusively in the append
    queue and the sender queue implementations.
*/
template <class Element>
class InputSetObject : public SetObject< Element > {
public:
   /**@name Public Class Methods of InputSetObject. */
   //@{

   /** Constructor.

       @param currentTime Current time  of event.
       @param currentElement Element to be encapsulated.
       @param senderID sender object if of the element.
   */
   InputSetObject(const VTime& currentTime, Element* currentElement,
                  const OBJECT_ID& id) :
      SetObject< Element >(currentTime, currentElement), senderID(id){}

   /** Constructor.

       @param currentTime Current time  of event.
       @param currentElement Element to be encapsulated.
   */
   InputSetObject(const VTime& currentTime, Element* currentElement):
      SetObject< Element >(currentTime, currentElement){}

   /** Constructor.

       @param currentTime Current time  of event.
   */
   InputSetObject(const VTime& currentTime):
      SetObject< Element >(currentTime, NULL){}

   /** Constructor.

       @param senderID sender Object id of the event.
   */
   InputSetObject(const OBJECT_ID& id) :
      SetObject< Element >(0, NULL), senderID(id){}

   /// Destructor.
   ~InputSetObject(){}

   /// Overloaded operator <
   friend bool operator<(const InputSetObject& a, const InputSetObject& b){
     return ((a.time < b.time) ? true : ((a.time == b.time) ? (a.senderID < b.senderID) : false));
   }
  
   /// Overloaded operator ==
   friend bool operator==(const InputSetObject& a, const InputSetObject& b){
      return (a.time == b.time) && (a.senderID == b.senderID);
   }
  
   /// Overloaded operator >
   friend bool operator>(const InputSetObject& a, const InputSetObject& b){
     return ((a.time > b.time) ? true : ((a.time == b.time) ? (a.senderID > b.senderID) : false));
   }
  
   /// Overloaded operator <<
   friend ostream& operator<<(ostream& os, const InputSetObject& a){
      return os << a.time;
   }

   //@} // End of Public Class Methods of InputSetObject.

protected:
   /**@name Protected Class Attributes of InputSetObject. */
   //@{

   ///OBJECT_ID of the process created this element(senderID)
   OBJECT_ID senderID;

   //@} // End of Protected Class Attributes of InputSetObject.

};

#endif
