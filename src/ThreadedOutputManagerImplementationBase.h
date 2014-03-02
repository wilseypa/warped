#ifndef THREADEDOUTPUTMANAGERIMPLEMENTATIONBASE_H_
#define THREADEDOUTPUTMANAGERIMPLEMENTATIONBASE_H_


#include <fstream>                      // for ofstream
#include <set>                          // for multiset
#include <vector>                       // for vector

#include "ObjectID.h"                   // for ObjectID
#include "ThreadedOutputEvents.h"
#include "ThreadedOutputManager.h"      // for ofstream, etc
#include "ThreadedTimeWarpMultiSetLTSF.h"  // for multiset
#include "ThreadedTimeWarpSimulationManager.h"
#include "warped.h"

class Event;
class SimulationObject;
class ThreadedOutputEvents;
class TimeWarpSimulationManager;
class VTime;

using std::ofstream;

class ThreadedTimeWarpSimulationManager;

using std::multiset;

/** The ThreadedOutputManagerImplementationBase class.

    This is the implementation base class from which all output
    manager implementations are derived from. This class contains all
    data common to the three support output managers: Aggressive,
    Lazy, and Adaptive.

*/
class ThreadedOutputManagerImplementationBase : public ThreadedOutputManager {
    friend class AggressiveOutputManagerTest;

public:

    /**@name Public Class Methods of ThreadedOutputManagerImplementationBase. */
    //@{

    /** Constructor.
        @param simMgr Handle to the simulation manager.
    */
    ThreadedOutputManagerImplementationBase(ThreadedTimeWarpSimulationManager* simMgr);

    /// Destructor
    ~ThreadedOutputManagerImplementationBase();

    /** Insert an event into the event set.

        @param event Ptr. to the event to be inserted.
        @param object Ptr. to the object who generated the output event.
    */
    virtual void insert(const Event* event, int threadID);

    /** Delete any unwanted (processed) elements.

        @param fossilCollectTime Time up to which to grbg-collect.
        @param object Simulation Object for which to grbg-collect
    */
    virtual void fossilCollect(SimulationObject* object,
                               const VTime& fossilCollectTime, int threadID);

    /** Delete any unwanted (processed) elements.

        @param fossilCollectTime Time up to which to grbg-collect.
        @param object Simulation Object for which to grbg-collect
    */
    virtual void fossilCollect(SimulationObject* object,
                               int fossilCollectTime, int threadID);

    /** Delete a single event from the output queue.

        @param toRemove The event to remove.
    */
    virtual void fossilCollectEvent(const Event* toRemove, int threadID);

    /** Gets the oldest event in the output set.

        @param unsigned int The needed size of the event.
        @return The oldest event, or NULL if none found that match the size.
    */

    ///this Method is not called anywhere. Need to optimize this if called somewhere
    virtual const Event* getOldestEvent(unsigned int size, int threadID);

    /** Save the output events at the specified checkpoint time.
        Only used for the optimistic fossil collection manager.

        @param outFile The checkpoint output file.
        @param objId The ID of the object being saved.
        @param saveTime The checkpoint time.
    */
    virtual void saveOutputCheckpoint(ofstream* outFile, const ObjectID& objId, unsigned int saveTime,
                                      int threadID);

    /**
      Remove all events from the output set. Used to restore state after
      a catastrophic rollback while using optimistic fossil collection.
    */
    virtual void ofcPurge(int threadID);

    //@} // End of Public Class Methods of ThreadedOutputManagerImplementationBase.

protected:
    /**@name Protected Class Methods of ThreadedOutputManagerImplementationBase. */
    //@{
    ThreadedOutputEvents& getOutputEventsFor(const ObjectID& objectId);

    TimeWarpSimulationManager* getSimulationManager() {
        return mySimulationManager;
    }

    //@} // End of Protected Class Methods of ThreadedOutputManagerImplementationBase.

private:
    /// Handle to the simulation manager.
    ThreadedTimeWarpSimulationManager* mySimulationManager;

    /// Pointer to the event set.
    vector< ThreadedOutputEvents*> myOutputEvents;


    ofstream* myStream;
};

#endif /* ThreadedOUTPUTMANAGERIMPLEMENTATIONBASE_H_ */
