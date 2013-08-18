#ifndef PING_FACTORY_MANAGER_H
#define PING_FACTORY_MANAGER_H

#include "warped.h"
#include "FactoryManager.h"

class FactoryImplementationBase;

class PingFactoryManager : public FactoryManager {
public:
  PingFactoryManager(FactoryImplementationBase *parent);
  ~PingFactoryManager() {};

  const string& getName() const {
     static string name("PingFactoryManager");
     return name;
  }
};

#endif

