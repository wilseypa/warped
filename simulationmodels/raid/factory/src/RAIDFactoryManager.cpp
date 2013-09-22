#include "../include/RAIDProcessStub.h"
#include "../include/RAIDDiskStub.h"
#include "../include/RAIDForkStub.h"
#include "../include/RAIDFactoryManager.h"
#include "FactoryImplementationBase.h"

RAIDFactoryManager::RAIDFactoryManager(FactoryImplementationBase *parent) : FactoryManager(parent){
   add(new RAIDProcessStub(this));
   add(new RAIDDiskStub(this));
   add(new RAIDForkStub(this));
}

// This is a static method defined in FactoryManager that the user must
// define (or compilation errors will result)
// This is how the simulation kernel gets a handle to the user factory
// that needs to be instantiated.
FactoryManager*
FactoryManager::createUserFactory(){
   return ((FactoryManager *)new RAIDFactoryManager(NULL));
}

