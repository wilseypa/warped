#ifndef SIMULATION_MANAGER_IMPLEMENTATION_BASE_H
#define SIMULATION_MANAGER_IMPLEMENTATION_BASE_H

#include <fstream>                      // for ostream
#include <string>                       // for string, hash
#include <unordered_map>                // for _Hashtable<>::iterator, etc
#include <utility>                      // for pair
#include <vector>                       // for vector

#include "SimulationManager.h"          // for SimulationManager
#include "warped.h"
using std::string;

class Event;
class SimulationObject;
class VTime;

/** The base class that implements a SimulationManager.

    The SimulationManagerImplementationBase class is the base class
    from which all other simulation managers are derived from. It
    contains the methods and data that is common to all types of
    simulation managers such as the sequential simulation manager and
    the Time Warp simulation manager.

*/
class SimulationManagerImplementationBase : public SimulationManager {
public:
    SimulationManagerImplementationBase();
    virtual ~SimulationManagerImplementationBase();

    /** This method is called to start the simulation.

    The Simulation Manager calls this method when it is ready to start
    the simulation.

    @param simulateUntil The time till which to simulate
    */
    virtual void simulate(const VTime& simulateUntil);

    /** Receive an event.

    @param event Pointer to the received event.
    @param sender Pointer to the sender of the event.
    @param receiver Pointer to the receiver of the event.
    */
    virtual void receiveEvent(Event* event, SimulationObject* sender,
                              SimulationObject* receiver);

    /** This method is called to register a list (a hash_map of simulation
        object pointers) of simulation objects with the Simulation Manager.
        The size of the hash_map must be greater than or equal to one.

        @param list The vector of simulation object pointers to register
    */
    virtual void registerSimulationObjects();

    /** This method is called to unregister a list (vector of simulation
        object pointers) of simulation objects from the SimulationManager.
        The size of the vector must be greater than or equal to one.

        @param list The vector of simulation object pointers to unregister
    */
    void unregisterSimulationObjects(vector<SimulationObject*>* list);

    /// returns the number of simulation objects
    unsigned int getNumberOfSimulationObjects() const { return numberOfObjects; }

    /// sets the number of simulation objects
    void setNumberOfObjects(unsigned int numObjects);

    /// returns the number of simulation managers
    unsigned int getNumberOfSimulationManagers() const {
        return numberOfSimulationManagers;
    }

protected:
    /** This method is used to initialize the Simulation Manager before the
        simulation begins.

        This gives each SimulationManager a chance to perform any actions
        required for initialization (examples of what might occur during
        initialization might include opening files, or setting up a
        distributed simulation).
    */
    void initializeObjects();

    /** This method is called after the simulation has ended.

    This allows the Simulation Manager to ``clean up'', performing
    actions such as closing files, collecting statistics, and producing
    output.
    */
    void finalizeObjects();

    /** Display the object name - object pointer map.

    @param out Out-stream.
    */
    void displayObjectMap(std::ostream& out);

    /// Number of objects registered with this simulation manager.
    unsigned int numberOfObjects;

    /// Number of simulation managers.
    unsigned int numberOfSimulationManagers;

    typedef std::unordered_map<string, SimulationObject*> typeSimMap;

    vector<SimulationObject*>* getElementVector(typeSimMap* elementMap) {
        vector<SimulationObject*>* objects = new vector<SimulationObject*>;
        //Obtains all the objects from elementMap
        for (typeSimMap::iterator i = elementMap->begin(); i != elementMap->end(); i++) {
            objects->push_back(i->second);
        }
        return objects;
    }

    vector<string>* getKeyVector(typeSimMap* elementMap) {
        vector<string>* keys = new vector<string>;
        //Obtains all the objects from elementMap
        for (typeSimMap::iterator i = elementMap->begin(); i != elementMap->end(); i++) {
            keys->push_back(i->first);
        }
        return keys;
    }

    typeSimMap* localArrayOfSimObjPtrs;

    /// Mapping between simulation object ids to names
    vector<SimulationObject*> localArrayOfSimObjIDs;

    /// Turns a vector<SimulationObject *> into a map<string, SimulationObject *>.
    typeSimMap* partitionVectorToHashMap(vector<SimulationObject*>* vector);
};

#endif

