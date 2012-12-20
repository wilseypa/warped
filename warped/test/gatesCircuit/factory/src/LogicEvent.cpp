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

// Authors: Manas Minglani              minglams@mail.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
//
// $Id: LogicEvent.cpp
//
//---------------------------------------------------------------------------

#include "../include/LogicEvent.h"

LogicEvent::LogicEvent(const VTime &initRecvTime,
                       const VTime &initSendTime,
                       SimulationObject *initSender,
                       SimulationObject *initReceiver) :
  DefaultEvent(initRecvTime, initSendTime, initSender, initReceiver),
  bitValue1(0),
  bitValue2(0),
  sourcePort(0), 
  destinationPort(0){}

LogicEvent::~LogicEvent(){}

unsigned int
LogicEvent::getEventSize() const 
{
  return sizeof(LogicEvent);
}

void
LogicEvent::setObjectName (string setObject){sObjectName = setObject;}

void 
LogicEvent::setbitValue1(int setValue1){bitValue1 = setValue1;}

void 
LogicEvent::setbitValue2(int setValue2){bitValue2 = setValue2;}

void 
LogicEvent::setsourcePort(int setSource){sourcePort = setSource;}

void
LogicEvent::setdestinationPort(int setDes){destinationPort = setDes;}

string
LogicEvent::getObjectName() const {return sObjectName;}

int
LogicEvent:: getbitValue1() const {return bitValue1;} 

int
LogicEvent:: getbitValue2() const {return bitValue2;} 

int
LogicEvent:: getsourcePort() const {return sourcePort;}

int
LogicEvent:: getdestinationPort() const {return destinationPort;}

void
LogicEvent::serialize(SerializedInstance *addTo) const
{
  Event::serialize( addTo );
  addTo -> addString(sObjectName);
  addTo -> addInt(bitValue1);
  addTo -> addInt(bitValue2);
  addTo -> addInt(sourcePort);
  addTo -> addInt(destinationPort);
}

Serializable*
LogicEvent::deserialize(SerializedInstance *instance)
{
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
   
   event -> setObjectName( instance -> getString() );   
   event -> setbitValue1 ( instance -> getInt() );
   event -> setbitValue2 ( instance -> getInt() );
   event -> setsourcePort ( instance -> getInt() );
   event -> setdestinationPort ( instance -> getInt() );

   delete sendTime;
   delete receiveTime;
   return event;
}

bool
LogicEvent::eventCompare(const Event* event) {
 return (this->getReceiveTime() == event->getReceiveTime());
 /*LogicEvent *LogicE = (LogicEvent*) event;
 return (compareEvents(this,event) &&
         bitValue == LogicE->getbitValue() &&
         sourcePort == LogicE->getsourcePort() &&
         destinationPort == LogicE->getdestinationPort() &&
         );*/
}

ostream&
operator<< (ostream& os, const LogicEvent& pe) {
  os << " bit Value1 " << pe.bitValue1
     << " bit Value2 " << pe.bitValue2
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
  bitValue1(0),
  bitValue2(0),
  sourcePort(0), 
  destinationPort(0){}
