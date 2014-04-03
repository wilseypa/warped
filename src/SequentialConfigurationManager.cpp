
#include "EventSetFactory.h"            // for EventSetFactory
#include "SequentialConfigurationManager.h"
#include "SequentialSimulationManager.h"

class SimulationConfiguration;
class SimulationManager;

SequentialConfigurationManager::SequentialConfigurationManager(Application* initApplication)
    : mySimulationManager(new SequentialSimulationManager(initApplication)),
      myEventSetFactory(new EventSetFactory()) {}

SequentialConfigurationManager::~SequentialConfigurationManager() {
    delete myEventSetFactory;
    delete mySimulationManager;
}

void
SequentialConfigurationManager::configure(SimulationConfiguration& configuration) {
    mySimulationManager->configure(configuration);
}

SimulationManager*
SequentialConfigurationManager::getSimulationManager() {
    return mySimulationManager;
}
