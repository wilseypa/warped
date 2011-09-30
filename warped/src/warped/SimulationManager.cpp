// See copyright notice in file Copyright in the root directory of this archive.

#include "SimulationManager.h"

SimulationManager::SimulationManager(){}

SimulationManager::~SimulationManager(){}

void 
SimulationManager::reportError( const string& msg, const SEVERITY level ){
  std::cerr << level << ": " << msg;
}
