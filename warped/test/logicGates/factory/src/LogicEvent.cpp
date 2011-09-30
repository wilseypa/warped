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

// Authors: Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
//
// $Id: LogicEvent.cpp
//
//---------------------------------------------------------------------------

#include "LogicEvent.h"

LogicEvent::LogicEvent(const VTime &recvTime)
   : FlatEvent(recvTime), bitValue(0), sourcePort(0), destinationPort(0){}

LogicEvent::~LogicEvent(){}

unsigned int
LogicEvent::getSize(){
   return sizeof(LogicEvent);
}

Event*
LogicEvent::deserialize(const SerializedInstance *instance){
   LogicEvent *event = new LogicEvent(VTime(0));
   memcpy(event, (LogicEvent *)instance->getData(), instance->getSize());
   return (Event *)event;
}


bool
LogicEvent::eventCompare(const Event* event) {
  return (this->getReceiveTime() == event->getReceiveTime());
}

ostream&
operator<< (ostream& os, const LogicEvent& pe) {
  os << " bit Value " << pe.bitValue
     << " sourcePort " << pe.sourcePort
     << " destinationPort " << pe.destinationPort << "\n";
  os << (const Event&)pe;
  return os;
}

