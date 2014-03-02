#ifndef APPLICATION_H
#define APPLICATION_H


#include <string>                       // for string
#include <vector>                       // for vector

#include "Configurable.h"               // for Configurable
#include "warped.h"

class PartitionInfo;
class SimulationConfiguration;
class SimulationObject;
class VTime;

/** The Application abstract class.

    This is an abstract class (which is actually never instantiated)
    that acts as a place holder for the createObjects() method.  This
    method must be defined by the application.  The method should
    return a pointer to the list of simulation objects that are
    involved in this simulation.
*/

class Application : public Configurable {
public:

    /** This method is called to get a vector of all the simulation objects
        that will be used in the simulation. The vector may later be passed to
        getPartitionInfo to partition the objects. The order of objects is not
        guaranteed to be preserved, so any infomation necessary for
        partitioning should be stored in the objects.

        @return A poiner to a vector containing all SimulationObjects that will
        be used in the simulation.
    */
    virtual std::vector<SimulationObject*>* getSimulationObjects() = 0;

    /** This is an optional method that partitions the SimulationObjects. If
        not implemented, a default partitioning scheme will be used.

       @return A pointer to partitioning information for at least one
       partition.
    */
    virtual const PartitionInfo* getPartitionInfo(unsigned int numProcessorsAvailable,
                                                  const std::vector<SimulationObject*>* simulationObjects);

    /** This method is invoked by the kernel so that the application can
        perform any cleanup necessary.

        @return Error code (non-zero return value indicates error)
    */
    virtual int finalize() = 0;

    virtual ~Application() {}

    /// This is an optional method
    virtual void configure(SimulationConfiguration&) {}

    /**
       The application should register all of it's deserializers when this
       callback is called.
    */
    virtual void registerDeserializers() = 0;

    /**
       Returns positive infinity in this application's time definition.
    */
    virtual const VTime& getPositiveInfinity() = 0;

    /**
       Returns zero in this application's time definition;
    */
    virtual const VTime& getZero() = 0;

    /**
       Returns a specified time in this application's time definition;
    */
    virtual const VTime& getTime(std::string&) = 0;

protected:
    /**
        Default constructor. It is protected to make sure this class never
        gets instantiated directly, but instead gets instantiated by
        derivation.
    */
    Application() {}

private:
};

#endif
