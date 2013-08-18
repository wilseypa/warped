#ifndef LOGIC_EVENT_H
#define LOGIC_EVENT_H

#include "DefaultEvent.h"
#include "Event.h"
#include "IntVTime.h"
#include "SerializedInstance.h"

/** The LogicEvent class.

    This class represents the event class that is used in the logicGates
    application. Objects defined in this application exchange LogicEvents.
*/
class LogicEvent : public DefaultEvent {
public:
   /**@name Public Class Methods of LogicEvent. */
   //@{

   /// what kind of logic event is this
   enum LogicEventKind {LOGIC_EVENT, DATA, UNKNOWN_EVENT};

   /// Default constructor
   LogicEvent(const VTime &initRecvTime,
              const VTime &initSendTime,
              SimulationObject *initSender,
              SimulationObject *initReceiver);

   /// Default destructor
   ~LogicEvent ();

   /// return the size of this event
   unsigned int getEventSize();

   void setbitValue(int setValue);
   void setsourcePort(int setSource);
   void setdestinationPort(int setDes);

   int getbitValue() const;
   int getsourcePort() const;
   int getdestinationPort() const;

   /// deserilize an instance to create a LogicEvent
   Event *deserialize(const SerializedInstance* instance);

   /// serialize
   void serialize( SerializedInstance *addTo)const;

   /// return the kind of LogicEvent this is
   virtual LogicEventKind getLogicEventKind() const {
      return LogicEvent::LOGIC_EVENT;
   }

   bool eventCompare(const Event* event);
   
   static const string &getLogicEventDataType(){
    static string logicEventDataType = "LogicEvent";
    return logicEventDataType;
  }

 
   friend std::ostream& operator<< (std::ostream& os, const LogicEvent& pe);

   //@} // End of Public Class Methods of LogicEvent.

   /**@name Public Class Attributes of LogicEvent. */
   //@{

   /// value of the bit field
   int bitValue;

   /// source port id
   int sourcePort;

   /// destination port id
   int destinationPort;
   
   /// Constructor called by deserializer. 
   
   LogicEvent(const VTime &initSendTime,
              const VTime &initRecvTime,
              const ObjectID &initSender,
              const ObjectID &initReceiver,
              const unsigned int eventIdVal);

 //@} // End of Public Class Attributes of LogicEvent.


};

#endif
