
#include "DefaultCommunicationManager.h"

class PhysicalCommunicationLayer;
class TimeWarpSimulationManager;

DefaultCommunicationManager::DefaultCommunicationManager(PhysicalCommunicationLayer*
                                                         initPhysicalLayer,
                                                         TimeWarpSimulationManager* initSimulationManager) :
    CommunicationManagerImplementationBase(initPhysicalLayer, initSimulationManager) {};

DefaultCommunicationManager::~DefaultCommunicationManager() {};


