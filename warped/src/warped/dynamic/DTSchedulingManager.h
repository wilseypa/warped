#ifndef DYNAMIC_TIME_WARP_SCHEDULING_MANAGER_H
#define DYNAMIC_TIME_WARP_SCHEDULING_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "InputEventQueue.h"

class DTSchedulingManager	{
public:
	DTSchedulingManager();
	~DTSchedulingManager();
	void updateExecuteQueue(int threadId, event* ret);
	void clearExecuteQueue(int threadId);
	void getExecuteLock(int threadId);
	void releaseExecuteLock(int threadId);
//	const event* LTSEvent(int threadId);
	int peekNextEvent(int threadId);
	const int currentGVT(int threadId);
	virtual void printExecute(int threadId);
private:
	AtomicState* executeQueueLock;
	vector <event*> executeQueue;
	vector <event*>::iterator executeQueueIterator;
//	InputEventQueue* Input;
};
#endif
