#include "PingObjectStub.h"
#include "PingFactoryManager.h"
#include "FactoryImplementationBase.h"

PingFactoryManager::PingFactoryManager(FactoryImplementationBase *parent) : FactoryManager(parent){
   add(new PingObjectStub(this));
}

// This is a static method defined in FactoryManager that the user must
// define (or compilation errors will result)
// This is how the simulation kernel gets a handle to the user factory
// that needs to be instantiated.
FactoryManager*
FactoryManager::createUserFactory(){
   return ((FactoryManager *)new PingFactoryManager(NULL));
}
