#include "../include/ProcessStub.h"
#include "../include/PHOLDFactoryManager.h"
#include "FactoryImplementationBase.h"

PHOLDFactoryManager::PHOLDFactoryManager(FactoryImplementationBase *parent) : FactoryManager(parent){
   add(new ProcessStub(this));
}

// This is a static method defined in FactoryManager that the user must
// define (or compilation errors will result)
// This is how the simulation kernel gets a handle to the user factory
// that needs to be instantiated.
FactoryManager*
FactoryManager::createUserFactory(){
   return ((FactoryManager *)new PHOLDFactoryManager(NULL));
}

