// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedLazyOutputManager.h"
#include "ThreadedOutputEvents.h"
#include "ThreadedTimeWarpSimulationManager.h"

ThreadedLazyOutputManager::ThreadedLazyOutputManager(
		ThreadedTimeWarpSimulationManager *simMgr) :
	ThreadedOutputManagerImplementationBase(simMgr) {
	//	lazyMinQueueLock = new AtomicState();
	// lazy Queues min
	//	lazyMinQueue = new multiset<const Event*, sendTimeLessThanEventIdLessThan> ;
	for (int i = 0; i < simMgr->getNumberOfSimulationObjects(); i++) {
		lazyQueues.push_back(new vector<const Event*> ());
		eventsToCancel.push_back(new vector<const Event*> ());
		int* tempHit = new int();
		*tempHit = 0;
		int* tempMiss = new int();
		*tempMiss = 0;
		bool* tempCompare = new bool();
		*tempCompare = true;
		compareAndInsertMode.push_back(tempCompare);
		lazyHitCount.push_back(tempHit);
		lazyMissCount.push_back(tempMiss);
		//		lowestObjectPosition.push_back(lazyMinQueue->end());
	}
}

ThreadedLazyOutputManager::~ThreadedLazyOutputManager() {
	//		delete lazyMinQueueLock;

}

/*void ThreadedLazyOutputManager::getLazyMinQueueLock(int threadId) {
 while (!lazyMinQueueLock->setLock(threadId))
 ;
 ASSERT(lazyMinQueueLock->hasLock(threadId));
 }
 void ThreadedLazyOutputManager::releaseLazyMinQueueLock(int threadId) {
 ASSERT(lazyMinQueueLock->hasLock(threadId));
 lazyMinQueueLock->releaseLock(threadId);
 }*/

/*void ThreadedLazyOutputManager::lazyMinQueueUpdate(unsigned int objectID, int threadId) {
 // Update gvt Queue with the new minimum time stamp of the object
 this->getLazyMinQueueLock(threadId);
 // Do not erase the first time.
 if (lowestObjectPosition[objectID] != lazyMinQueue->end()) {
 lazyMinQueue->erase(lowestObjectPosition[objectID]);
 }
 if (lazyQueues[objectID]->size() > 0)
 lowestObjectPosition[objectID] = lazyMinQueue->insert(
 *(lazyQueues[objectID]->begin()));
 else
 lowestObjectPosition[objectID] = lazyMinQueue->end();
 multiset<const Event*, sendTimeLessThanEventIdLessThan>::iterator iv1;
 utils::debug << "LazyMin Queue";
 for (iv1 = lazyMinQueue->begin(); iv1 != lazyMinQueue->end(); iv1++) {
 utils::debug << "-" << (*iv1)->getSendTime();
 }
 this->releaseLazyMinQueueLock(threadId);
 }*/

bool ThreadedLazyOutputManager::checkLazyCancelEvent(const Event *event,
		int threadId) {

	bool suppressEvent = false;
	int
			objectID = getSimulationManager()->getObjectHandle(
					event->getSender())->getObjectID()->getSimulationObjectID();
	vector<const Event*> *lazyCancelEvents = lazyQueues[objectID];
	vector<const Event*> *eveToCan = eventsToCancel[objectID];

	//Perform lazy cancellation when there are still events to be compared to in the
	//lazy cancellation queue.
	if (lazyCancelEvents->size() != 0) {
		vector<const Event *>::iterator LCEvent = lazyCancelEvents->begin();

		//Any events with a timestamp less than the current event time
		//were not regenerated. Add them to the cancel events list.
		//	bool iDeleteEvent = false;
		while (LCEvent != lazyCancelEvents->end() && (*LCEvent)->getSendTime()
				< event->getSendTime()) {
			eveToCan->push_back(*LCEvent);
			lazyCancelEvents->erase(LCEvent);
			(*lazyMissCount[objectID])++;
			//	iDeleteEvent = true;
		}
		/*	if (iDeleteEvent)
		 lazyMinQueueUpdate(objectID, threadId);*/

		//Compare the events in the lazy cancellation queue to this event.
		//If the queue is empty after checking for past time stamps, end lazy cancellation.
		if (lazyCancelEvents->size() > 0) {
			LCEvent = lazyCancelEvents->begin();

			while (suppressEvent == false && LCEvent != lazyCancelEvents->end()) {

				if (const_cast<Event *> ((*LCEvent))->eventCompare(event)) {
					if (*compareAndInsertMode[objectID]) {
						ThreadedOutputManagerImplementationBase::insert(
								*LCEvent, threadId);
					}
					suppressEvent = true;
					(*lazyHitCount[objectID])++;
					LCEvent = lazyCancelEvents->erase(LCEvent);
					//		lazyMinQueueUpdate(objectID, threadId);
				} else {
					LCEvent++;
				}
			}

			if (lazyCancelEvents->size() <= 0) {
				//End lazy cancellation phase.
				utils::debug << "Lazy Cancellation Phase Complete For Object: "
						<< event->getSender() << " Lazy Hits: "
						<< *lazyHitCount[objectID] << " Lazy Misses: "
						<< *lazyMissCount[objectID] << "\n";
				*lazyHitCount[objectID] = 0;
				*lazyMissCount[objectID] = 0;
			}
		} else {
			//End lazy cancellation phase.
			utils::debug << "Lazy Cancellation Phase Complete For Object: "
					<< event->getSender() << " Lazy Hits: "
					<< *lazyHitCount[objectID] << " Lazy Misses: "
					<< *lazyMissCount[objectID] << "\n";
			*lazyHitCount[objectID] = 0;
			*lazyMissCount[objectID] = 0;
		}
	} else {
		// Not in lazy cancellation.
		*lazyHitCount[objectID] = 0;
		*lazyMissCount[objectID] = 0;
	}

	return suppressEvent;
}

