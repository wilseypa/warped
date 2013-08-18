#include "PingPongApplication.h"
#include "PingObject.h"
#include "PingEvent.h"
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
#include <warped/DeserializerManager.h>

#include <vector>
using std::string;

PingPongApplication::PingPongApplication( unsigned int initNumObjects,
					  unsigned int initNumEventsPerObject,
					  unsigned int initNumBallsAtOnce,
					  bool initRandomDelays ) 
  : numObjects( initNumObjects ),
    numEventsPerObject( initNumEventsPerObject ),
    numBallsAtOnce( initNumBallsAtOnce ),
    randomDelays( initRandomDelays ){}

int 
PingPongApplication::getNumberOfSimulationObjects(int mgrId) const { 
  return numObjects;
}

vector<SimulationObject *> *
PingPongApplication::getSimulationObjects(){
  vector<SimulationObject *> *retval = new vector<SimulationObject *>;
  retval->reserve( numObjects );

  // The first and last objects are special...
  int mastersLeft = numBallsAtOnce;
  bool master = false;
  for( unsigned int i = 0; i < numObjects - 1; i++ ){
    if( mastersLeft-- > 0 ){
      master = true;
    }
    else{
      master = false;
    }
    retval->push_back( new PingObject( i,
				       PingObject::getName( i + 1 ),
				       numEventsPerObject,
				       master,
				       randomDelays
				       ) );
  }
  if( mastersLeft-- > 0 ){
    master = true;
  }
  else{
    master = false;
  }
  retval->push_back( new PingObject( numObjects - 1,
				     PingObject::getName( 0 ),
				     numEventsPerObject,
				     master,
				     randomDelays ) );
  
  return retval;
}


const PartitionInfo *
PingPongApplication::getPartitionInfo( unsigned int numberOfProcessorsAvailable ){
  const PartitionInfo *retval = 0;

  Partitioner *myPartitioner = new RoundRobinPartitioner();
  // Now we'll create some simulation objects...
  vector<SimulationObject *> *objects = getSimulationObjects();
  retval = myPartitioner->partition( objects, numberOfProcessorsAvailable );
  delete objects;

  return retval;
}

int 
PingPongApplication::finalize(){ 
  return 0; 
}

void 
PingPongApplication::registerDeserializers(){
  DeserializerManager::instance()->registerDeserializer( PingEvent::getPingEventDataType(),
							 &PingEvent::deserialize );

}

