#ifndef OUTPUT_MANAGER_IMPLEMENTATION_BASE_H
#define OUTPUT_MANAGER_IMPLEMENTATION_BASE_H


#include <fstream>                      // for ofstream
#include <set>                          // for multiset
#include <vector>                       // for vector

#include "ObjectID.h"                   // for ObjectID
#include "OutputEvents.h"               // for ofstream, etc
#include "OutputManager.h"              // for OutputManager
#include "warped.h"

class Event;
class SimulationObject;
class TimeWarpSimulationManager;
class VTime;

using std::multiset;
using std::ofstream;

/** The OutputManagerImplementationBase class.

    This is the implementation base class from which all output
    manager implementations are derived from. This class contains all
    data common to the three support output managers: Aggressive,
    Lazy, and Adaptive.

*/
class OutputManagerImplementationBase : public OutputManager {
    friend class AggressiveOutputManagerTest;

public:

    /**@name Public Class Methods of OutputManagerImplementationBase. */
    //@{

    /** Constructor.
        @param simMgr Handle to the simulation manager.
    */
    OutputManagerImplementationBase(TimeWarpSimulationManager* simMgr);

    /// Destructor
    ~OutputManagerImplementationBase();

    /** Insert an event into the event set.

        @param event Ptr. to the event to be inserted.
        @param object Ptr. to the object who generated the output event.
    */
    virtual void insert(const Event* event);

    /** Delete any unwanted (processed) elements.

        @param fossilCollectTime Time up to which to grbg-collect.
        @param object Simulation Object for which to grbg-collect
    */
    virtual void fossilCollect(SimulationObject* object,
                               const VTime& fossilCollectTime);

    /** Delete any unwanted (processed) elements.

        @param fossilCollectTime Time up to which to grbg-collect.
        @param object Simulation Object for which to grbg-collect
    */
    virtual void fossilCollect(SimulationObject* object,
                               int fossilCollectTime);

    /** Delete a single event from the output queue.

        @param toRemove The event to remove.
    */
    virtual void fossilCollectEvent(const Event* toRemove);

    /** Gets the oldest event in the output set.

        @param unsigned int The needed size of the event.
        @return The oldest event, or NULL if none found that match the size.
    */
    virtual const Event* getOldestEvent(unsigned int size);

    /** Save the output events at the specified checkpoint time.
        Only used for the optimistic fossil collection manager.

        @param outFile The checkpoint output file.
        @param objId The ID of the object being saved.
        @param saveTime The checkpoint time.
    */
    virtual void saveOutputCheckpoint(ofstream* outFile, const ObjectID& objId, unsigned int saveTime);

    /**
      Remove all events from the output set. Used to restore state after
      a catastrophic rollback while using optimistic fossil collection.
    */
    virtual void ofcPurge();

    //@} // End of Public Class Methods of OutputManagerImplementationBase.

protected:
    /**@name Protected Class Methods of OutputManagerImplementationBase. */
    //@{
    OutputEvents& getOutputEventsFor(const ObjectID& objectId);

    TimeWarpSimulationManager* getSimulationManager() {
        return mySimulationManager;
    }

    //@} // End of Protected Class Methods of OutputManagerImplementationBase.

private:
    /// Handle to the simulation manager.
    TimeWarpSimulationManager* mySimulationManager;

    /// Pointer to the event set.
    std::vector< OutputEvents*> myOutputEvents;

    /// Data structure used for the one-anti-message optimization.
    //unordered_ map<string, bool> alreadySentAntiMessages;

    ofstream* myStream;
};

#endif
