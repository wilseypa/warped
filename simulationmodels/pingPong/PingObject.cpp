#include <string>
#include "PingEvent.h"
#include "PingObject.h"
#include "PingObjectState.h"
#include <warped/SimulationManager.h>
#include <warped/warped.h>
#include <warped/IntVTime.h>

#include <sstream>
using std::stringstream;

PingObject::PingObject( int myId,
			const string &initDestObjectName,
			const int initNumberOfBalls,
			bool master,
			bool initRandomDelays ):
   myObjectName( getName( myId )), 
   myDestObjectName( initDestObjectName ), 
   numBalls( initNumberOfBalls ),
   sendTo( 0 ),
   isMaster( master ),
   randomDelays( initRandomDelays ){}

PingObject::~PingObject(){
  //The final fossil collection seems to handle
  //deleting the current state so deleting here
  //is not necessary
  //deallocateState(getState());
}

void
PingObject::initialize(){
  sendTo = getObjectHandle(myDestObjectName);
  // object_0 kick starts the simulation
  if( isMaster ){
    startBall();
  }
}

void
PingObject::startBall(){
  PingObjectState* myState = static_cast<PingObjectState*>(getState());
  myState->ballStarted();
  sendEvent( getName() );
}

void
PingObject::sendEvent( const string &owner ){
  //we want to send another event
  IntVTime recvTime = dynamic_cast<const IntVTime&> (getSimulationTime()) + 1;
  Event *newEvent =  new PingEvent( getSimulationTime(),
				    recvTime,
				    this,
				    sendTo,
				    owner );
  sendTo->receiveEvent( newEvent );
  PingObjectState* myState = static_cast<PingObjectState*>(getState());
  myState->ballSent();
}

void
PingObject::executeProcess(){
  PingObjectState *myState = static_cast<PingObjectState*>(getState());
  ASSERT(myState != 0);

  // If we're a master and we've started less balls than we're supposed to
  // have we'll send a new one on.
  if( isMaster ){
    if( myState->getNumStarted() < numBalls ){
      startBall();
    }
    // Else we're finished.
  }

  ASSERT( haveMoreEvents() == true );
  while(haveMoreEvents() == true){ 
    //we got an event
    //let's get the event
    const PingEvent *eventReceived = dynamic_cast<const PingEvent *>(getEvent());
    ASSERT( eventReceived != 0 );
    myState->ballReceived();
    if( randomDelays ){
      // We want reproducibility
      srandom( 42 );
      long maxCount = (long)((double)random()/(double)RAND_MAX)*10000000;
      double x = 1237.0;
      for( long i = 0; i < maxCount; i++ ){
	x = x / 3.3;
      }
      // Just to hopefully keep the compiler from optimizing this out.
      lastX = x;
    }
    // If this is a master object (somone capable of starting balls)
    if( isMaster ){
      // and this is NOT a ball we started already?
      if( eventReceived->getOwner() != getName() ){
		// Then we'll send it on.
		sendEvent( eventReceived->getOwner() );
      }
    }
     // If we are not a master we couldnt have started the ball so send it on
    else{
      sendEvent( eventReceived->getOwner() );
    }
  }
}

void
PingObject::finalize(){
  SEVERITY severity = NOTE;
  //simulation is over 
  //let's see how we did  
  PingObjectState* myState = static_cast<PingObjectState*>(getState());
  ASSERT(myState != NULL);
  
  string msg = myObjectName + " " + myState->getSummaryString() + "\n";
  
  reportError( msg, severity );
}


State*
PingObject::allocateState() {
  return new PingObjectState();
}

void
PingObject::deallocateState( const State *state ){
   // delete state
   // HINT: you could insert this in a free pool of states
	delete state;
}

void
PingObject::reclaimEvent(const Event *event){
   // delete event
   // HINT: you could insert this in a free pool of event
   delete event;
}

string 
PingObject::getName( int forId ){
   stringstream out;
   out << forId;
  return "PlayerObject" + out.str();
}
