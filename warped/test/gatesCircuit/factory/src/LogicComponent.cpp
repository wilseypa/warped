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
// $Id: LogicComponent.cpp
// 
//---------------------------------------------------------------------------

#include "../include/LogicComponent.h"
#include <iostream>

using namespace std;

LogicComponent::LogicComponent(string &objectName, const int numOutputs,
                               vector<string> outputs,
                               vector<int> *destInputPorts,
                               vector<int> *fanOutSize,
                               int objectDelay)
   : myObjectName(objectName), numberOfOutputs(numOutputs),
     outputNames(outputs), destinationInputPorts(destInputPorts),
     fanOuts(fanOutSize), delay(objectDelay), outputHandles(NULL){} 


LogicComponent::~LogicComponent(){
  /*if(numberOfOutputs != 0){
      delete outputNames; question - how can it be a pointer ??
      delete [] outputHandles;
      delete destinationInputPorts;
      delete fanOuts;
      }*//*changes made by Manas */
}

void
LogicComponent::initialize(){
  /*   if(numberOfOutputs != 0){
       outputHandles = new SimulationObject *[numberOfOutputs];
       for (int i = 0; i < numberOfOutputs; i++) {
       outputHandles[i] = getObjectHandle((*outputNames)[i]);
       }
       }// else do nothing. this object is a sink */
}

void
LogicComponent::finalize(){}

void
LogicComponent::executeProcess(){}

State*
LogicComponent::allocateState(){
  // cerr << "LogicComponent::allocateState() called !!" << endl;
   return NULL;
}

void
LogicComponent::deallocateState(const State *state){}

void
LogicComponent::reclaimEvent(const Event *event){}

void
LogicComponent::reportError(const string &msg, SEVERITY level){}

/*const string &
LogicComponent::getName()const{}*/ 

void
LogicComponent::sendEvent(const int output, LogicEvent *event) 
{
  LogicEvent *newEvent = NULL;
  int fanouts = (*fanOuts)[output];
  // cout<<(*fanOuts)[0]<<endl;
  int portStartNumber = 0;
  
  for(int i = 0; i < output; i++)
    {
      portStartNumber += (*fanOuts)[i];
    }
  
  for(int count = 0; count < fanouts; count++)
    {
      IntVTime sendTime = static_cast<const IntVTime&>(getSimulationTime());
      double ldelay = 1.0;
      IntVTime recvTime = sendTime +(int)ldelay;
      SimulationObject *receiver = outputHandles[count+portStartNumber];      
      // if(receiver)
      cout<<"is not here!"<<endl;
      newEvent = new LogicEvent(sendTime,
                                recvTime,
                                this,
                                this);
      cout<<"guess here !" <<endl;
      // newEvent = new LogicEvent(0);
      // memcpy(newEvent, event, event->getSize());
      newEvent->bitValue1 = event->bitValue1;
      newEvent->bitValue1 = event->bitValue1;
      newEvent->sourcePort = output+1;
      newEvent->destinationPort = (*destinationInputPorts)[count+portStartNumber];
      // outputHandles[count+portStartNumber]->receiveEvent(newEvent, this);
      receiver->receiveEvent(newEvent);
      cout<<"send event!!"<<endl;
    }
}
