// See copyright notice in file Copyright in the root directory of this archive.

#include "AtomicMatternGVTManager.h"
#include "TimeWarpSimulationManager.h"
#include "AtomicMatternObjectRecord.h"

AtomicMatternGVTManager::AtomicMatternGVTManager( TimeWarpSimulationManager *simMgr, unsigned int period )
	: MatternGVTManager(simMgr, period, false) {
	  objectRecord = new AtomicMatternObjectRecord();
}
/// Destructor
AtomicMatternGVTManager::~AtomicMatternGVTManager(){}

/// is it time to start a GVT estimation cycle
bool AtomicMatternGVTManager::checkGVTPeriod(){
	if (!gVTTokenPending) {
		if(__sync_add_and_fetch(&gVTPeriodCounter,1) == gVTPeriod){
			gVTPeriodCounter = 0;
			return true;
		}
	}
	return false;
}

///The output managers are already searched so this function is not needed
//for Atomic mattern
const VTime *AtomicMatternGVTManager::getEarliestEventTime(const VTime *lowEventTime) {
	return lowEventTime;
}

