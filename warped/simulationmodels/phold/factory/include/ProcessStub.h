#ifndef PHOLD_OBJECT_STUB_H
#define PHOLD_OBJECT_STUB_H

#include "warped.h"
#include "ObjectStub.h"

class SimulationObject;

class ProcessStub : public ObjectStub {
public:
   ProcessStub(FactoryImplementationBase *owner) : ObjectStub(owner){}
   ~ProcessStub(){};

   string &getName() const {
      static string name("Process");
      return name;
   }

   const string &getInformation() const {
      static string info("A Simple PHOLD Object");
      return info;
   }

   const bool isLocalObject() const {
      return true;
   }

   SimulationObject *createSimulationObject(int numberOfArguments,
                                            ostringstream &argumentStream);
};

#endif
