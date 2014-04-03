
#include <sstream>                      // for istringstream
#include <utility>                      // for pair, make_pair

#include "CommunicationManager.h"

class CommunicatingEntity;
class SimulationConfiguration;

using std::istringstream;

CommunicationManager::CommunicationManager(PhysicalCommunicationLayer* physicalLayer,
                                           TimeWarpSimulationManager*) :
    myPhysicalCommunicationLayer(physicalLayer),
    numCatastrophicRollbacks(0),
    recoveringFromCkpt(false) {
}

CommunicationManager::~CommunicationManager() {
    delete myPhysicalCommunicationLayer;
}

void
CommunicationManager::finalize() {
    myPhysicalCommunicationLayer->physicalFinalize();
}

int
CommunicationManager::getSize() {
    myPhysicalCommunicationLayer->physicalGetSize();
}

void
CommunicationManager::registerMessageType(const string& messageType,
                                          CommunicatingEntity* entity) {
    // store info in the list of receivers hash map
    listOfReceivers.insert(std::make_pair(messageType, entity));
}

void
CommunicationManager::configure(SimulationConfiguration& configuration) {
    initializePhysicalCommunicationLayer();
    initializeCommunicationManager();
}
