#ifndef RAID_OBJECT_STUB_H
#define RAID_OBJECT_STUB_H

#include "warped.h"
#include "ObjectStub.h"

class SimulationObject;

/** The class RAIDProcessStub.
*/
class RAIDProcessStub : public ObjectStub {
public:
   RAIDProcessStub(FactoryImplementationBase *owner) : ObjectStub(owner){}
   ~RAIDProcessStub(){};

   string &getName() const {
      static string name("RAIDProcess");
      return name;
   }

   const string &getInformation() const {
      static string info("A Simple RAID Object");
      return info;
   }

   const bool isLocalObject() const {
      return true;
   }

   SimulationObject *createSimulationObject(int numberOfArguments,
                                            ostringstream &argumentStream);
};

#endif
