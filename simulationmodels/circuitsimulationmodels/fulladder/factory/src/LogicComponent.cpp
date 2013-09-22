
#include "../include/LogicComponent.h"
#include <iostream>

using namespace std;

LogicComponent::LogicComponent(string &objectName, const int numOutputs,
                               vector<string> *outputs,
                               vector<int> *destInputPorts,
                               int objectDelay)
                               :myObjectName(objectName), numberOfOutputs(numOutputs),
                                outputNames(outputs), destinationInputPorts(destInputPorts),
                                delay(objectDelay), outputHandles(NULL){} 


LogicComponent::~LogicComponent(){ }


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
   return NULL;
}

void
LogicComponent::deallocateState(const State *state){}

void
LogicComponent::reclaimEvent(const Event *event){}

void
LogicComponent::reportError(const string &msg, SEVERITY level){}

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
  newEvent = new LogicEvent(sendTime,recvTime,this,receiver);
  newEvent->bitValue = outputBitValue;
  cout<<"OutputBitValue is "<<outputBitValue<<endl;
  newEvent->sourcePort = 1;
  newEvent->destinationPort = (*destinationInputPorts)[count];
  cout<<"des port is :"<<newEvent->destinationPort<<endl;
  receiver->receiveEvent(newEvent);
 }
}

