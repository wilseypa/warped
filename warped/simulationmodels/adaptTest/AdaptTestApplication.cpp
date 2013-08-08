// See copyright notice in file Copyright in the root directory of this archive.

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

AdaptTestApplication::AdaptTestApplication( unsigned int initNumObjects,
                                            unsigned int initNumStragglers,
                                            string initOutputMode,
					    bool initAdaptiveState ) 
  : numObjects( initNumObjects),
    numStragglers( initNumStragglers ),
    outputMode( initOutputMode ),
    adaptiveState( initAdaptiveState ){}

int 
AdaptTestApplication::initialize( vector<string> &arguments ){ 
  try {
    TCLAP::CmdLine cmd("Adapt Test Simulation");

    TCLAP::ValueArg<int> numStragglersArg("", "numStragglers", "number of stragglers", 
                                     false, numStragglers, "int", cmd);
    TCLAP::SwitchArg adaptiveStateArg("", "adaptiveState", "should the object execution and state saving time vary", 
                                      cmd, false);

    std::vector<string> allowed {"lazy", "aggr", "adapt"};
    TCLAP::ValuesConstraint<string> constraint(allowed);
    TCLAP::ValueArg<string> outputModeArg("", "outputMode", "event output mode", 
                                       false, outputMode, &constraint, cmd);

    cmd.parse(arguments);

    numStragglers = numStragglersArg.getValue();
    adaptiveState = adaptiveStateArg.getValue();
    outputMode = outputModeArg.getValue();

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    exit(-1);
  }

  return 0;
}

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