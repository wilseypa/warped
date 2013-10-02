
#ifndef THREADEDMATTERNGVTMANAGER_H_
#define THREADEDMATTERNGVTMANAGER_H_

#include "MatternGVTManager.h"
class MatternGVTMessage;
class ThreadedTimeWarpSimulationManager;

/** The ThreadedMatternGVTManager class.

 Here the necessary functions are re-implemented for Lazy
 and Adaptive Cancellation.
 */
class ThreadedMatternGVTManager: public MatternGVTManager {
public:
    /// Default Constructor
    ThreadedMatternGVTManager(ThreadedTimeWarpSimulationManager* simMgr,
                              unsigned int period);
    /// Special Constructor for defining objectRecord elsewhere when using this manager as a base class
    ThreadedMatternGVTManager(ThreadedTimeWarpSimulationManager* simMgr,
                              unsigned int period, bool objectRecordDefined);
    /// Destructor
    ~ThreadedMatternGVTManager();

//  const VTime *getEarliestEventTime(const VTime *lowEventTime);
    void receiveKernelMessage(KernelMessage* msg);

    void sendPendingGVTToken();
private:
    const MatternGVTMessage* pendingGVTMessage;

    const VTime* GVTMessageLastScheduledEventTime;

    const VTime* GVTMessageMinimumTimeStamp;

    ThreadedTimeWarpSimulationManager* mySimulationManager;
};
#endif /* ThreadedMATTERNGVTMANAGER_H_ */
