#ifndef LOGIC_EVENT_H
#define LOGIC_EVENT_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "DefaultEvent.h"
#include "Event.h"
#include "IntVTime.h"
#include "SerializedInstance.h"

/** The LogicEvent class.

    This class represents the event class that is used in the logicGates
    application. Objects defined in this application exchange LogicEvents.
*/

class LogicEvent : public DefaultEvent 
{
public:
   enum LogicEventKind {LOGIC_EVENT, DATA, UNKNOWN_EVENT};

   /// Default constructor
   LogicEvent(const VTime &initRecvTime,
              const VTime &initSendTime,
              SimulationObject *initSender,
              SimulationObject *initReceiver);

   /// Default destructor
   ~LogicEvent ();

   unsigned int getEventSize() const;
   void setObjectName (string setObject);
   void setbitValue1  (int setValue1);
   void setbitValue2  (int setValue2);
   void setsourcePort (int setSource);
   void setdestinationPort(int setDes);

   string getObjectName() const;
   int getbitValue1() const;
   int getbitValue2() const;
   int getsourcePort() const;
   int getdestinationPort() const;

   static Serializable *deserialize(SerializedInstance* instance);

   void serialize( SerializedInstance *addTo)const;

   virtual LogicEventKind getLogicEventKind() const 
   {
     return LogicEvent::LOGIC_EVENT;
   }
   
   bool eventCompare(const Event* event);

   const string &getDataType() const 
   {
     return getLogicEventDataType();
   }
   
   static const string &getLogicEventDataType()
   {
     static string logicEventDataType = "LogicEvent";
     return logicEventDataType;
   }
   
   friend ostream& operator<< (ostream& os, const LogicEvent& pe);

   string sObjectName;
   int bitValue1;
   int bitValue2;
   int sourcePort;
   int destinationPort;

   /// Constructor called by deserializer. 
   LogicEvent(const VTime &initSendTime,
              const VTime &initRecvTime,
              const ObjectID &initSender,
              const ObjectID &initReceiver,
              const unsigned int eventIdVal);
};

#endif
