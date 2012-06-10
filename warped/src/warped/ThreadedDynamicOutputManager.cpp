// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedDynamicOutputManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "ThreadedOutputEvents.h"
#include "Event.h"

ThreadedDynamicOutputManager::ThreadedDynamicOutputManager(
		ThreadedTimeWarpSimulationManager *simMgr, unsigned int filterDepth,
		double aggr2lazy, double lazy2aggr, double thirdThreshold,
		bool useThirdThreshold) :
			ThreadedLazyOutputManager(simMgr),
			filterDepth(filterDepth),
			aggressive_to_lazy(aggr2lazy),
			lazy_to_aggressive(lazy2aggr),
			third_threshold(thirdThreshold),
			thirdThreshold(useThirdThreshold),
			comparisonResults(simMgr->getNumberOfSimulationObjects(),
					new vector<int> (filterDepth, 0)) {
	for (int i = 0; i < simMgr->getNumberOfSimulationObjects(); i++) {
		int* tempHit = new int();
		*tempHit = 0;
		float* tempRatio = new float();
		*tempRatio = 0;
		int* tempCurMeasured = new int();
		*tempCurMeasured = 0;
		bool* tempPermanentAggr = new bool();
		*tempPermanentAggr = false;
		cancellationModes* tempMode = new cancellationModes();
		*tempMode = Lazy;
		hitCount.push_back(tempHit);
		hitRatio.push_back(tempRatio);
		curMeasured.push_back(tempCurMeasured);
		permanentlyAggressive.push_back(tempPermanentAggr);
		*compareAndInsertMode[i] = true;
		curCancelMode.push_back(tempMode);
	}
}

ThreadedDynamicOutputManager::~ThreadedDynamicOutputManager() {

}

bool ThreadedDynamicOutputManager::checkDynamicCancel(const Event *event,
		int threadId) {
	//This method will just perform the LazyOutputManager::checkLazyCancelEvent.
	//Based on the bool result, a hit or miss will be recorded.
	//Messages are only suppressed when the current mode is Lazy and there is a hit.
	//The curMeasured will be kept track of using the filterdepth.
	//The event will not be inserted and events will not be cancelled.

	bool suppressMessage = false;
	bool lazyCancelHit = false;
	int numCancelledEvents = 0;
	SimulationObject *sender = getSimulationManager()->getObjectHandle(
			event->getSender());
	int objID = sender->getObjectID()->getSimulationObjectID();

	// Only do the check if there are any events to check.
	if ((lazyQueues[objID])->size() > 0) {
		if (!(*permanentlyAggressive[objID])) {
			setCompareMode(sender, *curCancelMode[objID] == Lazy);
			lazyCancelHit = ThreadedLazyOutputManager::checkLazyCancelEvent(
					event, threadId);
			numCancelledEvents = (eventsToCancel[objID])->size();

			if (*curCancelMode[objID] == Lazy) {
				suppressMessage = lazyCancelHit;
				if (lazyCancelHit && getCompareMode(sender)) {
					//Inserting the original event, reclaim this one.
					sender->reclaimEvent(event);
				} else {
					ThreadedOutputManagerImplementationBase::insert(event,
							threadId);
				}
				ThreadedLazyOutputManager::handleCancelEvents(sender, threadId);
			} else {
				eventsToCancel[objID]->clear();
				ThreadedOutputManagerImplementationBase::insert(event, threadId);
			}

			// Record a lazy hit.
			if (lazyCancelHit) {
				(*(comparisonResults[objID]))[(*curMeasured[objID])
						% filterDepth] = 1;
				*curMeasured[objID] = *curMeasured[objID] + 1;
			}

			// Record a lazy miss.
			// Misses may have to be recorded as the size of the cancellation queue eventsToCancel.
			for (int i = 0; i < numCancelledEvents; i++) {
				(*(comparisonResults[objID]))[(*curMeasured[objID])
						% filterDepth] = 0;
				*curMeasured[objID] = *curMeasured[objID] + 1;
			}

			if (*curMeasured[objID] >= filterDepth) {
				determinecancellationModes(objID, threadId);
				*curMeasured[objID] = 0;
			}
		} else {
			ThreadedOutputManagerImplementationBase::insert(event, threadId);
		}
	} else {
		ThreadedOutputManagerImplementationBase::insert(event, threadId);
	}

	return suppressMessage;
}

