#include "NInputGate.h"
#include "LogicEvent.h"
#include "NInputGateState.h"

NInputGate::NInputGate(string &objectName, const int numInputs,
                       const int numOutputs, vector<string> *outputs,
                       vector<int> *destInputPorts,
                       vector<int> *fanOutSize, int objectDelay)
   : LogicComponent(objectName, numOutputs, outputs,
                    destInputPorts, fanOutSize, objectDelay),
                          numberOfInputs(numInputs){}

NInputGate::~NInputGate(){
   deallocateState(getState());
}

void
NInputGate::initialize() {
   LogicComponent::initialize();
   NInputGateState *newState = (NInputGateState *) getState();
   for(int i = 0; i < numberOfInputs; i++){
      newState->inputBits[i] = 0;
   }
   newState->outputBit1 = 0;
   
   IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());

   LogicEvent *firstEvent = new LogicEvent(sendTime,
                                           sendTime+1,
                                           this,
                                           this);
   
   this->receiveEvent(firstEvent);
}

void
NInputGate::executeProcess() {
  NInputGateState *state = static_cast<NInputGateState *>(getState());
 // LogicEvent *logicEvent  = NULL;
  LogicEvent *received  = NULL;
  int updateFlag = 0;
  do {
    logicEvent = (LogicEvent *) getEvent();
    state->inputBits[logicEvent->destinationPort - 1] = logicEvent->bitValue;
    cout << logicEvent->destinationPort << ": got bit value " << logicEvent->bitValue << endl;
    updateFlag = 1;
  } while (haveMoreEvents() == true);
  
  if (updateFlag == 1) {
    updateOutput();
  }
}

void
NInputGate::finalize(){}

void
NInputGate::updateOutput() {
  NInputGateState *state = static_cast<NInputGateState *> getState();

  LogicEvent* newEvent = new LogicEvent(getSimulationTime()+delay);

  state->outputBit1  = computeOutput(state->inputBits);
  newEvent->bitValue = state->outputBit1;
  
  sendEvent(0, newEvent);
  
  delete newEvent;
}

State*
NInputGate::allocateState() {
  return (State *) new NInputGateState(numberOfInputs);
}

void
NInputGate::deallocateState(State *state){
   delete (NInputGateState *)state;
}

void
NInputGate::reclaimEvent(const Event *event){
   delete (LogicEvent *)event;
}

void
NInputGate::reportError(const string &msg, SEVERITY level){}