void ThreadedLazyOutputManager::handleCancelEvents(int threadId) {
	for (int t = 0; t < eventsToCancel.size(); t++) {
		if ((eventsToCancel[t])->size() > 0) {
			getSimulationManager()->cancelEvents(*(eventsToCancel[t]));
			(eventsToCancel[t])->clear();
		}
	}
}

void ThreadedLazyOutputManager::handleCancelEvents(SimulationObject *object,
		int threadId) {
	int objectID = object->getObjectID()->getSimulationObjectID();
	if ((eventsToCancel[objectID])->size() > 0) {
		getSimulationManager()->cancelEvents(*(eventsToCancel[objectID]));
		(eventsToCancel[objectID])->clear();
	}
}

/*void ThreadedLazyOutputManager::emptyLazyQueues(const VTime &time, int threadId) {
 for (int t = 0; t < lazyQueues.size(); t++) {
 if ((lazyQueues[t])->size() > 0) {
 vector<const Event *>::iterator LCEvent = (lazyQueues[t])->begin();
 while (LCEvent != (lazyQueues[t])->end()
 && (*LCEvent)->getSendTime() < time) {
 (eventsToCancel[t])->push_back(*LCEvent);
 LCEvent = (lazyQueues[t])->erase(LCEvent);
 (*lazyMissCount[t])++;
 }
 }
 }
 handleCancelEvents(threadId);
 }*/

void ThreadedLazyOutputManager::emptyLazyQueue(SimulationObject *object,
		const VTime &time, int threadId) {
	int objectID = object->getObjectID()->getSimulationObjectID();
	//bool iDeleteEvent = false;
	if ((lazyQueues[objectID])->size() > 0) {
		vector<const Event *>::iterator LCEvent =
				(lazyQueues[objectID])->begin();
		while (LCEvent != (lazyQueues[objectID])->end()
				&& (*LCEvent)->getSendTime() < time) {
			(eventsToCancel[objectID])->push_back(*LCEvent);
			LCEvent = (lazyQueues[objectID])->erase(LCEvent);
			//	iDeleteEvent = true;
			(*lazyMissCount[objectID])++;
		}
		if (lazyQueues[objectID]->size() <= 0) {
			//End lazy cancellation phase.
			utils::debug << "Lazy Cancellation Phase Complete For Object: "
					<< objectID << " Lazy Hits: " << *lazyHitCount[objectID]
					<< " Lazy Misses: " << *lazyMissCount[objectID] << "\n";
			*lazyHitCount[objectID] = 0;
			*lazyMissCount[objectID] = 0;
		}
	}
	handleCancelEvents(object, threadId);
	//if (iDeleteEvent)
	//lazyMinQueueUpdate(objectID, threadId);
}

/*const VTime &ThreadedLazyOutputManager::getLazyQMinTime(int threadId) {
 const VTime *minTime = &getSimulationManager()->getPositiveInfinity();
 this->getLazyMinQueueLock(threadId);
 if (lazyMinQueue->size() > 0) {
 minTime = &(*(lazyMinQueue->begin()))->getSendTime();
 }
 this->releaseLazyMinQueueLock(threadId);
 return (*minTime);
 }*/

