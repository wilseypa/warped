// See copyright notice in file Copyright in the root directory of this archive.

#include "DTSchedulingManager.h"
#include "assert.h"

#define NUM_THREADS 4

InputEventQueue* Input = new InputEventQueue();

DTSchedulingManager::DTSchedulingManager() {

	executeQueueLock = new AtomicState();
	executeQueue.resize(NUM_THREADS);
	event* emptyEvent = new event(1, 1, 999);
	for (int i = 0; i < NUM_THREADS; i++) {
		executeQueue[i] = emptyEvent;
	}

	//	Input = new InputEventQueue();
}

DTSchedulingManager::~DTSchedulingManager() {

	delete executeQueueLock;
	delete *executeQueueIterator;
}

void DTSchedulingManager::getExecuteLock(int threadId) {
	while (!executeQueueLock->setLock(threadId))
		;
	assert(executeQueueLock->hasLock(threadId));
}

void DTSchedulingManager::releaseExecuteLock(int threadId) {
	assert(executeQueueLock->hasLock(threadId));
	executeQueueLock->releaseLock(threadId);
}

void DTSchedulingManager::updateExecuteQueue(int threadId, event* ret) {
	this->getExecuteLock(threadId);
	//	executeQueue.push_back(ret);
	executeQueue[threadId - 1] = ret;
	if (ret != NULL)
		cout << "adding to " << threadId - 1 << " Object "
				<< executeQueue[threadId - 1]->receiveTime << endl;
	//this->printExecute(threadId);
	this->releaseExecuteLock(threadId);
}

void DTSchedulingManager::clearExecuteQueue(int threadId) {
	/*this->getExecuteLock(threadId);
	 executeQueue[threadId - 1] = NULL;
	 this->releaseExecuteLock(threadId);*/
	//	cout << "now cleared!" << endl;
}

const int DTSchedulingManager::currentGVT(int threadId) {
	int ltsevent = peekNextEvent(threadId);
	if (ltsevent != NULL)
		return ltsevent;
	else {
		cout << "Issue ---" << endl;
		return 100;
	}
}

// Function to print the receive times of the events in the executeQueue
void DTSchedulingManager::printExecute(int threadId) {
	cout << "Execute Queue: " << endl;
	//this->getExecuteLock(threadId);
	for (int i = 0; i < NUM_THREADS; i++) {
		if (executeQueue[i]->receiveTime != 999) {
			cout << executeQueue[i]->receiveTime << " ";
		} else
			cout << "-";
	}
	//this->releaseExecuteLock(threadId);

	cout << endl;
}

int DTSchedulingManager::peekNextEvent(int threadId) {
	int ret = 100000;
	//ret = Input->pickScheduleQueue(threadId);
	//	ret = *(executeQueue.begin());
	this->getExecuteLock(threadId);
	this->printExecute(threadId);
	//cout << "Size of Exe Q" << (executeQueue[1])->receiveTime << endl;
	executeQueueIterator = executeQueue.begin();
	for (int i = 0; i < NUM_THREADS; i++) {
		if (executeQueue[i]->receiveTime != 999) {
			if ((executeQueue[i]->getReceiveTime()) < (ret))
				ret = executeQueue[i]->receiveTime;
			//cout << (*executeQueueIterator)->receiveTime << ",,";
		}
	}
	this->releaseExecuteLock(threadId);
	return ret;
}

