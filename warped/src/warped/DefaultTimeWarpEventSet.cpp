
#include "DefaultTimeWarpEventSet.h"
#include "DefaultTimeWarpEventContainer.h"
#include "DefaultTimeWarpEventContOneAntiMsg.h"
#include "Event.h"
#include "NegativeEvent.h"
#include "SimulationObject.h"
#include "TimeWarpSimulationManager.h"

using std::cerr;
using std::endl;

DefaultTimeWarpEventSet::DefaultTimeWarpEventSet( TimeWarpSimulationManager *initSimManager, 
                                                  bool usingOneAntiMsg ) :
  mySimulationManager( initSimManager ) {
 
  if(usingOneAntiMsg){ 
    for(int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++){
      events.push_back( new DefaultTimeWarpEventContOneAntiMsg() );
    }
  }
  else{
    for(int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++){
      events.push_back( new DefaultTimeWarpEventContainer() );
    }
  }
}

DefaultTimeWarpEventSet::~DefaultTimeWarpEventSet() {
  for(int i = 0; i < events.size(); i++){
    delete events[i];
  }
};

DefaultTimeWarpEventContainer &
DefaultTimeWarpEventSet::getEventContainer( const OBJECT_ID *objectID ){
	return *(events[objectID->getSimulationObjectID()]);
}

bool
DefaultTimeWarpEventSet::insert( const Event *event ){
  bool retval = false;
  ASSERT( event != 0 );
  SimulationObject *object = mySimulationManager->getObjectHandle( event->getReceiver() );
  ASSERT( object != 0 );
  if( event->getReceiveTime() < object->getSimulationTime() ){
    retval = true;
  }
  getEventContainer( object->getObjectID() ).insert( event );
  return retval;
}

bool
DefaultTimeWarpEventSet::handleAntiMessage( SimulationObject *eventFor,
                                            const NegativeEvent *cancelEvent ){
  return getEventContainer( eventFor->getObjectID() ).handleAntiMessage( eventFor, cancelEvent );
}

void
DefaultTimeWarpEventSet::fossilCollect( const Event *toRemove ){
  getEventContainer(&toRemove->getReceiver()).fossilCollect(toRemove);
}

const Event *
DefaultTimeWarpEventSet::getEvent( SimulationObject *object ){
  ASSERT( object != 0 );
  return getEventContainer( object->getObjectID() ).getNextEvent();
}

const Event *
DefaultTimeWarpEventSet::getEvent(SimulationObject *object, 
				  const VTime &minimumTime ){
  const Event *retval = 0;
  ASSERT( object != 0 );
  const Event *peeked = peekEvent( object, minimumTime );
  if( peeked != 0 ){
    retval = getEvent( object );
    ASSERT( peeked == retval );
  }
  return retval;
}

const Event *
DefaultTimeWarpEventSet::peekEvent(SimulationObject *object ){
  ASSERT( object != 0 );
  return getEventContainer( object->getObjectID() ).nextEvent();
}

const Event *
DefaultTimeWarpEventSet::peekEvent( SimulationObject *object, 
				    const VTime &minimumTime ){
  ASSERT( object != 0 );
  DefaultTimeWarpEventContainer &objectContainer = getEventContainer( object->getObjectID() );
  const Event *retval = objectContainer.nextEvent();
  if( retval != 0 ){
    if( !(retval->getReceiveTime() < minimumTime) ){
      retval = 0;
    }
  }
  return retval;
}

void
DefaultTimeWarpEventSet::fossilCollect( SimulationObject *object,
					 const VTime &collectTime ){
  getEventContainer( object->getObjectID() ).fossilCollect( object, collectTime );
}

void
DefaultTimeWarpEventSet::fossilCollect( SimulationObject *object, int collectTime ){
  getEventContainer( object->getObjectID() ).fossilCollect( object, collectTime );
}

void
DefaultTimeWarpEventSet::rollback( SimulationObject *object,
				   const VTime &rollbackTime ){
  getEventContainer( object->getObjectID() ).rollback( rollbackTime );
}

bool
DefaultTimeWarpEventSet::inThePast( const Event *toCheck ){
  return getEventContainer( &toCheck->getReceiver() ).isInThePast( toCheck );
}

void
DefaultTimeWarpEventSet::ofcPurge(){
  SimulationObject *obj = NULL;
  for(int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++){
    obj = mySimulationManager->getObjectHandle(ObjectID(i,mySimulationManager->getSimulationManagerID()));
    (events[i])->ofcPurge(obj);
  }
}

void
DefaultTimeWarpEventSet::debugDump( const string &objectName, ostream &os ){
  os << "\n---------------\n" << objectName << "\n";
  getEventContainer( &mySimulationManager->getObjectId(objectName) ).debugDump( os );
}
