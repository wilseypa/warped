#ifndef ATOMICMATTERNGVTMANAGER_H_
#define ATOMICMATTERNGVTMANAGER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "MatternGVTManager.h"
#include "GVTManagerImplementationBase.h"
class TimeWarpSimulationManager;
class MatternObjectRecord;
class SchedulingManager;
class CommunicationManager;

/** The AtomicMatternGVTManager class.

	   Here the necessary functions are re-implemented with atomic
	   functions so they are thread safe.
*/
class AtomicMatternGVTManager : public MatternGVTManager {
public:
	  /// Default Constructor
	  AtomicMatternGVTManager( TimeWarpSimulationManager *simMgr, unsigned int period );

	  /// Destructor
	  ~AtomicMatternGVTManager();

	  /// is it time to start a GVT estimation cycle
	  virtual bool checkGVTPeriod();

	  ///The output managers are already searched for Threaded Simulations
	  const VTime *getEarliestEventTime(const VTime *lowEventTime);
};
#endif /* ATOMICMATTERNGVTMANAGER_H_ */