cancellationModes ThreadedDynamicOutputManager::determinecancellationModes(
		int objID, int threadId) {
	// Calculate the hit ratio. Based on the hitRatio, determine which
	// mode should be used. Return that mode.

	SimulationObject *object = getSimulationManager()->getObjectHandle(objID);
	bool lazyToAggr = false;
	bool aggrToLazy = false;

	*hitCount[objID] = 0;
	vector<int> *compRes = comparisonResults[objID];

	for (int i = 0; i < filterDepth; i++) {
		if ((*compRes)[i] == 1) {
			*hitCount[objID] = (*hitCount[objID]) + 1;
		}
	}

	*hitRatio[objID] = (float) (*hitCount[objID]) / filterDepth;
	utils::debug << "Object " << objID << " Hit Ratio is " << *hitRatio[objID]
			<< endl;

	// If the hit ratio is between the AGGRESSIVE_TO_LAZY and LAZY_TO_AGGRESSIVE
	// values, then do not change the mode.
	if (thirdThreshold && *hitRatio[objID] < third_threshold) {
		if (*curCancelMode[objID] == Lazy) {
			utils::debug << "Object " << objID
					<< " Switching from Lazy to Aggressive Output Manager PERMANENTLY.\n";
			lazyToAggr = true;
		}
		*curCancelMode[objID] = Aggressive;
		setCompareMode(object, false);
		*permanentlyAggressive[objID] = true;
	} else if ((*curCancelMode[objID]) == Lazy && (*hitRatio[objID])
			< lazy_to_aggressive) {
		utils::debug << "Object " << objID
				<< " Switching from Lazy to Aggressive Output Manager.\n";
		*curCancelMode[objID] = Aggressive;
		setCompareMode(object, false);
		lazyToAggr = true;
	} else if ((*curCancelMode[objID]) == Aggressive && (*hitRatio[objID])
			> aggressive_to_lazy) {
		utils::debug << "Object " << objID
				<< " Switching from Aggressive to Lazy Output Manager.\n";
		*curCancelMode[objID] = Lazy;
		setCompareMode(object, true);
		aggrToLazy = true;
	}

	// Any possible regenerated events remaining in the lazy cancel queue have
	// already been cancelled and need to be sent, so it is essentially remaining
	// in aggressive mode until the next rollback. End this current cancellation phase
	// by emptying the lazy cancel queue.
	if (aggrToLazy) {
		(lazyQueues[objID])->clear();
	}

	// Cancel the rest of the events in the lazy cancel queue. No proceeding
	// events will be suppressed.
	if (lazyToAggr) {
		(eventsToCancel[objID])->insert((eventsToCancel[objID])->end(),
				(lazyQueues[objID])->begin(), (lazyQueues[objID])->end());
		handleCancelEvents(object, threadId);
		(lazyQueues[objID])->clear();
	}

	*curMeasured[objID] = 0;

	return *curCancelMode[objID];
}

//void ThreadedDynamicOutputManager::emptyLazyQueues(const VTime &time) {
//	for (int t = 0; t < lazyQueues.size(); t++) {
//		if ((lazyQueues[t])->size() > 0) {
//			vector<const Event *>::iterator LCEvent = (lazyQueues[t])->begin();
//			while (LCEvent != (lazyQueues[t])->end()
//					&& (*LCEvent)->getSendTime() < time) {
//				// Only cancel when in lazy mode. The events have already been cancelled
//				// when in aggressive mode. Cancelling again can cause errors.
//				if (curCancelMode[t] == LAZY) {
//					(eventsToCancel[t])->push_back(*LCEvent);
//				}
//				LCEvent = (lazyQueues[t])->erase(LCEvent);
//				(*(comparisonResults[t]))[curMeasured[t] % filterDepth] = 0;
//				curMeasured[t] = curMeasured[t] + 1;
//			}
//		}
//	}
//
//	handleCancelEvents();
//
//	for (int t = 0; t < curMeasured.size(); t++) {
//		if (curMeasured[t] >= filterDepth) {
//			determinecancellationModes(t);
//			curMeasured[t] = 0;
//		}
//	}
//}

