#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H


#include <iosfwd>                       // for ios
#include <string>                       // for string
#include <vector>                       // for vector

#include "Configurable.h"               // for Configurable
#include "SimulationObject.h"           // for SimulationObject (ptr only), etc
#include "SimulationStream.h"
#include "warped.h"                     // for SEVERITY

using std::string;

class Event;
class SimulationStream;
class VTime;

/** The abstract base class SimulationManager.

    The SimulationManager class is the abstract base class that
    defines the interface that a simulation manager provides to the
    simulation object.

*/
class SimulationManager : virtual public Configurable {
public:
    /**@name Public Class Methods of SimulationObject. */
    //@{

    /// Destructor.
    virtual ~SimulationManager();

    /** This method is used to initialize the SimulationManager before the
        simulation begins.

        This gives each SimulationManager a chance to perform any actions
        required for initialization (examples of what might occur during
        initialization might include opening files, or setting up a
        distributed simulation).

        This is a pure virtual function and has to be overridden by the
        user.
    */
    virtual void initialize() = 0;

    /** This method is called after the simulation has ended.

    This allows the SimulationManager to ``clean up'', performing
    actions such as closing files, collecting statistics, and producing
    output.

    This is a pure virtual function and has to be overridden by the user.
    */
    virtual void finalize() = 0;

    /** Handle an event on behalf of a SimulationObject.

    This is the method that SimulationObjects call to send events to other
    objects, local or remote.

    This is a pure virtual function and has to be overridden by the user.

    @param event Pointer to the received event.
    */
    virtual void handleEvent(const Event* event) = 0;

    /** Get and remove event for simulation object.

    This is a pure virtual function and has to be overridden by the user.

    @param object Simulation object whose event to get.
    @return Pointer to the event.
    */
    virtual const Event* getEvent(SimulationObject* object) = 0;

    /** Get a pointer to the next event for a simulation object.

    This is a pure virtual function and has to be overridden by the user.

    @param object Simulation object whose event to peek at.
    @return Pointer to the event.
    */
    virtual const Event* peekEvent(SimulationObject* object) = 0;

    /** This method is called to start the simulation.

    The SimulationManager calls this method when it is ready to start
    the simulation.

    This is a pure virtual function and has to be overridden by the user.

    @param simulateUntil The time till which to simulate
    */
    virtual void simulate(const VTime& simulateUntil) = 0;

    /**
       Returns true if the simulation has completed, false otherwise.
       Is useful in the case where we drive the simulation external through
       repeated calls to "simulation" with VTimes passed in.
    */
    virtual bool simulationComplete() = 0;

    /** This method is called to register a list (vector of simulation
        object pointers) of simulation objects with the SimulationManager.
        The size of the vector must be greater than or equal to one.

        This is a pure virtual function and has to be overridden by the user.

        @param list The vector of simulation object pointers to register
    */
    virtual void registerSimulationObjects() = 0;

    /** This method is called to unregister a list (vector of simulation
        object pointers) of simulation objects from the SimulationManager.
        The size of the vector must be greater than or equal to one.

        This is a pure virtual function and has to be overridden by the user.

        @param list The vector of simulation object pointers to unregister
    */
    virtual
    void unregisterSimulationObjects(vector<SimulationObject*>* list) = 0;

    /** Get object handle with string object as lookup.

    This is a pure virtual function and has to be overridden by the user.

    @param object String used to look up object.
    @return Handle to the object.
    */
    virtual SimulationObject* getObjectHandle(const string& object) const = 0;

    /** Get the current simulation time.

    This is a pure virtual function and has to be overriden by the user.

    @return The current simulation time.
    */
    virtual const VTime& getSimulationTime() const = 0;

    /// get a handle to a simulation input stream
    virtual SimulationStream* getIFStream(const string& filename,
                                          SimulationObject* object) = 0;

    /// get a handle to a simulation output stream
    virtual SimulationStream* getOFStream(const string& filename,
                                          SimulationObject* object,
                                          ios::openmode mode=ios::out) = 0;

    /// get a handle to a simulation input-output stream
    virtual SimulationStream* getIOFStream(const string& filename,
                                           SimulationObject* object) = 0;

    /// Returns the number of objects associated xwith this SimulationManager
    virtual unsigned int getNumberOfSimulationObjects() const = 0;

    /**
       This call can be used during the simulation to report than an error
       has occurred. If the {\tt Severity} is {\tt WARNING}, the message
       will simply be logged by the kernel with some kernel-dependent debug
       data.  If the {\tt Severity} is {\tt ABORT}, the simulation will
       abort and terminate immediately, with a call to {\tt finalize()}.

       @param msg The error message.
       @param level The level of severity of the error.
    */
    virtual void reportError(const string& msg, const SEVERITY level);

    /**
       This returns the simulation time that we are "safely" up to - we will
       not roll back before this time.
    */
    virtual const VTime& getCommittedTime() = 0;

    /**
       This returns the next event time of this simulation manager.  There
       are no guarantees about rollback, messages in transit, or anything
       else when calling this method.
    */
    virtual const VTime& getNextEventTime() = 0;

    /**
       Get a handle to the applications PInfinity singleton.
    */
    virtual const VTime& getPositiveInfinity() const = 0;

    /**
       Get a handle to the applications zero singleton.
    */
    virtual const VTime& getZero() const = 0;

    /**
       Shutdown, displaying "errorMessage" as the reason.
    */
    virtual void shutdown(const string& errorMessage) = 0;

    //@} // End of Public Class Methods of SimulationManager.

protected:
    /**@name Protected Class Members of SimulationManager. */
    //@{

    /// Default Constructor.
    SimulationManager();

    //@} // End of  Protected Class Members of SimulationManager.

};

#endif
