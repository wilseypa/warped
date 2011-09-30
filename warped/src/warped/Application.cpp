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

int
Application::initialize(int argc, vector<string> *parameters) {
  if (argc < 4) {
    return 1;
  }
  
  vector<string>::iterator iter = parameters->begin();
  vector<string>::iterator iter_end = parameters->end();
  
  string path((*parameters)[0]);
  
  while (iter != iter_end) {
    if ((*iter) == "-simulate") {
      if ( (++iter) == iter_end) {
	// Oops! we are looking for more parameters
	return 2;
      }
      if ( (++iter) == iter_end) {
	return 3;
      }
      const char* tempString = (*iter).c_str();
      simulate_label = new char[strlen(tempString) + 1];
      strcpy(simulate_label, tempString);
    }
    else {
      iter++;
    }
  }

  myFactoryManager = FactoryManager::createUserFactory();
  
  return 0; // No errors!
}

// This function returns the total number of objects present in the simulation
int
Application::getNumberOfSimulationObjects(int) {
  //  return (elaboratedModule->getComponentInstantiations()->getNumberOfSymbols() - 1);
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
