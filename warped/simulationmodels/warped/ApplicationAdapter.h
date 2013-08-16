#ifndef APPLICATION_ADAPTER_H
#define APPLICATION_ADAPTER_H

#include "warped/Application.h"

class ApplicationAdapter : public Application {
public:
  ApplicationAdapter() {}

  const PartitionInfo *getPartitionInfo( unsigned int numProcessorsAvailable ){ return 0; }
  int getNumberOfSimulationObjects(int mgrId) const { return 0; }
  virtual int finalize(){ return 0; }
  ~ApplicationAdapter(){}
  void configure( SimulationConfiguration &configuration ){}
  void registerDeserializers(){}
  const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(std::string&){ return IntVTime::getIntVTimeZero(); }

protected:

private:
};

#endif
