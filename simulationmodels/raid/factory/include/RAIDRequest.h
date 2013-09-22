//-*-c++-*-
#ifndef RAIDTYPES_H
#define RAIDTYPES_H

#if 1
#include <fstream>
#endif

#include <stdlib.h>
#include "Event.h"
#include "DefaultEvent.h"
#include "SerializedInstance.h"
using namespace std;

enum DISK_TYPE {FUJITSU, FUTUREDISK, LIGHTNING};

class RAIDRequest : public DefaultEvent {
public:
  RAIDRequest(const VTime &sendTime,
              const VTime &recvTime,
              SimulationObject *sender,
              SimulationObject *receiver)
    :DefaultEvent(sendTime ,recvTime, sender, receiver),
    sinkObject(""), sourceObject(sender->getName()), startStripe(0),
    logicalSector(0), startCylinder(0), startSector(0), 
    beginningOfStrype(0), tokenNumber(0), sizeRead(0),  
    sizeParity(0), parityMsg(false), read(true) {
  }

  static const string &getRAIDRequestDataType(){
    static string RAIDRequestDataType = "RAIDRequest";
    return RAIDRequestDataType;
  }

  const string &getDataType() const {
    return getRAIDRequestDataType();
  }

  void serialize( SerializedInstance *addTo ) const{
    Event::serialize( addTo );
    addTo->addString(sinkObject);
    addTo->addString(sourceObject);
    addTo->addInt(startStripe);
    addTo->addInt(logicalSector);
    addTo->addInt(startCylinder);
    addTo->addInt(startSector);
    addTo->addInt(beginningOfStrype);
    addTo->addInt(tokenNumber);
    addTo->addInt(sizeRead);
    addTo->addInt(sizeParity);
    addTo->addInt(parityMsg);
    addTo->addInt(read);
  }

  static Serializable *deserialize(SerializedInstance* instance) {
    VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
    VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
    unsigned int senderSimManID = instance->getUnsigned();
    unsigned int senderSimObjID = instance->getUnsigned();
    unsigned int receiverSimManID = instance->getUnsigned();
    unsigned int receiverSimObjID = instance->getUnsigned(); 
    unsigned int eventId = instance->getUnsigned();
 
    ObjectID sender(senderSimObjID, senderSimManID);
    ObjectID receiver(receiverSimObjID, receiverSimManID);

    RAIDRequest *event = new RAIDRequest(*sendTime, *receiveTime, sender, receiver, eventId);
    
    event->setSinkObject( instance->getString() );
    event->setSourceObject( instance->getString() );
    event->setStartStripe( instance->getInt() );
    event->setLogicalSector( instance->getInt() );
    event->setStartCylinder( instance->getInt() );
    event->setStartSector( instance->getInt() );
    event->setBeginningOfStrype( instance->getInt() );
    event->setTokenNumber( instance->getInt() );
    event->setSizeRead( instance->getInt() );
    event->setSizeParity( instance->getInt() );
    event->setParityMsg( instance->getInt() );
    event->setRead( instance->getInt() );    

    delete sendTime;
    delete receiveTime;
    return event;
  }

  unsigned int getEventSize() const { return sizeof(RAIDRequest); }

  RAIDRequest &operator=(const RAIDRequest &rhs) {
    if (this != &rhs) {
      sinkObject = rhs.sinkObject;
      sourceObject = rhs.sourceObject;
      startStripe = rhs.startStripe;
      logicalSector = rhs.logicalSector;
      startSector = rhs.startSector;
      beginningOfStrype = rhs.beginningOfStrype;
      tokenNumber = rhs.tokenNumber;
      sizeRead = rhs.sizeRead;
      sizeParity = rhs.sizeParity;
      parityMsg = rhs.parityMsg;
      read = rhs.read;
    }
    return *this;
  }
  
  bool eventCompare(const Event* event) {
    //return compareEvents(this, event);
    RAIDRequest *RAIDEvent = (RAIDRequest*) event;
    return ( compareEvents(this, event) && 
             sinkObject == RAIDEvent->sinkObject &&
        sourceObject == RAIDEvent->sourceObject &&
        startStripe == RAIDEvent->startStripe &&
        logicalSector == RAIDEvent->logicalSector &&
        startSector == RAIDEvent->startSector &&
        beginningOfStrype == RAIDEvent->beginningOfStrype &&
        tokenNumber == RAIDEvent->tokenNumber &&
        sizeRead == RAIDEvent->sizeRead &&
        parityMsg == RAIDEvent->parityMsg &&
        read == RAIDEvent->read );
  }

  const string getSinkObject() const {return sinkObject;}
  const string getSourceObject() const {return sourceObject;}
  int getStartStripe() const {return startStripe;}
  int getLogicalSector() const {return logicalSector;}
  int getStartCylinder() const {return startCylinder;}
  int getStartSector() const {return startSector;}
  int getBeginningOfStrype() const {return beginningOfStrype;}
  int getTokenNumber() const {return tokenNumber;}
  int getSizeRead() const {return sizeRead;}
  int getSizeParity() const {return sizeParity;}
  bool getParityMsg() const {return parityMsg;}
  bool getRead() const {return read;}

  void setSinkObject(string id) { sinkObject = id; }
  void setSourceObject(string id) { sourceObject = id; }
  void setStartStripe(int stripe) { startStripe = stripe; }
  void setLogicalSector(int sector) { logicalSector = sector; }
  void setStartCylinder(int cylinder) { startCylinder = cylinder; }
  void setStartSector(int sector) { startSector = sector; }
  void setBeginningOfStrype(int beginning) { beginningOfStrype = beginning; }
  void setTokenNumber(int number) { tokenNumber = number; }
  void setSizeRead(int readSize) { sizeRead = readSize; }
  void setSizeParity(int paritySize) { sizeParity = paritySize; }
  void setParityMsg(bool isParity) { parityMsg = isParity; }
  void setRead(bool isRead) { read = isRead; }

  void print(ofstream &fout) {
    fout << "EventSource: (" << sourceObject << ") "
	 << "TokenNr:" << tokenNumber << " "
	 << "SizeRead:" << sizeRead << " " 
	 << "SizeParity:" << sizeParity << " " 
	 << "IsParityMsg:" << parityMsg << " " 
	 << "IsRead:" << read;
  }
private:
  // Constructor called by deserializer.
  RAIDRequest(const VTime &sendTime,
              const VTime &recvTime,
              const ObjectID &sender,
              const ObjectID &receiver,
              unsigned int initEventId)
    :DefaultEvent(sendTime ,recvTime, sender, receiver, initEventId), 
    sinkObject(""), sourceObject(""), startStripe(0),
    logicalSector(0), startCylinder(0), startSector(0),
    beginningOfStrype(0), tokenNumber(0), sizeRead(0),
    sizeParity(0), parityMsg(false), read(true) {
  }

  string sourceObject; // Who created this event.
  string sinkObject; // Who's going to (eventually) consume this event.
  // NEVER USED: OBJECT_ID serverId; // Where this event is going.
  // The starting location for our stripe request
  int startStripe;
  int logicalSector;
  int startCylinder;
  int startSector;
  int beginningOfStrype;
  int tokenNumber;
  // Size of the read or write
  int sizeRead;
  // How many parity messages the process should expect.
  int sizeParity;
  // If this is a parity message or not.
  bool parityMsg;
  // Is this request for a read or write.
  bool read;
};
#endif
