#ifndef RAID_FORK_STUB_H
#define RAID_FORK_STUB_H

#include "warped.h"
#include "ObjectStub.h"

class SimulationObject;

/** The class RAIDForkStub.
*/
class RAIDForkStub : public ObjectStub {
public:
   RAIDForkStub(FactoryImplementationBase *owner) : ObjectStub(owner){}
   ~RAIDForkStub(){};

   string &getName() const {
      static string name("RAIDFork");
      return name;
   }

   const string &getInformation() const {
      static string info("A Simple RAID Fork Object");
      return info;
   }

   const bool isLocalObject() const {
      return true;
   }

   SimulationObject *createSimulationObject(int numberOfArguments,
                                            ostringstream &argumentStream);
};

#endif
