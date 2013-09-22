#ifndef MEMEVENTS_HH
#define MEMEVENTS_HH

#include "Event.h"
#include "DefaultEvent.h"
#include "IntVTime.h"
#include "SerializedInstance.h"

/*
   The MemRequest Class.

   This class represents a memory request sent from a MemSourceObject.
   The request keeps track of when it was started so that a total
   request delay can be found.

*/
class MemRequest : public DefaultEvent {
public:
  MemRequest( const VTime &initSendTime,
              const VTime &initRecvTime,
              SimulationObject *initSender,
              SimulationObject *initReceiver) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
    startTime(0),
    processor(""){
  }

  ~MemRequest(){};

  unsigned int getEventSize() const { return sizeof(MemRequest); }

  static Serializable *deserialize( SerializedInstance *instance ){
    VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
    VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
    unsigned int senderSimManID = instance->getUnsigned();
    unsigned int senderSimObjID = instance->getUnsigned();
    unsigned int receiverSimManID = instance->getUnsigned();
    unsigned int receiverSimObjID = instance->getUnsigned();
    unsigned int eventId = instance->getUnsigned();

    ObjectID sender(senderSimObjID, senderSimManID);
    ObjectID receiver(receiverSimObjID, receiverSimManID);

    MemRequest *event = new MemRequest(*sendTime, *receiveTime, sender, receiver, eventId);

    event->setStartTime(instance->getInt());
    event->setProcessor(instance->getString());

    delete sendTime;
    delete receiveTime; 
    return event;
  }

  void serialize( SerializedInstance *addTo ) const{
     Event::serialize( addTo );
     addTo->addInt(startTime);
     addTo->addString(processor);
  }
  
  bool eventCompare(const Event* event){
    MemRequest *MemEvent = (MemRequest*) event;
    return ( compareEvents(this, event) && 
             startTime == MemEvent->getStartTime() &&
             processor == MemEvent->getProcessor());
  }

  static const string &getMemRequestDataType(){
    static string memRequestDataType = "MemRequest";
    return memRequestDataType;
  }
  
  const string &getDataType() const {
    return getMemRequestDataType();
  }

  int getStartTime(){ return startTime; }
  void setStartTime(int setTime){ startTime = setTime; }

  string getProcessor(){ return processor; }
  void setProcessor(string setProc){ processor = setProc; }

private:
  /// Constructor called by deserializer.
  MemRequest( const VTime &initSendTime,
	     const VTime &initRecvTime,
	     const ObjectID &initSender,
	     const ObjectID &initReceiver,
	     const unsigned int eventIdVal) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver, eventIdVal ),
    startTime(0),
    processor("")
  {}

  /// This is used to determine the amount of time it took to satisfy the memory request.
  int startTime;

  /// This is used to determine which source processor started the request.
  string processor;
};

#endif
