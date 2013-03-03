// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpMultiSetSchedulingManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "warped/Event.h"

class ThreadedTimeWarpEventSet;

ThreadedTimeWarpMultiSetSchedulingManager::ThreadedTimeWarpMultiSetSchedulingManager(
		ThreadedTimeWarpSimulationManager *initSimulationManager) :
	mySimulationManager(initSimulationManager), lastScheduledTime(
			initSimulationManager->getZero().clone()) {
	int num_threads = mySimulationManager->getNumberofThreads();
	executeQueue.resize(num_threads); // Allocate the no. of threads initialized
	executeQueueLock = new AtomicState();
	//	for(int i=0;i<NUM_THREADS;i++)
	//		executeQueue[i] = new Event(0);						// Initializing an event -- Need to check
}

ThreadedTimeWarpMultiSetSchedulingManager::~ThreadedTimeWarpMultiSetSchedulingManager() {
	// TODO Auto-generated destructor stub
	//	for(int i=0;i<NUM_THREADS;i++)							// Destroying the vector to hold the executeQueue
	//		delete executeQueue[i];
	delete executeQueueLock;
}

const VTime & ThreadedTimeWarpMultiSetSchedulingManager::getLastEventScheduledTime() {
	ThreadedTimeWarpEventSet *eventSet = mySimulationManager->getEventSetManagerNew();
	ASSERT( eventSet != 0 );
	const VTime *nextEventTime = (eventSet->nextEventToBeScheduledTime(0));
	if (nextEventTime == NULL)
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
				<< ")" << "NextEvent Time from Schedule Queue is NULL" << endl;
	/*	const VTime *minTime =
	 &(dynamic_cast<ThreadedTimeWarpMultiSet*> (eventSet)->getMinimumOfAll(1));*/
	const VTime *minTime = mySimulationManager->getLVT();
	if (nextEventTime != NULL && *minTime > *nextEventTime)
		minTime = nextEventTime;

	/*const VTime *executeTime =
	 (mySimulationManager->getPositiveInfinity().clone());
	 utils::debug << "(" << mySimulationManager->getSimulationManagerID()
	 << ")" << "Unprocessed Min Time : " << *minTime << endl;
	 this->getExecuteLock(0); // Its hard coded as 0 as only the master thread would be accessing it.
	 executeQueueIterator = executeQueue.begin();
	 while (executeQueueIterator != executeQueue.end()) {
	 if ((*executeQueueIterator) != NULL)
	 if (((*executeQueueIterator)-> getReceiveTime()) < (*executeTime))
	 executeTime = (*executeQueueIterator)->getReceiveTime().clone();
	 executeQueueIterator++;
	 }
	 this->releaseExecuteLock(0);

	 if (*minTime == mySimulationManager->getPositiveInfinity()) {
	 //ASSERT(false);
	 minTime = &(mySimulationManager->getGVTManager()->getGVT());
	 }

	 lastScheduledTime = (*minTime) > (*executeTime) ? (*executeTime).clone()
	 : (*minTime).clone();
	 utils::debug << "(" << mySimulationManager->getSimulationManagerID()
	 << ")" << "Execute & Schedule Min Time : " << *executeTime << endl;
	 */
	lastScheduledTime = minTime->clone();
	ASSERT( lastScheduledTime != 0 );

	return *lastScheduledTime;
}

const Event *
ThreadedTimeWarpMultiSetSchedulingManager::peekNextEvent(int threadId) {
	ThreadedTimeWarpEventSet *eventSet = mySimulationManager->getEventSetManagerNew();
	ASSERT( eventSet != 0 );

	const Event *earliestEvent = eventSet->peekEvent(NULL, threadId);

	return earliestEvent;
}

const Event *
ThreadedTimeWarpMultiSetSchedulingManager::peekNextEvent() {
	const Event *earliestEvent;
	return earliestEvent;
}

void ThreadedTimeWarpMultiSetSchedulingManager::getExecuteLock(int threadId) {
	while (!executeQueueLock->setLock(threadId))
		;
	ASSERT(executeQueueLock->hasLock(threadId));
}

void ThreadedTimeWarpMultiSetSchedulingManager::releaseExecuteLock(int threadId) {
	ASSERT(executeQueueLock->hasLock(threadId));
	executeQueueLock->releaseLock(threadId);
}

void ThreadedTimeWarpMultiSetSchedulingManager::updateExecuteQueue(int threadId,
		const Event* updateEvent) {
	this->getExecuteLock(threadId);
	executeQueue[threadId - 1] = updateEvent;
	this->releaseExecuteLock(threadId);
}

void ThreadedTimeWarpMultiSetSchedulingManager::clearExecuteQueue(int threadId) {
	this->getExecuteLock(threadId);
	executeQueue[threadId - 1] = NULL;
	this->releaseExecuteLock(threadId);
}

