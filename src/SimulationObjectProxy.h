#ifndef SIMULATION_OBJECT_PROXY_H
#define SIMULATION_OBJECT_PROXY_H


#include "warped.h"
#include <fstream>
#include "SimulationObject.h"

class Event;
class CommunicationManager;
class SimulationConfiguration;

/** The SimulationObjectProxy Class.

    Simulation objects are the core of a discrete event simulation.  A
    simulation object proxy represents a local representation of a
    simulation object that lives elsewhere (possibly on a different
    processor). The AbstractFactory of a simulation object proxy is the same
    as the simulation object AbstractFactory. However, as the derivation is
    protected, only the methods defined in this class can be invoked
    on a simulation object proxy instance.

*/
class SimulationObjectProxy : public SimulationObject {
public:

    /**@name Public Class Methods of SimulationObjectProxy */
    //@{

    /** Constructor.

    @param sourceSimMgr
    */
    SimulationObjectProxy(const string& objectName,
                          unsigned int sourceSimMgr,
                          unsigned int destSimMgr,
                          CommunicationManager* commMgr);

    /// Destructor.
    ~SimulationObjectProxy();

    /// reclaim the event
    void reclaimEvent(const Event*);

    void configure(SimulationConfiguration&) {}

    /** Returns the message type for an EventMessage */
    static const string& getEventMessageType();

    /** Returns the message type for a NegativeEventMessage */
    static const string& getNegativeEventMessageType();

    const string& getName() const {
        return objectName;
    }

    unsigned int getDestId() {
        return destinationSimulationManager;
    }


    //@} // End of Public Class Methods of SimulationObjectProxy

private:

    /**@name Private Class Methods of SimulationObjectProxy */
    //@{

    /// Empty (does not do anything).
    void initialize();

    /// Empty (does not do anything).
    void finalize();

    /// Empty (does not do anything).
    void executeProcess();

    /// Empty (does not do anything, except return NULL).
    State* allocateState();

    /// Empty (does not do anything).
    void deallocateState(const State* state);

    /// Empty (does not do anything).
    void reportError(const string&, SEVERITY);

    //@} // End of Private Class Attributes of SimulationObjectProxy

    /**@name Private Class Attributes of SimulationObjectProxy */
    //@{

    /// My source simulation manager id.
    unsigned int sourceSimulationManager;

    /// My destination simulation manager id.
    unsigned int destinationSimulationManager;

    /// My handle to the communication fabric.
    static CommunicationManager* myCommunicationManagerHandle;

    /**
       The name of the object I'm proxying for.
    */
    const string objectName;


    DEBUG(
        unsigned int numberOfPositiveNetworkEvents;
        unsigned int numberOfNegativeNetworkEvents;
    )


    //@} // End of Private Class Attributes of SimulationObjectProxy

};

#endif
