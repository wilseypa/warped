// See copyright notice in file Copyright in the root directory of this archive.

#include <string>
#include "AdaptTestEvent.h"
#include "AdaptTestObject.h"
#include "AdaptTestObjectState.h"
#include <warped/SimulationManager.h>
#include <warped/warped.h>
#include <warped/IntVTime.h>
using namespace std;

const unsigned int numAdaptObjects = 6;

AdaptTestObject::AdaptTestObject( int myId,
			          const string &initDestObjectName,
			          const int initNumberOfStragglers,
			          bool initAdaptiveState,
                                  string initOutputMode ):
  myObjectName( getName( myId )), 
  myDestObjectName( initDestObjectName ), 
  numStragglers( initNumberOfStragglers ),
  sendTo( 0 ),
  rollbackOccurred( false ),
  rollbackCount( 0 ),
  adaptiveState( initAdaptiveState ),
  outputMode( initOutputMode){}

AdaptTestObject::~AdaptTestObject(){
  deallocateState(getState());
}

void
AdaptTestObject::initialize(){
  sendTo = getObjectHandle(myDestObjectName);
  IntVTime initTime = dynamic_cast<const IntVTime&> (getSimulationTime());

  // Setup the objects vector. The index into the vector returns the object id
  // of the object with name "Object#" where "#" is the object number and the index.
  string objName;
  objects.resize(numAdaptObjects);
  for(int i = 0; i < numAdaptObjects; i++){
    stringstream out;
    out << i;
    objName = "Object" + out.str();
    objects[i] = *(getObjectHandle(objName)->getObjectID());
  }
  
  if( *getObjectID() == objects[0]){
    startEvent();
  }
  else if( *getObjectID() == objects[3]){
    // Object3 starts the first event for the mini-pingpong ring.
    // This event will continue to circle around the ring for the duration
    // of the simulation.
    AdaptTestObjectState* myState = static_cast<AdaptTestObjectState*>(getState());
    Event *startNewEvent = new AdaptTestEvent( initTime,
                                               initTime + 5,
                                               this,
                                               sendTo );
    sendTo->receiveEvent( startNewEvent );
    myState->eventSent();
    myState->eventStarted(); 
  }
}

void
AdaptTestObject::startEvent(){
  IntVTime sendTime = dynamic_cast<const IntVTime&> (getSimulationTime());
  AdaptTestObjectState* myState = static_cast<AdaptTestObjectState*>(getState());

  Event *startNewEvent = new AdaptTestEvent( sendTime,
                                             sendTime + 20,
                                             this,
                                             sendTo );
  sendTo->receiveEvent( startNewEvent );
  myState->eventStarted();
  myState->eventSent();
  
  // Send an event back to itself that this object continues to
  // generate events.
  Event *repeatEvent = new AdaptTestEvent( sendTime,
                                           sendTime + 20,
                                           this,
                                           this );
  receiveEvent( repeatEvent );
}

void
AdaptTestObject::executeProcess(){
  AdaptTestObjectState *myState = static_cast<AdaptTestObjectState*>(getState());
  ASSERT(myState != 0);
  IntVTime sendTime = dynamic_cast<const IntVTime&> (getSimulationTime());

  ASSERT( haveMoreEvents() == true );
  while(haveMoreEvents() == true){ 
    const AdaptTestEvent *eventReceived = dynamic_cast<const AdaptTestEvent *>(getEvent());
    ASSERT( eventReceived != 0 );

    // This delay is used to test the adaptive state manager.
    if( adaptiveState ){
      double x = 35.39453234857;
      for(long double wait = -123456789.1234567890; wait < 4000000; wait = wait + 10000){
        x = wait / x;
      }
    }
  
    ObjectID senderObject = eventReceived->getSender(); 
 
    if( myState->getNumStragglersSent() < numStragglers ){
      
      if( *getObjectID() == objects[0] && myState->getNumSent() < 10000 ){
        // Object0 will generate 10000 events
        startEvent();
      }
      else if( *getObjectID() == objects[1] ){
        myState->eventReceived();

        // When the event is from Object3, it is the straggler event.
        // Do not send a new event for this event. Object1 is the main
        // object in this simulation. There are 3 modes that occur after
        // a rollback:
        //    1. Regenerate all the same events. (LAZY)
        //    2. Generate entirely different events. (AGGR)
        //    3. Generate the same events for half the simualtion, then
        //       different events for the second half of the simulation. (ADAPT)
        if(senderObject == objects[3]){
          rollbackCount++;
          if( outputMode == "lazy" ){
            rollbackOccurred = false;
          }
          else if( outputMode == "aggr" ){
            rollbackOccurred = !rollbackOccurred;
          }
          else if( outputMode == "adapt" ){
            if( rollbackCount <= numStragglers/2 ){
              rollbackOccurred = false;
            }
            else{
              rollbackOccurred = !rollbackOccurred;
            }
          }
          else{
            std::cerr << "Improper output mode. Must be lazy, aggr, or adapt.\n";
            abort();
          }
        }
        if(senderObject == objects[0]){
          
          if( !rollbackOccurred  ){ 
            Event *sameEvent = new AdaptTestEvent( sendTime,
                                                   sendTime + 20,
                                                   this,
                                                   sendTo );
            sendTo->receiveEvent( sameEvent );
            myState->eventSent();
          }
          else{
            Event *differentEvent = new AdaptTestEvent( sendTime,
                                                        sendTime + 19,
                                                        this,
                                                        sendTo );
            sendTo->receiveEvent( differentEvent );
            myState->eventSent();
                                                        
          }
        }
      }
      else if( *getObjectID() == objects[2] ){
        // Do nothing for Object2. This is a sink object.
        myState->eventReceived();
      }
      else if( *getObjectID() == objects[3] || *getObjectID() == objects[4] || *getObjectID() == objects[5] ){
        myState->eventReceived();

        // Send a straggler for every 100 events sent.
        if( *getObjectID() == objects[3] && myState->getNumSent() % 100 == 0 && sendTime > 0 ){
          SimulationObject *recv = getObjectHandle("Object1");
          Event *secondEvent = new AdaptTestEvent( sendTime,
                                                   sendTime + 1,
                                                   this,
                                                   recv );
          recv->receiveEvent( secondEvent );
          myState->eventSent();
          myState->stragglerSent();
        }
        
        Event *shortEvent = new AdaptTestEvent( sendTime,
                                                sendTime + 1,
                                                this,
                                                sendTo );
        sendTo->receiveEvent( shortEvent );
        myState->eventSent();    
      }
    }
  }
}

void
AdaptTestObject::finalize(){
  SEVERITY severity = NOTE;
  //simulation is over 
  //let's see how we did  
  AdaptTestObjectState* myState = static_cast<AdaptTestObjectState*>(getState());
  ASSERT(myState != NULL);
  
  string msg = myObjectName + " " + myState->getSummaryString() + "\n";
  
  reportError( msg, severity );
}


State*
AdaptTestObject::allocateState() {
  return new AdaptTestObjectState( adaptiveState );
}

void
AdaptTestObject::deallocateState( const State *state ){
   // delete state
   // HINT: you could insert this in a free pool of states
   delete state;
}

void
AdaptTestObject::reclaimEvent(const Event *event){
   // delete event
   // HINT: you could insert this in a free pool of event
   delete event;
}

string 
AdaptTestObject::getName( int forId ){
  return "Object" + to_string(forId);
}
