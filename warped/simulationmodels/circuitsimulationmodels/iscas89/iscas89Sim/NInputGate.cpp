#include "NInputGate.h"
#include "LogicEvent.h"
#include "NInputGateState.h"
#include "SimulationStream.h"
#include <sstream>

using namespace std;

NInputGate::NInputGate(string &objectName, const int numInputs,
                       const int numOutputs,vector<string> *outputs,
                       vector<int> *destInputPorts,
                       int objectDelay):LogicComponent(objectName, numOutputs,
                       outputs,destInputPorts, objectDelay),
                       numberOfInputs(numInputs),numInputValue(0){}

NInputGate::~NInputGate(){
   deallocateState(getState());
}

void
NInputGate::initialize() {
   //cout<<endl;
   //cout<<"this is "<<getName()<<endl;
   LogicComponent::initialize();
   NInputGateState *newState = (NInputGateState *) getState();
   for(int i = 0; i < numberOfInputs; i++){
     newState->inputBits[i] = 0;
   }
   //cout<<"numberOfInputs is:"<<numberOfInputs<<endl;
   newState->outputBit1 = 0;
}

void
NInputGate::executeProcess() {
  //cout<<endl;
  //cout<<"in the executePorcess()"<<getName()<<endl;
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

