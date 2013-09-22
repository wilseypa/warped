#ifndef OUTPUT_MANAGER_H
#define OUTPUT_MANAGER_H


#include "warped.h"
#include "Configurable.h"
#include <fstream>
using std::ofstream;

class Event;
class SimulationObject;

/** The OutputManager class.

    This is the abstract base class from which all output manager
    implementations are derived from. The choices are:
    AggressiveOutputManager, LazyOutputManager, and
    AdaptiveOutputManager.

*/
class OutputManager : virtual public Configurable {
public:

  /**@name Public Class Methods of OutputManager. */
  //@{

  /// Destructor
  virtual ~OutputManager(){};
  
  /** Insert an event into the event set.
      
      This is a pure virtual function and has to be overridden by the
      user.

      @param event The output event to insert.
      @param object Ptr to the object who generated the event.
  */
  virtual void insert( const Event *event ) = 0;
  
  /** Rollback...
      
      This is a pure virtual function and has to be overridden by the
      user.

      @param rollbackTime Bound for how many antimessages will be sent.
      @param object A pointer the the object who experienced rollback.
  */
  virtual void rollback( SimulationObject *object, 
			 const VTime &rollbackTime ) = 0;
  
  /** Delete any unwanted (processed) elements.

      This is a pure virtual function and has to be overridden by the
      user.

      @param fossilCollectTime Time up to which to grbg-collect.
      @param object Simulation Object for which to grbg-collect 
  */
  virtual void fossilCollect(SimulationObject *object,
                             const VTime &fossilCollectTime) = 0;

  /** Delete any unwanted (processed) elements.

      This is a pure virtual function and has to be overridden by the
      user.

      @param fossilCollectTime Time up to which to grbg-collect.
      @param object Simulation Object for which to grbg-collect
  */
  virtual void fossilCollect(SimulationObject *object,
                             int fossilCollectTime) = 0;

  /** Delete a single event from the output queue.

      This is a pure virtual function and has to be overriden by the
      user.

      @param toRemove The event to remove.
  */
  virtual void fossilCollectEvent(const Event *toRemove) = 0;

  /** Gets the oldest event in the output set.

      @param unsigned int The needed size of the event.
      @return The oldest event, or NULL if none found that match the size.
  */
  virtual const Event *getOldestEvent(unsigned int size) = 0;

  /** Save the output events at the specified checkpoint time.
      Only used for the optimistic fossil collection manager.

      @param outFile The checkpoint output file.
      @param objId The ID of the object being saved.
      @param saveTime The checkpoint time.
  */
  virtual void saveOutputCheckpoint(ofstream* outFile, const ObjectID &objId, unsigned int saveTime) = 0;

  /**
     Remove all events from the output set. Used to restore state after
     a catastrophic rollback while using optimistic fossil collection.
  */
  virtual void ofcPurge() = 0;

  //@} // End of Public Class Methods of OutputManager.
  
protected:

  /**@name Protected Class Methods of OutputManager. */
  //@{
  
  /// Default Constructor
  OutputManager(){};
  
  //@} // End of Protected Class Methods of OutputManager.
};

#endif
