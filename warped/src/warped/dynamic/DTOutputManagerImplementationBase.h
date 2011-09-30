#ifndef DTOUTPUTMANAGERIMPLEMENTATIONBASE_H_
#define DTOUTPUTMANAGERIMPLEMENTATIONBASE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include <fstream>
#include <set>
#include "warped.h"
#include "warped/dynamic/DTOutputEvents.h"
#include "DTOutputManager.h"
using std::ofstream;

class DTTimeWarpSimulationManager;

using std::multiset;

/** The DTOutputManagerImplementationBase class.

    This is the implementation base class from which all output
    manager implementations are derived from. This class contains all
    data common to the three support output managers: Aggressive,
    Lazy, and Adaptive.

*/
class DTOutputManagerImplementationBase : public DTOutputManager {
  friend class AggressiveOutputManagerTest;

public:

   /**@name Public Class Methods of DTOutputManagerImplementationBase. */
   //@{

   /** Constructor.
       @param simMgr Handle to the simulation manager.
   */
   DTOutputManagerImplementationBase( DTTimeWarpSimulationManager *simMgr );

   /// Destructor
   ~DTOutputManagerImplementationBase();

   /** Insert an event into the event set.

       @param event Ptr. to the event to be inserted.
       @param object Ptr. to the object who generated the output event.
   */
  virtual void insert(const Event *event, int threadID );

   /** Delete any unwanted (processed) elements.

       @param fossilCollectTime Time up to which to grbg-collect.
       @param object Simulation Object for which to grbg-collect
   */
   virtual void fossilCollect(SimulationObject *object,
                               const VTime &fossilCollectTime, int threadID);

   /** Delete any unwanted (processed) elements.

       @param fossilCollectTime Time up to which to grbg-collect.
       @param object Simulation Object for which to grbg-collect
   */
   virtual void fossilCollect(SimulationObject *object,
                              int fossilCollectTime, int threadID);

   /** Delete a single event from the output queue.

       @param toRemove The event to remove.
   */
   virtual void fossilCollectEvent(const Event *toRemove, int threadID);

   /** Gets the oldest event in the output set.

       @param unsigned int The needed size of the event.
       @return The oldest event, or NULL if none found that match the size.
   */

   ///this Method is not called anywhere. Need to optimize this if called somewhere
   virtual const Event *getOldestEvent(unsigned int size, int threadID);

   /** Save the output events at the specified checkpoint time.
       Only used for the optimistic fossil collection manager.

       @param outFile The checkpoint output file.
       @param objId The ID of the object being saved.
       @param saveTime The checkpoint time.
   */
   virtual void saveOutputCheckpoint(ofstream* outFile, const ObjectID &objId, unsigned int saveTime, int threadID);

   /**
     Remove all events from the output set. Used to restore state after
     a catastrophic rollback while using optimistic fossil collection.
   */
   virtual void ofcPurge(int threadID);

   //@} // End of Public Class Methods of DTOutputManagerImplementationBase.

protected:
  /**@name Protected Class Methods of DTOutputManagerImplementationBase. */
  //@{
  DTOutputEvents &getOutputEventsFor( const ObjectID &objectId );

  TimeWarpSimulationManager *getSimulationManager(){
    return mySimulationManager;
  }

  //@} // End of Protected Class Methods of DTOutputManagerImplementationBase.

private:
  /// Handle to the simulation manager.
  DTTimeWarpSimulationManager *mySimulationManager;

  /// Pointer to the event set.
  vector< DTOutputEvents *> myOutputEvents;


  ofstream *myStream;
};

#endif /* DTOUTPUTMANAGERIMPLEMENTATIONBASE_H_ */