/*const VTime &ThreadedLazyOutputManager::getLazyQMinTime(const unsigned int objectID,
 int threadId) {
 const VTime *minTime = &getSimulationManager()->getPositiveInfinity();
 this->getLazyMinQueueLock(threadId);
 if (lowestObjectPosition[objectID] != lazyMinQueue->end()) {
 minTime = &((*(lowestObjectPosition[objectID]))->getReceiveTime());
 }
 this->releaseLazyMinQueueLock(threadId);
 return (*minTime);
 }*/

bool ThreadedLazyOutputManager::lazyCancel(const Event *event, int threadId) {
	SimulationObject *sender = getSimulationManager()->getObjectHandle(
			event->getSender());
	int objectID = sender->getObjectID()->getSimulationObjectID();
	bool retval = checkLazyCancelEvent(event, threadId);
	if (retval && *compareAndInsertMode[objectID]) {
		//Inserting the original event, reclaim this one.
		sender->reclaimEvent(event);
	} else {
		ThreadedOutputManagerImplementationBase::insert(event, threadId);
	}

	handleCancelEvents(sender, threadId);
	return retval;
}

void ThreadedLazyOutputManager::setCompareMode(SimulationObject *obj, bool mode) {
	int objectID = obj->getObjectID()->getSimulationObjectID();
	*compareAndInsertMode[objectID] = mode;
}

bool ThreadedLazyOutputManager::getCompareMode(SimulationObject *obj) {
	int objectID = obj->getObjectID()->getSimulationObjectID();
	return *compareAndInsertMode[objectID];
}

void ThreadedLazyOutputManager::rollback(SimulationObject *object,
		const VTime &rollbackTime, int threadId) {
	unsigned int objectID = object->getObjectID()->getSimulationObjectID();
	//Put the events that have a timestamp greater than rollback event's
	//timestamp into the Lazy Cancellation Queue to be compared to regenerated output.
	ThreadedOutputEvents &outputEvents = getOutputEventsFor(
			*(object->getObjectID()));
	vector<const Event*>
			*tempOutEvents = outputEvents.getEventsSentAtOrAfterAndRemove(
					rollbackTime, threadId);
	utils::debug << tempOutEvents->size() << " events added to object "
			<< objectID << " Lazy Queue" << endl;
	vector<const Event*> *lazyCancelEvents = lazyQueues[objectID];

	//These output events need to be added to the lazy cancel queue. There may already be
	//events in the queue, so the new ones need to be added.
	lazyCancelEvents->insert(lazyCancelEvents->end(), tempOutEvents->begin(),
			tempOutEvents->end());
	std::sort(lazyCancelEvents->begin(), lazyCancelEvents->end(),
			sendTimeLessThan());

	/*	vector<const Event*>::iterator it;
	 for (it = lazyQueues[objectID]->begin(); it != lazyQueues[objectID]->end(); it++) {
	 utils::debug << "-" << (*it)->getSendTime();
	 }*/
	// Update gvt Queue with the new minimum time stamp of the object
	//lazyMinQueueUpdate(objectID, threadId);

	//  outputEvents.remove(*tempOutEvents);
	delete tempOutEvents;
}

void ThreadedLazyOutputManager::ofcPurge(int threadId) {
	// Reset all queues and values.
	for (int i = 0; i < getSimulationManager()->getNumberOfSimulationObjects(); i++) {
		lazyQueues[i]->clear();
		eventsToCancel[i]->clear();
		*compareAndInsertMode[i] = true;
		*lazyHitCount[i] = 0;
		*lazyMissCount[i] = 0;
	}

	ThreadedOutputManagerImplementationBase::ofcPurge(threadId);
}

void ThreadedLazyOutputManager::printAll() {
	cout << "Lazy Queue" << endl;
	vector<const Event*>::iterator it;
	for (int i = 0; i < lazyQueues.size(); i++) {
		cout << "Queue " << i << " ::: ";
		for (it = lazyQueues[i]->begin(); it != lazyQueues[i]->end(); it++) {
			cout << "-" << (*it)->getSendTime();
		}
		cout << " " << endl;
	}
	/*cout << "GVT Queue" << endl;
	 multiset<const Event*, sendTimeLessThanEventIdLessThan>::iterator iv1;
	 //	if (lazyMinQueue->size() > 0){
	 for (iv1 = lazyMinQueue->begin(); iv1 != lazyMinQueue->end(); iv1++) {
	 cout << "-" << (*iv1)->getSendTime();
	 }
	 cout << " " << endl;*/
	cout << "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: "
			<< endl;
}
