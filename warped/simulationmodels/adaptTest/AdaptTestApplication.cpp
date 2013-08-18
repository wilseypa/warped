#include "AdaptTestApplication.h"
#include "AdaptTestObject.h"
#include "AdaptTestEvent.h"
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
#include <warped/DeserializerManager.h>

#include <tclap/CmdLine.h>
#include <tclap/ValuesConstraint.h>
#include <vector>
using std::string;

AdaptTestApplication::AdaptTestApplication(unsigned int initNumObjects,
                                           unsigned int initNumStragglers,
                                           string initOutputMode,
                                           bool initAdaptiveState)
  : numObjects(initNumObjects),
    numStragglers(initNumStragglers),
    outputMode(initOutputMode),
    adaptiveState(initAdaptiveState) {}

int 
AdaptTestApplication::getNumberOfSimulationObjects(int mgrId) const { 
  return numObjects;
}

vector<SimulationObject *> *
AdaptTestApplication::getSimulationObjects(){
  vector<SimulationObject *> *retval = new vector<SimulationObject *>;
  retval->reserve( numObjects );

  for( unsigned int i = 0; i < 2; i++ ){
    retval->push_back( new AdaptTestObject( i,
				            AdaptTestObject::getName( i + 1 ),
				            numStragglers,
				            adaptiveState,
                                            outputMode ) );
  }
  retval->push_back( new AdaptTestObject( 2,
                                          AdaptTestObject::getName(2),
                                          numStragglers,
                                          adaptiveState,
                                          outputMode ) );
  
  for( unsigned int j = 3; j < 5; j++ ){

    retval->push_back( new AdaptTestObject( j,
				            AdaptTestObject::getName( j+1 ),
				            numStragglers,
				            adaptiveState,
                                            outputMode ) );
  }
  retval->push_back( new AdaptTestObject( 5,
                                          AdaptTestObject::getName(3),
                                          numStragglers,
                                          adaptiveState,
                                          outputMode ) );
  return retval;
}


const PartitionInfo *
AdaptTestApplication::getPartitionInfo( unsigned int numberOfProcessorsAvailable ){
  const PartitionInfo *retval = 0;

  Partitioner *myPartitioner = new RoundRobinPartitioner();
  // Now we'll create some simulation objects...
  vector<SimulationObject *> *objects = getSimulationObjects();
  retval = myPartitioner->partition( objects, numberOfProcessorsAvailable );
  delete objects;

  return retval;
}

int 
AdaptTestApplication::finalize(){ 
  return 0; 
}

void 
AdaptTestApplication::registerDeserializers(){
  DeserializerManager::instance()->registerDeserializer( AdaptTestEvent::getAdaptTestEventDataType(),
							 &AdaptTestEvent::deserialize );

}