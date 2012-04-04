// See copyright notice in file Copyright in the root directory of this archive.

#ifndef DTMATTERNGVTMANAGER_H_
#define DTMATTERNGVTMANAGER_H_

#include "MatternGVTManager.h"
class MatternGVTMessage;
class DTTimeWarpSimulationManager;

/** The DTMatternGVTManager class.

 Here the necessary functions are re-implemented for Lazy
 and Adaptive Cancellation.
 */
class DTMatternGVTManager: public MatternGVTManager {
public:
	/// Default Constructor
	DTMatternGVTManager(DTTimeWarpSimulationManager *simMgr,
			unsigned int period);
	/// Special Constructor for defining objectRecord elsewhere when using this manager as a base class
	DTMatternGVTManager(DTTimeWarpSimulationManager *simMgr,
			unsigned int period, bool objectRecordDefined);
	/// Destructor
	~DTMatternGVTManager();

	const VTime *getEarliestEventTime(const VTime *lowEventTime);
	void receiveKernelMessage(KernelMessage *msg);

	void sendPendingGVTToken();
private:
	const MatternGVTMessage* pendingGVTMessage;

	const VTime* GVTMessageLastScheduledEventTime;

	const VTime* GVTMessageMinimumTimeStamp;

	DTTimeWarpSimulationManager* mySimulationManager;
};
#endif /* DTMATTERNGVTMANAGER_H_ */
