
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
// $Id: NAryGenerator.cpp
// 
//---------------------------------------------------------------------------

#include "NAryBitGenerator.h"
#include "NAryBitGeneratorState.h"
#include "LogicEvent.h"
#include <math.h>

NAryBitGenerator::NAryBitGenerator(string &objectName, const int numOutputs,
                                   vector<string> *outputs,
                                   vector<int> *destInputPorts,
                                   vector<int> *fanOutSize,
                                   int numIterations, int numOutputBits,
                                   int objectDelay, bool print)
   : LogicComponent(objectName, numOutputs, outputs, destInputPorts,
                    fanOutSize, objectDelay) {
   numberOfBits = numOutputBits;
   maxIterations = numIterations;
   printFlag = print;
   maxCount = (int) pow(2, numberOfBits);
}


void
NAryBitGenerator::initialize() {
   LogicComponent::initialize();
   string filename("input");
   myStream = getOFStream(filename);
   
   NAryBitGeneratorState *newState = (NAryBitGeneratorState *)getState();
   
   newState->counterState = 0;
   newState->iteration = 0;

   LogicEvent* newEvent = (LogicEvent *) new LogicEvent(VTime(1));

   // Now, send an event to ourself to keep us alive...
   newEvent->bitValue = -1;
   receiveEvent(newEvent, this);
}

void
NAryBitGenerator::executeProcess() {
   NAryBitGeneratorState *state = (NAryBitGeneratorState *) getState();
   LogicEvent *bitEvent = NULL;
   bitEvent = (LogicEvent *) getEvent();
  
   if (state->counterState < maxCount) {
      updateOutput();
      state->counterState++;
   }
   
   if (state->counterState == maxCount)  {
      state->iteration++;
      if (state->iteration < maxIterations) {
         state->counterState = 0;
      }
   }
}

void
NAryBitGenerator::finalize() {}

void
NAryBitGenerator::updateOutput() {
   NAryBitGeneratorState *state = (NAryBitGeneratorState *) getState();
   LogicEvent* newEvent =
      (LogicEvent *) new LogicEvent(getSimulationTime()+delay);

   int counter, value = state->counterState;

   if(printFlag == true){
      *myStream << getSimulationTime() << ": ";
   }
  
   for (counter = 0; (counter < numberOfBits); counter++) {
      newEvent->bitValue = (int) (value & 1);

      if(printFlag == true){
         *myStream << ((((int) (value & 1)) == 1) ? "1 " : "0 ");
      }
    
      sendEvent(counter, newEvent);
      value = value >> 1;
   }
    
   if(printFlag == true){
      *myStream << nl;
   }
  
   // Now, send an event to ourself to keep us alive...
   newEvent->bitValue = -1;
   receiveEvent(newEvent, this);
}

State*
NAryBitGenerator::allocateState() {
   return (State *) new NAryBitGeneratorState();
}

void
NAryBitGenerator::deallocateState(State* state){
   delete (NAryBitGeneratorState *)state;
}

void
NAryBitGenerator::reclaimEvent(const Event *event){
   delete (LogicEvent *)event;
}

void
NAryBitGenerator::reportError(const string& msg, SEVERITY level){}
