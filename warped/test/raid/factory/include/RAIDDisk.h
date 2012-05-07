//-*-c++-*-
#ifndef RAIDDISK_H
#define RAIDDISK_H

#include "RAIDRequest.h"
#include "SimulationObject.h"
#include<map>

class IntVTime;
/** The class RAIDDisk.

    Objects of this class will receive disk requests from RAIDFork
    objects and process them After they have been processed they will
    be returned to the RAIDProcess (source) object that generated the
    request currently being processed.  
*/
class RAIDDisk : public SimulationObject {
public:
  /**@name Public Class Methods of RAIDisk */
  //@{

  /** Constructor.

      @param myName The name of this simulation object.
      @parm theDisk The type of this disk.
  */
  RAIDDisk(string &myName, DISK_TYPE theDisk);

  /// Desctructor.
  ~RAIDDisk();

  /// Initialize this simulation object before simulation starts.
  void initialize();

  /// Finalize simulation object before simulation terminates.
  void finalize();

  /// Execute the event that was scheduled.
  void executeProcess();

  /** Allocate a new state.

      @return The newly allocated state.
  */
  State* allocateState();

  /** Deallocate a state.

      @param state A pointer to the state to deallocate.
  */
  void deallocateState(const State *state);

  /** Reclaim an event.

      @param event A handle to the event to reclaim.
  */
  void reclaimEvent(const Event *event);

  /** Report an error.

      @param error An error message.
      @param SEVERITY The severity of the error.
  */
  void reportError(const string& error, SEVERITY);

  /** Get the name of this simulation object.

      @return The name of this simulation object.
  */
  const string &getName() const {return objectName;}
  //@} // End of Public Class Methods of RAIDDisk.

private:
  /**@name Private Class Members of RAIDisk */
  //@{

  /// The name of this simulation object.
  string objectName;

  /// Revolution time of this disk.
  double revolutionTime;

  /// Minimum seek time of this disk.
  double minSeekTime;

  /// Average seek time of this disk.
  double avgSeekTime;

  /// Maximum seek time of this disk.
  double maxSeekTime;

  /// Number of sectors per track on this disk.
  int    sectorsPerTrack;

  /// Number of tracks per cylinder on this disk.
  int    tracksPerCyl;

  /// Number of cylinders on this disk.
  int    numCylinders;
  //@} // End of Private Class Members of RAIDDisk.

  std::map<std::string,warped64_t> lastEventTime;
};

#endif
