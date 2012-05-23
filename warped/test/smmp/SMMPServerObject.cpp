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

#include "SMMPServerObject.h"
#include "SMMPServerState.h"
#include "IntVTime.h"
#include "MemEvents.h"
using namespace std;

SMMPServerObject::SMMPServerObject(string initName, string initDest, double initSeed):
   myObjectName(initName),
   dest(initDest),
   serverDist(UNIFORM),
   delay(0),
   first(0),
   second(0),
   seed(initSeed){
}

SMMPServerObject::~SMMPServerObject(){
   deallocateState(getState());
}

void
SMMPServerObject::initialize() {
   SMMPServerState *myState = dynamic_cast<SMMPServerState*>(getState());
   //int id = getObjectID()->getSimulationObjectID();
   myState->gen = new MLCG(seed, seed + 1);
}

void
SMMPServerObject::finalize(){
}

void
SMMPServerObject::executeProcess(){ 
   MemRequest *recvEvent = NULL;
   IntVTime sendTime = static_cast<const IntVTime&> (getSimulationTime());
   SMMPServerState *myState = static_cast<SMMPServerState*>(getState());

   while(haveMoreEvents() == true){ 
      recvEvent = (MemRequest*)getEvent();

      if ( recvEvent != NULL ){
         switch(serverDist){
            case UNIFORM: {
               Uniform uniform(first, second, myState->gen);
               delay = uniform();
               break;
            }
            case NORMAL: {
               Normal normal(first, second, myState->gen);
               delay = normal();
               break;
            }
            case BINOMIAL: {
               Binomial binomial((int)first,second, myState->gen);
               delay = binomial();
               break;
            }
            case POISSON: {
               Poisson poisson(first, myState->gen);
               delay = poisson();
               break;
            }
            case EXPONENTIAL: {
               NegativeExpntl expo(first, myState->gen);
               delay = expo();
               break;
            }
            case FIXED: {
               delay = (int) first;
               break;
            }
            default: {
               cerr << "Invalid Delay type!!!!!!!!!!!!!!" << endl;
               delay = 0;
               break;
            }
         }       

         SimulationObject *receiver = getObjectHandle(dest);

         MemRequest *newEvent = new MemRequest(sendTime, sendTime + delay,
                                               this, receiver);

         newEvent->setStartTime(recvEvent->getStartTime());
         newEvent->setProcessor(recvEvent->getProcessor());

         receiver->receiveEvent(newEvent);
      }
   }  
}
 
void
SMMPServerObject::setServerDistribution(distribution_t distribution, 
                                        double firstParameter,
                                        double secondParameter,
                                        double (*func)() ) {
  serverDist = distribution ; 
  first = firstParameter ;        
  second = secondParameter ;       
  userFunction = func;
}

State*
SMMPServerObject::allocateState() {
  return new SMMPServerState();
}

void
SMMPServerObject::deallocateState( const State *state ){
   delete state;
}

void
SMMPServerObject::reclaimEvent(const Event *event){
   delete event;
}
