
#include <iostream>                     // for operator<<, ostream, etc

#include "SimulationManager.h"

SimulationManager::SimulationManager() {}

SimulationManager::~SimulationManager() {}

void
SimulationManager::reportError(const string& msg, const SEVERITY level) {
    std::cerr << level << ": " << msg;
}
