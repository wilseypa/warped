// Copyright (c) The University of Cincinnati.
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
// THE SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Xinyu Guo                   guox2@mail.uc.edu        
//          Philip A. Wilsey            phil.wilsey@uc.edu
//
//---------------------------------------------------------------------------
//
// $Id: LogicEvent.cpp
//
//---------------------------------------------------------------------------

#include "LogicEvent.h"

LogicEvent::LogicEvent(const VTime &initRecvTime,
                       const VTime &initSendTime,
                       SimulationObject *initSender,
                       SimulationObject *initReceiver) :
  DefaultEvent(initRecvTime, initSendTime, initSender, initReceiver),
  bitValue(0),
  sourcePort(0), 
  destinationPort(0){}

LogicEvent::~LogicEvent(){}

unsigned int
LogicEvent::getEventSize() const {
   return sizeof(LogicEvent);
}

void 
LogicEvent::setbitValue(int setValue){bitValue = setValue;}

void 
LogicEvent::setsourcePort(int setSource){sourcePort = setSource;}

void
LogicEvent::setdestinationPort(int setDes){destinationPort = setDes;}

int
LogicEvent:: getbitValue() const {return bitValue;} 

int
LogicEvent:: getsourcePort() const {return sourcePort;}

int
LogicEvent:: getdestinationPort() const {return destinationPort;}




void
LogicEvent::serialize(SerializedInstance *addTo) const{
  Event::serialize( addTo );
  addTo->addInt(bitValue);
  addTo->addInt(sourcePort);
  addTo->addInt(destinationPort);
}

Serializable*
LogicEvent::deserialize(SerializedInstance *instance){

   VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
   VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
   unsigned int senderSimManID = instance->getUnsigned(); 
   unsigned int senderSimObjID = instance->getUnsigned();
   unsigned int receiverSimManID = instance->getUnsigned();
   unsigned int receiverSimObjID = instance->getUnsigned();
   unsigned int eventId = instance->getUnsigned();
   
   ObjectID sender(senderSimObjID, senderSimManID);
   ObjectID receiver(receiverSimObjID, receiverSimManID);
   
   LogicEvent *event = new LogicEvent(*sendTime, *receiveTime, sender, receiver, eventId);
  
   event->setbitValue(instance->getInt());
   event->setsourcePort(instance->getInt());
   event->setdestinationPort(instance->getInt());

   delete sendTime;
   delete receiveTime;
   return event;
}


bool
LogicEvent::eventCompare(const Event* event) {
 LogicEvent *LogicE = (LogicEvent*) event;
 return (compareEvents(this,event) &&
         bitValue == LogicE->getbitValue() &&
         sourcePort == LogicE->getsourcePort() &&
         destinationPort == LogicE->getdestinationPort());
}


ostream&
operator<< (ostream& os, const LogicEvent& pe) {
  os << " bit Value " << pe.bitValue
     << " sourcePort " << pe.sourcePort
     << " destinationPort " << pe.destinationPort << "\n";
  os << (const Event&)pe;
  return os;
}

LogicEvent::LogicEvent(const VTime &initSendTime,
                       const VTime &initRecvTime,
                       const ObjectID &initSender,
                       const ObjectID &initReceiver,
                       const unsigned int eventIdVal):
  DefaultEvent(initSendTime, initRecvTime, initSender, initReceiver, eventIdVal),
  bitValue(0),
  sourcePort(0), 
  destinationPort(0){}

