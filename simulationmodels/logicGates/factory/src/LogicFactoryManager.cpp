#include "LogicFactoryManager.h"
#include "FactoryImplementationBase.h"

#include "NInputAndGateStub.h"
#include "NInputXorGateStub.h"
#include "NInputOrGateStub.h"
#include "NAryBitGeneratorStub.h"
#include "NAryBitPrinterStub.h"
#include "HalfAdderStub.h"
#include "NotGateStub.h"

LogicFactoryManager::LogicFactoryManager(FactoryImplementationBase *parent)
   : FactoryManager(parent) {
   add(new NInputAndGateStub(this));
   add(new NInputXorGateStub(this));
   add(new NInputOrGateStub(this));
   add(new NAryBitGeneratorStub(this));
   add(new NAryBitPrinterStub(this));
   add(new HalfAdderStub(this));
   add(new NotGateStub(this));
}

// This is a static method defined in FactoryManager that the user must
// define (or compilation errors will result)
// This is how the simulation kernel gets a handle to the user factory
// that needs to be instantiated.
FactoryManager*
FactoryManager::createUserFactory(){
   return ((FactoryManager *)new LogicFactoryManager(NULL));
}

