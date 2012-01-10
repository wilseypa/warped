// See copyright notice in file Copyright in the root directory of this archive.

#include "DTMatternGVTManager.h"
#include "DTTimeWarpSimulationManager.h"
#include "DTLazyOutputManager.h"

DTMatternGVTManager::DTMatternGVTManager(TimeWarpSimulationManager *simMgr,
		unsigned int period) :
	MatternGVTManager(simMgr, period) {
}
DTMatternGVTManager::DTMatternGVTManager(TimeWarpSimulationManager *simMgr,
		unsigned int period, bool objectRecordDefined) :
	MatternGVTManager(simMgr, period, objectRecordDefined) {
}
/// Destructor
DTMatternGVTManager::~DTMatternGVTManager() {
}

const VTime *DTMatternGVTManager::getEarliestEventTime(
		const VTime *lowEventTime) {
	if (mySimulationManager->getOutputMgrType() == LAZYMGR
			|| mySimulationManager->getOutputMgrType() == ADAPTIVEMGR) {
		DTLazyOutputManager
				*lmgr =
						dynamic_cast<DTLazyOutputManager*> (dynamic_cast<DTTimeWarpSimulationManager*> (mySimulationManager)->getOutputManagerNew());
		// GVT calculation is done by manager thread, hence passing 0 as threadId
		const VTime *lazyMinTime = &lmgr->getLazyQMinTime(0);
		lowEventTime = &MIN_FUNC(*lazyMinTime, *lowEventTime);
	}
	return lowEventTime;
}
