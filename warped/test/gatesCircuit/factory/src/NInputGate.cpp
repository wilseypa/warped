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
// $Id: NInputGate.cpp
// 
//---------------------------------------------------------------------------

#include "../include/NInputGate.h"
#include "../include/LogicEvent.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

NInputGate::NInputGate(string &objectName, 
		       const int numInputs,
                       const int numOutputs, 
		       vector<string> outputs,
                       vector<int> *destInputPorts,
                       vector<int> *fanOutSize, 
		       int objectDelay)
  : LogicComponent(objectName, 
		   numOutputs, 
		   outputs,
		   destInputPorts, 
		   fanOutSize, 
		   objectDelay),
    numberOfInputs(numInputs){outputNames = outputs;}

NInputGate::~NInputGate()
{
  deallocateState(getState());
}

void
NInputGate::initialize() 
{
  outputHandles.resize(outputNames.size(),NULL);
  outputHandles[0] = getObjectHandle(outputNames[0]);

  NInputGateState *newState = (NInputGateState *) getState();

  for(int i = 0; i < numberOfInputs; i++)
    {
      newState -> inputBits[i] = 0;
    }
  
  //cout<<"numberOfInputs is:"<<numberOfInputs<<endl;
  
  newState->outputBit1 = 0;
    
  IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());
  LogicEvent *firstEvent = new LogicEvent(sendTime,
                                          sendTime + 1,
                                          this,
					  outputHandles[0]);
  
  firstEvent->setbitValue1(1);
  firstEvent->setbitValue2(1);
  firstEvent->setsourcePort(0);
  firstEvent->setdestinationPort(2);
  
  (outputHandles[0]) -> receiveEvent(firstEvent);
}

void
NInputGate::executeProcess() 
{

}

void
NInputGate::finalize(){}

const string&
NInputGate::getName() const{return myObjectName;}

void
NInputGate::updateOutput() 
{
  
  NInputGateState *state = (NInputGateState *) getState();
  
  for(int i = 0; i < numberOfInputs; i++)
    {
      state->inputBits[i] = 1;
    }
  
  cout<<"numberOfInputs is:"<< numberOfInputs << endl;
  
  state->outputBit1 = 0;
  ////////////////////////////////////////////////////////
  
  IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());
  LogicEvent *firstEvent = new LogicEvent(sendTime,
                                          sendTime + 1,
                                          this,
					  outputHandles[0]);
  
  firstEvent->setbitValue1(0);
  firstEvent->setbitValue2(0);
  firstEvent->setsourcePort(0);
  firstEvent->setdestinationPort(2);
  
  /*changes made by Manas 
    this->receiveEvent(firstEvent);
  */

  (outputHandles[0]) -> receiveEvent(firstEvent);
}

State*
NInputGate::allocateState() 
{
  return (State *) new NInputGateState(numberOfInputs);
}

void
NInputGate::deallocateState(const State *state)
{
  delete (NInputGateState *)state;
}

void
NInputGate::reclaimEvent(const Event *event)
{
  delete (LogicEvent *)event;
}

void
NInputGate::reportError(const string &msg, SEVERITY level){}
