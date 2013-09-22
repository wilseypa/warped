#ifndef PHOLD_FACTORY_MANAGER_H
#define PHOLD_FACTORY_MANAGER_H

#include "warped.h"
#include "FactoryManager.h"

class FactoryImplementationBase;

class PHOLDFactoryManager : public FactoryManager {
public:
  PHOLDFactoryManager(FactoryImplementationBase *parent);
  ~PHOLDFactoryManager() {};

  const string& getName() const {
     static string name("PHOLDFactoryManager");
     return name;
  }
};

#endif

