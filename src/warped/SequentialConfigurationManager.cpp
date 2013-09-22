
#include "SequentialConfigurationManager.h"

SequentialConfigurationManager::SequentialConfigurationManager( Application *initApplication )
  : mySimulationManager( new SequentialSimulationManager( initApplication ) ),
    myEventSetFactory(new EventSetFactory()){}

SequentialConfigurationManager::~SequentialConfigurationManager(){
  delete myEventSetFactory;
  delete mySimulationManager;
}

void
SequentialConfigurationManager::configure( SimulationConfiguration &configuration ){
  mySimulationManager->configure( configuration );
}

SimulationManager *
SequentialConfigurationManager::getSimulationManager(){
  return mySimulationManager;
}
