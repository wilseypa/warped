// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Malolan Chetlur             mal@ececs.uc.edu
//          Jorgen Dahl                 dahlj@ececs.uc.edu
//          Dale E. Martin              dmartin@ececs.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Dhananjai Madhava Rao       dmadhava@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
// 
// $Id: HalfAdder.cpp
// 
//---------------------------------------------------------------------------

#include "HalfAdder.h"
#include "HalfAdderState.h"
#include "LogicEvent.h"

HalfAdder::HalfAdder(string &objectName, const int numObjects,
                     vector<string> *outputs,
                     vector<int> *outputPortNumbers,
                     vector<int> *fanOutSize, int objectDelay)
   : LogicComponent(objectName, numObjects, outputs, outputPortNumbers,
                    fanOutSize, objectDelay){}

void
HalfAdder::initialize() {
   HalfAdderState *newState = (HalfAdderState *) getState();
   newState->inputBit1  = 0;
   newState->inputBit2  = 0;
   newState->outputBit1 = 0;
   newState->outputBit2 = 0;
   LogicComponent::initialize();
}

void
HalfAdder::executeProcess() {
  HalfAdderState *state = (HalfAdderState *) getState();
  LogicEvent *bitEvent  = NULL;
  int updateFlag = 0;
  
  do {
    bitEvent = (LogicEvent *) getEvent();
    switch (bitEvent->destinationPort) {
    case 1:
      state->inputBit1 = bitEvent->bitValue;
      updateFlag       = 1;
      break;
      
    case 2:
      state->inputBit2 = bitEvent->bitValue;
      updateFlag       = 1;
      break;
      
    default:
      cout << "Warning : HalfAdder::executeProcess() - "
           << "Received an event that I don't know how to handle.\n";
    }
  } while (haveMoreEvents() == true);
  
  if (updateFlag == 1) {
    updateOutput();
  }
}

void
HalfAdder::finalize(){}

void
HalfAdder::updateOutput() {
  HalfAdderState *state = (HalfAdderState *) getState();
  
  state->outputBit1 = state->inputBit1 & state->inputBit2;
  state->outputBit2 = state->inputBit1 ^ state->inputBit2;

  LogicEvent *newEvent = new LogicEvent(getSimulationTime() + delay);
  newEvent->bitValue = state->outputBit1;
  sendEvent(1, newEvent);
  delete newEvent;

  newEvent = (LogicEvent *) new LogicEvent(getSimulationTime() + delay);
  newEvent->bitValue = state->outputBit2;
  sendEvent(0, newEvent);
  delete newEvent;
}

State*
HalfAdder::allocateState() {
  return (State *)new HalfAdderState();
}

void
HalfAdder::deallocateState(State *state){
   delete (HalfAdderState *)state;
}

void
HalfAdder::reclaimEvent(const Event *event){
   delete (LogicEvent *)event;
}

void
HalfAdder::reportError(const string& msg, SEVERITY level){}

