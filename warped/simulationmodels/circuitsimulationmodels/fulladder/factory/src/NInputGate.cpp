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
//          Xinyu Guo                   guox2@mail.uc.edu
//          Manas Minglani              minglams@mail.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
// 
// $Id: NInputGate.cpp
// 
//---------------------------------------------------------------------------

#include "../include/NInputGate.h"
#include "../include/LogicEvent.h"
#include "../include/NInputGateState.h"
#include "SimulationStream.h"
#include <iostream>
#include <sstream>

using namespace std;

NInputGate::NInputGate(string &objectName, const int numInputs,/*const int numInValue,*/
                       const int numOutputs,vector<string> *outputs,
                       vector<int> *destInputPorts,
                       int objectDelay)
                      :LogicComponent(objectName, numOutputs, outputs,
                       destInputPorts, objectDelay),
                       numberOfInputs(numInputs),
                       numInputValue(0){}

NInputGate::~NInputGate(){}

void
NInputGate::initialize() {
  cout<<endl;
  cout<<"this is "<<getName()<<endl;
  LogicComponent::initialize();
  NInputGateState *newState = (NInputGateState *) getState();
  for(int i = 0; i < numberOfInputs; i++){
    newState->inputBits[i] = 0;
  }
  cout<<"numberOfInputs is:"<<numberOfInputs<<endl;
  newState->outputBit1 = 0;
}

void
NInputGate::executeProcess() {
  cout<<endl;
  cout<<"in the executePorcess()"<<getName()<<endl;
  NInputGateState *state = static_cast<NInputGateState *>( getState());
  LogicEvent *logicEvent  = NULL;
  while(true==haveMoreEvents()){
    logicEvent = (LogicEvent *)getEvent();
    if(logicEvent != NULL){
      if(numInputValue != numberOfInputs-1){
        state->inputBits[logicEvent->destinationPort - 1] = logicEvent->bitValue;
	numInputValue++;
      }
      else{
	state->inputBits[logicEvent->destinationPort - 1] = logicEvent->bitValue;
	updateOutput();
	numInputValue=0;
      }
    }
  }
}

       
void
NInputGate::finalize(){}

const string&
NInputGate::getName() const{return myObjectName;}

void
NInputGate::updateOutput() {
  NInputGateState *state =static_cast<NInputGateState *>(getState());
  state->outputBit1 = computeOutput(state->inputBits);
  int outputValue = state->outputBit1;
  sendEvent(outputValue);
} 

State*
NInputGate::allocateState() {
  return (State *) new NInputGateState(numberOfInputs);
}

void
NInputGate::deallocateState(const State *state){
   delete (NInputGateState *)state;
}

void
NInputGate::reclaimEvent(const Event *event){
   delete (LogicEvent *)event;
}

void
NInputGate::reportError(const string &msg, SEVERITY level){}

