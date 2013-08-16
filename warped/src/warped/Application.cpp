// See copyright notice in file Copyright in the root directory of this archive.

#include "Application.h"
#include "FactoryManager.h"
#include "ObjectStub.h"

// This file provides a default definition of the createObjects() method
// that must be defined by each application that uses warped.

// global argument variables

char *simulate_label = NULL;

extern bool debugFlag;  // This variable get's set by warped kernel
extern int errors, warnings;  // These variables are used by warped kernel

/// This is the handle to the singleton factory manager.
FactoryManager *myFactoryManager = NULL;

// This function returns the total number of objects present in the simulation
int
Application::getNumberOfSimulationObjects(int) {
  //  return (elaboratedModule->getComponentInstantiations()->getNumberOfSymbols() - 1);
  return 0;
}
    
int
Application::finalize() {
  delete elaboratedModule;
  delete myFactoryManager;
  
  return 0;
}

void
Application::displayCommandLineParameters(ostream &os) {
  //  os << "  * -simulate        specify ssl_file_name and ssl_module to simulate\n";
}
