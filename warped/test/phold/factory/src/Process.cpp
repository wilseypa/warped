// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "../include/Process.h"
#include "../include/ProcessState.h"
#include "SimulationManager.h"
#include "TimeWarpSimulationManager.h"
#include "IntVTime.h"
#include <iostream>
#include <sstream>
#include <time.h>
using namespace std;

Process::Process(unsigned int procNr, string &name, unsigned int nrOfOutputs, 
                 vector<string> outputs, unsigned int stateSize, unsigned int numBalls,
                 distribution_t dist, double initCompGrain, double seed): 
  processNumber(procNr), myObjectName(name),  numberOfOutputs(nrOfOutputs), 
  outputNames(outputs), sizeOfState(stateSize), numberOfTokens(numBalls), 
  compGrain(initCompGrain), sourceDistribution(dist), first(seed), second(0.0) { 
}

Process::~Process() { 
  deallocateState(getState());
};

void
Process::initialize() {
   outputHandles = vector<SimulationObject *>(outputNames.size(),0);
   for (int i = 0; i < outputNames.size(); i++) {
      outputHandles[i] = getObjectHandle(outputNames[i]);
   }

   ProcessState* myState = dynamic_cast<ProcessState *>( getState() );
   ASSERT(myState != NULL);

   delete myState->gen; // delete old copy
   myState->gen = new MLCG(processNumber, (processNumber + 1));
   for (int i = 0; i < myState->sizeOfStateData; i++) {
      myState->stateBulk[i] = '1';
   }

   // Process numbers have to be 0, 1, 2, ... to get a fully ordered
   // and sequenced set of event numbers in steps of 1.
   int eventNumberBase = numberOfTokens * processNumber;
   for(int i = 1; i <= numberOfTokens; i++ ) {
      IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());
      int ldelay = msgDelay();
      PHOLDEvent *event = new PHOLDEvent(sendTime, sendTime + 1 + ldelay, this, this);
      event->eventNumber = eventNumberBase + i;

      receiveEvent(event);
      myState->eventSent();
   }
}

void
Process::finalize(){
  SEVERITY severity = NOTE;

  ProcessState* myState = dynamic_cast<ProcessState*>(getState());
  ASSERT(myState != NULL);
    
  ostringstream oss;
  oss << "  Sent: " << myState->getNumSent() << " Received: " << myState->getNumReceived();
   
  string msg = myObjectName + oss.str() + "\n";
  cout << msg;
  reportError( msg, severity ); 
}

void 
Process::executeProcess(){  
   IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());
   
   do { 
      PHOLDEvent* recvEvent = (PHOLDEvent*) getEvent();
    
      if( recvEvent != NULL ){
        warped64_t util_start;
        TimeWarpSimulationManager* twsm =
                dynamic_cast<TimeWarpSimulationManager*>(mySimulationManager);
        if(twsm)
          util_start = rdtsc();

         ProcessState* myState = (ProcessState *) getState();
         myState->eventReceived();

         // Generate the destination for the event.
         DiscreteUniform Dest(0, numberOfOutputs-1, myState->gen);
         int myDestination = (int) Dest();

         ASSERT (myDestination < numberOfOutputs);
         SimulationObject *receiver = outputHandles[myDestination];

         // Generate the delay between the send and receive times.
         int ldelay = msgDelay();
         IntVTime recvTime = sendTime + 1 + ldelay;

         PHOLDEvent* newRequest = new PHOLDEvent(sendTime, recvTime, this, receiver);

         newRequest->numberOfHops = recvEvent->numberOfHops + 1;
         newRequest->eventNumber = recvEvent->eventNumber;

         computationGrain();
         if(twsm) {
           int util = rdtsc() - util_start;
           twsm->doDelay(util);
           recvEvent->setWork(util);
           addEffectiveWork(util);
         }

         receiver->receiveEvent(newRequest);
         myState->eventSent();
      }
   } while(haveMoreEvents() == true); // end of while loop
}

int 
Process::msgDelay() {

  double ldelay;

  ProcessState* myState = (ProcessState *) getState();

  switch(sourceDistribution){
  case UNIFORM :
    {
      Uniform uniform(first, second, myState->gen);
      ldelay = uniform();
      break;
    }
  case NORMAL :
    {
      Normal normal(first, second, myState->gen);
      ldelay = normal();
      break;
    }
  case BINOMIAL :
    {
      Binomial binomial((int)first, second, myState->gen);
      ldelay = binomial();
      break;
    }
  case POISSON :
    {
      Poisson poisson(first, myState->gen);
      ldelay = poisson();
      break;
    }
  case EXPONENTIAL :
    {
      NegativeExpntl expo(first, myState->gen);
      ldelay = expo();
      break;
    }
  case FIXED :
    ldelay = (int)first ;
    break;
  default :
    ldelay = 0;
    cerr << "Improper Distribution for a Source Object!!!" << "\n";
    break;
  }
  return ( (int) ldelay );
     
}

void
Process::computationGrain(){
  // Perform the designated number of double divisions.
  double x = 123731.1;
  for( unsigned int i = 0; i < compGrain; i++ ){
    x = x / 3.3;
  }

  // This is here to prevent the compiler from optimizing everything out.
  lastX = x;
}

State*
Process::allocateState() {
  return new ProcessState(sizeOfState);
}

void
Process::deallocateState(const State* state){
  delete state;
}

void
Process::reclaimEvent(const Event *event){
   delete event;
}

void
Process::reportError(const string&, SEVERITY){
}
