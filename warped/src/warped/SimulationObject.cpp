// See copyright notice in file Copyright in the root directory of this archive.

#include "SimulationObject.h"
#include "SimulationManager.h"
#include "SimulationManagerImplementationBase.h"
#include "TimeWarpSimulationManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "Event.h"

using std::ios;
using std::cerr;
using std::endl;

SimulationObject::SimulationObject() : mySimulationManager(),
				       myObjectID(0),
				       localVirtualTime(0),
                                       eventIdVal(0),
                                       myState(0){}

SimulationObject::~SimulationObject(){
   delete myObjectID;
}

void
SimulationObject::receiveEvent( const Event *newEvent ){
  ASSERT( newEvent != 0 );
  ASSERT( newEvent->getReceiver() == *getObjectID() );
  getSimulationManager()->handleEvent( newEvent );
}

SerializedInstance*
SimulationObject::serializeEvent(Event* event){
  cerr << "Error: SimulationObject::serializeEvent called\n";
  cerr << "Event is " << event << endl;
  return NULL;
}

Event*
SimulationObject::deserializeEvent(SerializedInstance* instance){
  cerr << "Error: SimulationObject::deserializeEvent called\n";
  cerr << "Instance is " << instance << endl;
  return NULL;
}

SimulationObject*
SimulationObject::getObjectHandle( const string& objectToGet ) const {
  ASSERT(getSimulationManager() != NULL);
  // return a pointer to the simulation object
  return getSimulationManager()->getObjectHandle(objectToGet);
}

const Event*
SimulationObject::getEvent(){
  // return the next event for this simulation object
  ASSERT(getSimulationManager() != NULL);
  return (getSimulationManager()->getEvent(this));
}

bool
SimulationObject::haveMoreEvents(){
  // does this guy have any more events to process at this time ?
  const Event* event =  getSimulationManager()->peekEvent(this);
  if( event == 0 ){
    return false;
  }
  else {
     // return true only if the next event is for the same destination
     // and is at the current simulation time.
    if( event->getReceiveTime() == getSimulationTime() ){
      if( event->getReceiver() == *getObjectID() ){
	return true;
      }
      else {
	// this is an error condition in the decentralized queue case
	return false;
      }
    }
    else {
      return false;
    }
  }
}

const VTime &
SimulationObject::getSimulationTime() const {
  ASSERT( localVirtualTime != 0 );
  return *localVirtualTime;
}

void
SimulationObject::setSimulationTime( const VTime &newSimulationTime ){
  ASSERT( newSimulationTime >= newSimulationTime.getZero() );
  delete localVirtualTime;
  localVirtualTime = newSimulationTime.clone();
}


// get a handle to a simulation input stream
SimulationStream *
SimulationObject::getIFStream(const string &filename){
  ASSERT(getSimulationManager() != NULL);
  return getSimulationManager()->getIFStream(filename, this);
}
   
// get a handle to a simulation output stream
SimulationStream *
SimulationObject::getOFStream(const string &filename,
                              ios::openmode mode ){
  ASSERT(getSimulationManager() != NULL);
  return getSimulationManager()->getOFStream(filename, this, mode);
}

void 
SimulationObject::reportError( const string& msg, const SEVERITY level ){
  getSimulationManager()->reportError( msg, level );
}

   
// get a handle to a simulation input-output stream
SimulationStream *
SimulationObject::getIOFStream(const string &filename){
  ASSERT(getSimulationManager() != NULL);
  return getSimulationManager()->getIOFStream(filename, this);
}

void 
SimulationObject::setSimulationManager( SimulationManager *simMgr ){
  mySimulationManager = simMgr;
  setSimulationTime( simMgr->getZero() );
}
