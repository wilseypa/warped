
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
// $Id: LogicComponent.cpp
// 
//---------------------------------------------------------------------------

#include "../include/LogicComponent.h"
#include <iostream>

using namespace std;

LogicComponent::LogicComponent(string &objectName, const int numOutputs,
                               vector<string> *outputs,
                               vector<int> *destInputPorts,
                               int objectDelay)
   : myObjectName(objectName), numberOfOutputs(numOutputs),
     outputNames(outputs), destinationInputPorts(destInputPorts),
     delay(objectDelay), outputHandles(NULL){} 


LogicComponent::~LogicComponent(){
   if(numberOfOutputs != 0){
      delete outputNames;
      delete [] outputHandles;
      delete destinationInputPorts;
   }
}

void
LogicComponent::initialize(){
   if(numberOfOutputs != 0){
      outputHandles = new SimulationObject *[numberOfOutputs];
      for (int i = 0; i < numberOfOutputs; i++) {
         outputHandles[i] = getObjectHandle((*outputNames)[i]);
				 cout<<"the gate's des gate is " << (*outputNames)[i]<<endl;
      }
   }// else do nothing. this object is a sink
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
LogicComponent::sendEvent(const int outputBitValue) {
   LogicEvent *newEvent = NULL;
   for(int count = 0; count < numberOfOutputs; count++){

   cout<<getName()<<" will send an event "<<endl;
	 
      IntVTime sendTime = static_cast<const IntVTime&>(getSimulationTime());
      double ldelay = 1.0;
      IntVTime recvTime = sendTime +(int)ldelay;
      SimulationObject *receiver = outputHandles[count]; 
			cout<<"the receiver is "<<receiver->getName()<<endl;
      newEvent = new LogicEvent(sendTime,
                                recvTime,
                                this,
                                receiver);
      newEvent->bitValue = outputBitValue;
		  cout<<"OutputBitValue is "<<outputBitValue<<endl;
      newEvent->sourcePort = 1;
      newEvent->destinationPort = (*destinationInputPorts)[count];
			cout<<"des port is :"<<newEvent->destinationPort<<endl;
      receiver->receiveEvent(newEvent);
    //  delete newEvent;
   }
}

