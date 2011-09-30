//-*-c++-*-
#ifndef RAIDFork_H
#define RAIDFork_H

#include "RAIDRequest.h"
#include "SimulationObject.h"

using std::string;
using std::vector;

/** The class RAIDFork.

    Objects of this class will receive disk requests from 
    RAIDProcess (source) objects and distribute these requests
    to the RAID (Reduntant Array of Independent Disks).
*/
class RAIDFork: public SimulationObject {
public:
  
  /**@name Public Class Methods of RAIDFork */
  //@{

  /** Constructor.

      @param myName The name of this simulation object.
      @param numOutputs The number of outputs (disks).
      @param outNames The object names of the disks.
      @param disks The number of disks.
      @param startDisk The first disk to receive a request.
  */
  RAIDFork(string &myName, int numOutputs, vector<string> outNames,
	   int disks, int startDisk);

  /// Destructor.
  ~RAIDFork();

  /// Initialize the simulation object before simulation starts.
  void initialize();

  /// Finalize the simulation object before simulation terminates.
  void finalize();

  /// Execute the event that was scheduled.
  void executeProcess();

  /** Allocate a new state.

      @return A pointer to the newly allocated state.
  */
  State* allocateState();

  /** Deallocate a state.

      @param state A pointer to the state to deallocate.
  */
  void deallocateState(const State *state);

  /** Reclaim an event.

      @param event A pointer to the event to reclaim.
  */
  void reclaimEvent(const Event *event);

  /** Report an error.

      @param error An error message.
      @param SEVERITY The severity of the error.
  */
  void reportError(const string& error, SEVERITY);

  /** Accessor the objectName.

      @return The name of this simulation object.
  */
  const string &getName() const { return objectName; }
  //@} // End of Public Class Methods of RAIDFork.

private:
  /**@name Private Class Methods of RAIDFork */
  //@{

  /** Calculate stripe info.

      @param event This event will be modified here.
      @param pDisk The parity disk.
      @param rDisk The disk that will recieve the event.
  */
  void calculateStripeInfo(RAIDRequest *event, int &pDisk, int &rDisk);

  /** Calculates which disk will receive parity information.

      @param stripeUnit The stripe unit.
      @return The disk that will receive parity information.
  */
  int getParityDiskId(int stripeUnit);
  //@} // End of Private Class Methods of RAIDFork.

  /**@name Private Class Members of RAIDFork */
  //@{

  /// The name of this simulation object.
  string objectName;

  /// The number of outputs (disks).
  int numberOfOutputs;

  /// The names of the disks.
  vector<string> outputNames;

  /// The handles to the disks.
  vector<SimulationObject *> outputHandles;

  /// The number of disks.
  const int numDisks;

  /// The first disk that will receive a disk request.
  const int startDiskId;  
  //@} // End of Private Class Members of RAIDFork.
};
#endif
