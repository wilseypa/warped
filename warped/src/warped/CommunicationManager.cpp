// See copyright notice in file Copyright in the root directory of this archive.

#include "Event.h"
#include "CommunicationManager.h"
#include "SimulationObject.h"
#include <sstream>
using std::istringstream;

CommunicationManager::CommunicationManager( PhysicalCommunicationLayer *physicalLayer,
					    TimeWarpSimulationManager * ) :
  myPhysicalCommunicationLayer(physicalLayer),
  numCatastrophicRollbacks(0),
  recoveringFromCkpt(false){
}

CommunicationManager::~CommunicationManager(){
	delete myPhysicalCommunicationLayer;
}

void
CommunicationManager::finalize(){
  myPhysicalCommunicationLayer->physicalFinalize();
}

void
CommunicationManager::registerMessageType( const string &messageType,
					   CommunicatingEntity *entity ){
  // store info in the list of receivers hash map
  listOfReceivers.insert( std::make_pair(messageType, entity));
}

void   
CommunicationManager::configure( SimulationConfiguration &configuration ){
  initializePhysicalCommunicationLayer( configuration );
  initializeCommunicationManager();
}
