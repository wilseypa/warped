#ifndef APPLICATION_ADAPTER_H
#define APPLICATION_ADAPTER_H

#include "Application.h"

class ApplicationAdapter : public Application {
public:
    ApplicationAdapter() {}

    std::vector<SimulationObject*>* getSimulationObjects(unsigned int numProcessorsAvailable) 
        { return new std::vector<SimulationObject*>; }
    virtual int finalize() { return 0; }
    ~ApplicationAdapter() {}
    void configure(SimulationConfiguration& configuration) {}
    void registerDeserializers() {}
    const VTime& getPositiveInfinity() { return IntVTime::getIntVTimePositiveInfinity(); }
    const VTime& getZero() { return IntVTime::getIntVTimeZero(); }
    const VTime& getTime(std::string&) { return IntVTime::getIntVTimeZero(); }

protected:

private:
};

#endif