void ThreadedDynamicOutputManager::emptyLazyQueue(SimulationObject *object,
		const VTime &time, int threadId) {
	int id = object->getObjectID()->getSimulationObjectID();
	if ((lazyQueues[id])->size() > 0) {
		vector<const Event *>::iterator LCEvent = (lazyQueues[id])->begin();
		while (LCEvent != (lazyQueues[id])->end() && (*LCEvent)->getSendTime()
				< time) {
			if (*curCancelMode[id] == Lazy) {
				(eventsToCancel[id])->push_back(*LCEvent);
			}
			(*lazyMissCount[id])++;
			LCEvent = (lazyQueues[id])->erase(LCEvent);
			(*(comparisonResults[id]))[(*curMeasured[id]) % filterDepth] = 0;
			*curMeasured[id] = (*curMeasured[id]) + 1;
		}

		if (lazyQueues[id]->size() <= 0) {
			//End lazy cancellation phase.
			utils::debug << "Dynamic Cancellation Phase Complete For Object: "
					<< id << " Hits: " << *lazyHitCount[id] << " Misses: "
					<< *lazyMissCount[id] << "\n";
			*lazyHitCount[id] = 0;
			*lazyMissCount[id] = 0;
		}
	}

	handleCancelEvents(object, threadId);

	if (*curMeasured[id] >= filterDepth) {
		determinecancellationModes(id, threadId);
		*curMeasured[id] = 0;
	}
}

void ThreadedDynamicOutputManager::rollback(SimulationObject *object,
		const VTime &rollbackTime, int threadId) {
	int objID = object->getObjectID()->getSimulationObjectID();

	ThreadedOutputEvents &outputEvents = getOutputEventsFor(
			*(object->getObjectID()));
	vector<const Event*>
			*tempOutEvents = outputEvents.getEventsSentAtOrAfterAndRemove(
					rollbackTime, threadId);

	if (!(*permanentlyAggressive[objID])) {
		utils::debug << tempOutEvents->size() << " events added to object "
				<< objID << " Dynamic Lazy Queue" << endl;
		//These output events need to be added to the lazy cancel queue. There may already be
		//events in the queue, so the new ones need to be added.
		vector<const Event*> *lazyCancelEvents = lazyQueues[objID];
		lazyCancelEvents->insert(lazyCancelEvents->end(),
				tempOutEvents->begin(), tempOutEvents->end());
		std::sort(lazyCancelEvents->begin(), lazyCancelEvents->end(),
				sendTimeLessThan());

		//Send out the anti-messages if in aggressive mode.
		if (*curCancelMode[objID] == Aggressive) {
			if (tempOutEvents->size() > 0) {
				getSimulationManager()->cancelEvents(*tempOutEvents);
			}
		}
	} else {
		if (tempOutEvents->size() > 0) {
			getSimulationManager()->cancelEvents(*tempOutEvents);
		}
	}
	delete tempOutEvents;
}

void ThreadedDynamicOutputManager::ofcPurge(int threadId) {
	// Reset all of the values.
	for (int t = 0; t < getSimulationManager()->getNumberOfSimulationObjects(); t++) {
		// Take care of the ThreadedLazyOutputManager members.
		lazyQueues[t]->clear();
		eventsToCancel[t]->clear();
		*lazyHitCount[t] = 0;
		*lazyMissCount[t] = 0;

		// Take care of the ThreadedDynamicOutputManager members.
		*hitCount[t] = 0;
		*hitRatio[t] = 0;
		*curMeasured[t] = 0;
		for (int i = 0; i < comparisonResults[t]->size(); i++) {
			(*comparisonResults[t])[i] = 0;
		}
	}

	// Clear the output events.
	ThreadedOutputManagerImplementationBase::ofcPurge(threadId);
}

