
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
// $Id: NAryPrinter.cpp
// 
//---------------------------------------------------------------------------

#include "NAryBitPrinter.h"
#include "NAryBitPrinterState.h"
#include "LogicEvent.h"

NAryBitPrinter::NAryBitPrinter(string &objectName, const int numOutputs,
                               vector<string> *outputs,
                               vector<int> *destInputPorts,
                               vector<int> *fanOutSize, int numBits,
                               bool print)
   : LogicComponent(objectName, numOutputs, outputs, destInputPorts,
                    fanOutSize, 0){
   printFlag = print;
   numberOfBits = numBits;
   triggerVector = new vector<int>(numBits,0);
}

NAryBitPrinter::~NAryBitPrinter(){
   deallocateState(getState());
}

void
NAryBitPrinter::initialize() {
   LogicComponent::initialize();

   string filename("output");
   myStream = getOFStream(filename);
}

void
NAryBitPrinter::executeProcess() {
   NAryBitPrinterState *state = (NAryBitPrinterState *) getState();
   LogicEvent *bitEvent = NULL;

   do {

      bitEvent = (LogicEvent *) getEvent();

      // save the bitValue in the state
      state->bitVector[bitEvent->destinationPort - 1] = bitEvent->bitValue;
      (*triggerVector)[bitEvent->destinationPort - 1] = 1;

      int trigger = 0;
      for(int i = 0; i < numberOfBits; i++){
         trigger += (*triggerVector)[i];
      }

      if(trigger == numberOfBits){
         // all the bits have been set
         *myStream << getSimulationTime() << ": ";
         for(int i = 0; i < numberOfBits; i++){
            *myStream << state->bitVector[i] << " ";
         }
         *myStream << nl;
         // reset the trigger vector
         for(int i = 0; i < numberOfBits; i++){
            (*triggerVector)[i] = 0;
         }
      }
   } while(haveMoreEvents() == true);
} 


void
NAryBitPrinter::finalize() {}

void
NAryBitPrinter::updateOutput() {
   //   NAryBitPrinterState *state = (NAryBitPrinterState *) getState();
   //int value = state->counterState;

   //if(printFlag == true){
   //   *myStream << ((((int) (value & 1)) == 1) ? "1 " : "0 ");
   //}
}

State*
NAryBitPrinter::allocateState() {
  return (State *) new NAryBitPrinterState(numberOfBits);
}

void
NAryBitPrinter::deallocateState(State *state) {
   // delete state
   // HINT: you could insert this in a free pool of states
   delete (NAryBitPrinterState *)state;
}

void
NAryBitPrinter::reclaimEvent(const Event *event){
   // delete event
   // HINT: you could insert this in a free pool of event
   delete (LogicEvent *)event;
}

void
NAryBitPrinter::reportError(const string& msg, SEVERITY level){
}

void
NAryBitPrinter::setDelay(const int del) {
  delay = del;
}

int
NAryBitPrinter::getDelay() const {
  return delay;
}

void
NAryBitPrinter::sendSelfTrigger() {
  LogicEvent* newEvent =
     (LogicEvent *) new LogicEvent(getSimulationTime() + delay);

  newEvent->bitValue = -1;
  newEvent->destinationPort = -1;
  receiveEvent(newEvent, this);
}

