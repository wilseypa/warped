#ifndef LOGIC_EVENT_H
#define LOGIC_EVENT_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "FlatEvent.h"

/** The LogicEvent class.

    This class represents the event class that is used in the logicGates
    application. Objects defined in this application exchange LogicEvents.
*/
class LogicEvent : public FlatEvent {
public:
   /**@name Public Class Methods of LogicEvent. */
   //@{

   /// what kind of logic event is this
   enum LogicEventKind {LOGIC_EVENT, DATA, UNKNOWN_EVENT};

   /// Default constructor
   LogicEvent(const VTime& recvTime);

   /// Default destructor
   ~LogicEvent ();

   /// return the size of this event
   unsigned int getSize();

   /// deserilize an instance to create a LogicEvent
   Event *deserialize(const SerializedInstance* instance);

   /// return the kind of LogicEvent this is
   virtual LogicEventKind getLogicEventKind() const {
      return LogicEvent::LOGIC_EVENT;
   }

   bool eventCompare(const Event* event);

   friend ostream& operator<< (ostream& os, const LogicEvent& pe);

   //@} // End of Public Class Methods of LogicEvent.

   /**@name Public Class Attributes of LogicEvent. */
   //@{

   /// value of the bit field
   int bitValue;

   /// source port id
   int sourcePort;

   /// destination port id
   int destinationPort;
   
   //@} // End of Public Class Attributes of LogicEvent.
  
};

#endif
