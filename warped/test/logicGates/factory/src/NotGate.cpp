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
// $Id: NotGate.cpp
//
//---------------------------------------------------------------------------

#include "NotGate.h"
#include "NotGateState.h"
#include "LogicEvent.h"

NotGate::NotGate(string &objectName, const int numOutputs,
                 vector<string> *outputs, vector<int> *destInputPorts,
                 vector<int> *fanOutSize, int objectDelay)
   : LogicComponent(objectName, numOutputs, outputs, destInputPorts,
                    fanOutSize, objectDelay){}

NotGate::~NotGate(){
   deallocateState(getState());
}

void
NotGate::initialize(){
   LogicComponent::initialize();
   NotGateState *newState = (NotGateState *) getState();
   newState->counter = 0;
}

void
NotGate::executeProcess(){
   NotGateState *newState = (NotGateState *) getState();
   LogicEvent *event, *newEvent = NULL;
   int outputBit = 0;
   
   do {
      event = (LogicEvent *)getEvent();
      switch(event->destinationPort){
      case 1:
         if(event->bitValue == 0){
            outputBit = 1;
         }
         else {
            outputBit = 0;
         }
         newState->counter++;
         newEvent = new LogicEvent(getSimulationTime() + delay);
         newEvent->bitValue = outputBit;
         sendEvent(0, newEvent);
         reclaimEvent(newEvent);
         break;
      default:
         cerr << "Warning: NotGate::executeProcess() received unknown event"
              << endl;
         break;
      }
     } while (haveMoreEvents() == true);
}


void
NotGate::finalize(){}

State*
NotGate::allocateState() {
  return (State *) new NotGateState();
}

void
NotGate::deallocateState(State* state){
   delete (NotGateState *)state;
}

void
NotGate::reclaimEvent(const Event *event){
   delete (LogicEvent *)event;
}

void
NotGate::reportError(const string& msg, SEVERITY level){}
