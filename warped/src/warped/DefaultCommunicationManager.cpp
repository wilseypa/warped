// See copyright notice in file Copyright in the root directory of this archive.

#include "DefaultCommunicationManager.h"

DefaultCommunicationManager::DefaultCommunicationManager( PhysicalCommunicationLayer *initPhysicalLayer, 
							  TimeWarpSimulationManager *initSimulationManager) :
  CommunicationManagerImplementationBase( initPhysicalLayer, initSimulationManager ){};

DefaultCommunicationManager::~DefaultCommunicationManager(){};

 
