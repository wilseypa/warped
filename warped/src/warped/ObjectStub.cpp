
#include "ObjectStub.h"
#include "FactoryImplementationBase.h"

ObjectStub::ObjectStub(FactoryImplementationBase *myFactory) {
  factory = myFactory;
}

FactoryImplementationBase *
ObjectStub::getParentFactory() const {
   return factory;
}

const Kind
ObjectStub::getKind() const {
   return STUB;
}

