#ifndef RAID_DISK_STUB_H
#define RAID_DISK_STUB_H

#include "warped.h"
#include "ObjectStub.h"

class SimulationObject;

/** The class RAIDDiskStub.
*/
class RAIDDiskStub : public ObjectStub {
public:
   RAIDDiskStub(FactoryImplementationBase *owner) : ObjectStub(owner){}
   ~RAIDDiskStub(){};

   string &getName() const {
      static string name("RAIDDisk");
      return name;
   }

   const string &getInformation() const {
      static string info("A Simple RAID Disk Object");
      return info;
   }

   const bool isLocalObject() const {
      return true;
   }

   SimulationObject *createSimulationObject(int numberOfArguments,
                                            ostringstream &argumentStream);
};

#endif
