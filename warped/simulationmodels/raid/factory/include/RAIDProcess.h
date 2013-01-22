//-*-c++-*-
#ifndef RAIDPROCESS_H
#define RAIDPROCESS_H

#include "../../../rnd/MLCG.h"
#include "../../../rnd/Normal.h"
#include "../../../rnd/Poisson.h"
#include "../../../rnd/Binomial.h"
#include "../../../rnd/Uniform.h"
#include "../../../rnd/NegExp.h"
#include "../../../rnd/DiscUnif.h"

#include "RAIDRequest.h"
#include "SimulationObject.h"

enum distribution_t {UNIFORM, POISSON, EXPONENTIAL, NORMAL, BINOMIAL, FIXED,
                     ALTERNATE, ROUNDROBIN, CONDITIONAL, ALL};

/** The class RAIDProcess.

    Objects of this class will generate disk requests (read and write
    requests). These requests will be passed on to RAIDDisks through a
    RAIDFork object. With the current implementation, each RAIDProcess
    will have one RAIDFork that distributes requests to N disks.
*/
class RAIDProcess : public SimulationObject {
public:
  /**@name Public Class Methods of RAIDProcess */
  //@{

  /// Default Constructor.
  RAIDProcess(){};
  
  /** Constructor.

      Note that firstdisk is statically set to 0 in the constructor as
      of now, as it doesn't seem to be of any consequence which disk
      is requested first.

      @param myName The name of this object.
      @param outName The name of the fork object.
      @param maxdisks The number of disks in the simulation.
      @param disk The type of the disk.
      @param maxrequests How many requests to generate.
      @param firstdisk The first disk to send request to.  
  */
  RAIDProcess(string &myName, string &outName, int maxdisks, 
	      DISK_TYPE disk, int maxrequests, int firstdisk, double initSeed);

  /// Destructor.
  ~RAIDProcess();

  /// Initialize the simulation object.
  void initialize();

  /// Finalize the simulation object before termination.
  void finalize();

  /// Execute the scheduled event.
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

      @param string An error message.
      @param SEVERITY The severity of the error.
  */
  void reportError(const string&, SEVERITY);

  /** Accessor for objectName.

      @return The name of this simulation object.
  */
  const string &getName() const {return objectName;}

  /** Accessor for maxDisks.

      @return The number of disks in the simulation.
  */
  int getMaxDisks() const {return maxDisks;}

  /** Accessor for maxCylinder.

      @return The maximum numberr of cylinders.
  */
  int getMaxCylinder() const {return maxCylinder;}

  /** Accessor for maxTrack.

      @return The maximum number of tracks.
  */
  int getMaxTrack() const {return maxTrack;}

  /** Accessor for maxSector.

      @return The maximum number of sectors.
  */
  int getMaxSector() const {return maxSector;}

  /** Accessor for totalStripeSize.

      @return Total stripe size.
  */
  int getTotalStripeSize() const {return totalStripeSize;}

  /** Accessor for maxStripeSize.

      @return Maximum stripe size.
  */
  int getMaxStripeSize() const {return maxStripeSize;}

  /** Accessor for maxRequests.

      @return The number of requests to generate.
  */
  int getMaxRequests() const {return maxRequests;}

  /** Accessor for firstDiskId.

      @return The first disk to generate request for.
  */
  int getFirstDiskId() const {return firstDiskId;}

  /** Accessor for diskType.

      @return The type of disk.
  */
  DISK_TYPE getDiskType() const {return diskType;}
  

  /** Set the number of disks in the simulation.

      @param disks The number of disks.
  */
  void setMaxDisks(int disks) {maxDisks = disks;}

  /** Set the maximum number of cylinders.

      @param cyl The maximum number of cylinders.
  */
  void setMaxCylinder(int cyl) {maxCylinder = cyl;}

  /** Set the maximum number of tracks.

      @param track The number of tracks.
  */
  void setMaxTrack(int track) {maxTrack = track;}

  /** Set the number of sectors.

     @param sect The number of sectors.
  */
  void setMaxSector(int sect) {maxSector = sect;}

  /** Set the total stripe size.

      @param size The total stripe size.
  */
  void setTotalStripeSize(int size) {totalStripeSize = size;}

  /** Set the maximum stripe size.

      @param size The maximum stripe size.
  */
  void setMaxStripeSize(int size) {maxStripeSize = size;}

  /** Set the number of requests to generate.

      @param req The number of requests to generate.
  */
  void setMaxRequests(int req) {maxRequests = req;}

  /** Set the first disk to receive a request.

      NOTE: By default, in the current implemenation, 
      the first disk id is always 0.

      @param id The first disk to receive a request.
  */
  void setFirstDiskId(int id) {firstDiskId = id;}

  /** Set the type of disk.

      @param type The type of disk.
  */
  void setDiskType(DISK_TYPE type) {diskType = type;}

  //@} // End of Public Class Methods of RAIDProcess.
private:
  /**@name Private Class Methods of RAIDProcess */
  //@{
  
  void newConfiguration();

  int msgDelay();
  void newMsg();
  //@} // End of Private Class Methods of RAIDProcess.

  /**@name Private Class Members of RAIDProcess */
  //@{

  /// Name of this simulation object.
  string objectName;

  /// Name of the RAIDFork that will distribute our requests.
  string outputName;

  /// A handle to the RAIDFork that will distribute our requests.
  SimulationObject *outputHandle;

  /// The number of disks in the simulation.
  int maxDisks;

  /// The maximum number of cylinders in the simulation.
  int maxCylinder;

  /// The maximum number of tracks in the simulation.
  int maxTrack;

  /// The maximum number of sectors in the simulation.
  int maxSector;

  /// The total stripe size.
  int totalStripeSize;

  /// The maximum stripe size.
  int maxStripeSize;

  /// The number of requests to generate.
  int maxRequests;

  /// First disk to receive a request (0 by default).
  int firstDiskId;

  /// The type of the disk.
  DISK_TYPE diskType;
  
  /// The source distribution.
  distribution_t sourcedist;

  /// First distribition seed.
  double first;

  /// Second distribution seed.
  double second;

  /// The initial seed.
  double seed;
  //@} // End of Private Class Members of RAIDProcess.
};

#endif
