#ifndef ADAPTIVE_OUTPUT_MANAGER_H
#define ADAPTIVE_OUTPUT_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <fstream>
#include "warped.h"
#include "LazyOutputManager.h"

#define FILTER_DEPTH 16
#define AGGRESSIVE_TO_LAZY 0.5
#define LAZY_TO_AGGRESSIVE 0.2
#define THIRD_THRESHOLD 0.1

enum cancellationMode {AGGRESSIVE, LAZY};

class Event;
class SimulationObject;
class OutputManager;

/** The DynamicOutputManager class.

    This class implements an dynamic cancellation scheme as a part of
    its output manager functionality.

*/
class DynamicOutputManager : public LazyOutputManager {
public:   

  /**@name Public Class Methods of DynamicOutputManager. */
  //@{

  /** Constructor.

      @param simMgr Handle to the simulation manager.
      @param useThirdThreshold Default is false, not used.
  */
  DynamicOutputManager( TimeWarpSimulationManager *simMgr, bool useThirdThreshold = false );

  // Destructor.
  ~DynamicOutputManager();

  /** Returns true if the event should be suppressed.
      Returns false otherwise.

      When in aggressive cancellation, lazy checks will be performed but
      events will not be cancelled (they have already been cancelled through
      aggressive).

      When in lazy cancellation, the method will perform the same as the 
      lazyCancel method.

      @param event The event to be checked.
  */
  bool checkDynamicCancel( const Event *event );

  /** Returns the current cancellation mode.
  */
  cancellationMode getCancelMode(int objID){
     return curCancelMode[objID];
  }

  /** Sets the current cancellation mode.

      @param mode The cancellation mode to be set.
  */
  void setCancelMode(cancellationMode mode, int objID){
     curCancelMode[objID] = mode;
  }

  /** Rolls back the output queue to rollbackTime.

      Any events with send time greater than rollbackTime are added to the
      lazy cancellation queue.

      When in aggressive mode, anti-messages are also sent for those events.

      @param rollbackTime The time to which the object is rolled back.
      @param object A pointer to the object who experienced rollback.
  */
  void rollback( SimulationObject *object,  const VTime &rollbackTime );

  void emptyLazyQueues(const VTime &time);

  void emptyLazyQueue(SimulationObject *object, const VTime &time);

  /**
     Remove all output events. Used to restore state after
     a catastrophic rollback while using optimistic fossil collection.
  */
  void ofcPurge();

  //@} // End of Public Class Methods of DynamicOutputManager.

protected:
  /**@name protected Class Methods of DynamicOutputManager. */
  //@{

  /** Calculates the hit ratio and determines what mode to use.
      Also returns the cancellation mode that the calculation determined.

      @param objID The object ID of the object to set.
  */
  cancellationMode determineCancellationMode(int objID);

  /** The current cancellation mode.
  */ 
  vector<cancellationMode> curCancelMode;

  /** The hit count, how many lazy hits have occured.
  */
  vector<int> hitCount;

  /** The hit ratio is used to determine the cancellation mode.
  */
  vector<float> hitRatio;

  /** The filter depth determines how many comparisons are used.
  */
  int filterDepth;

  /** Keeps track of the results past comparisons.
  */
  vector< vector<int> *> comparisonResults;

  /** Keeps track of index for comparisonResults.
  */
  vector<int> curMeasured;

  /** True if permanently in aggressive mode.
  */
  vector<bool> permanentlyAggressive;

  /** True if using 3 threshold values instead of 2.
  */
  bool thirdThreshold;

  //@} // End of Protected Class Methods of DynamicOutputManager.
};

#endif
