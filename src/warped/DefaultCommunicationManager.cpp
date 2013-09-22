
#include "DefaultCommunicationManager.h"

DefaultCommunicationManager::DefaultCommunicationManager( PhysicalCommunicationLayer *initPhysicalLayer, 
							  TimeWarpSimulationManager *initSimulationManager) :
  CommunicationManagerImplementationBase( initPhysicalLayer, initSimulationManager ){};

DefaultCommunicationManager::~DefaultCommunicationManager(){};

 
