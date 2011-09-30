// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationManager.h"
#include "TimeWarpSimulationObjectLockedQueue.h"
#include "ThreadedTimeWarpEventSet.h"
#include "SimulationObject.h"
#include "TimeWarpEventSet.h"
#include "Event.h"
#include "IntVTime.h"
#include <iostream>

using std::cout;
using std::endl;
TimeWarpSimulationObjectLockedQueue::TimeWarpSimulationObjectLockedQueue(TimeWarpSimulationManager *initSimManager)
	: TimeWarpSimulationObjectQueue(initSimManager),
	  minimumQueueTime(initSimManager->getZero().clone())
{
	pthread_spin_init(&queueLock,NULL);
}


TimeWarpSimulationObjectLockedQueue::~TimeWarpSimulationObjectLockedQueue()
{
}

const ObjectEventPair *TimeWarpSimulationObjectLockedQueue::getNext()
{
	const ObjectEventPair *nextPair=NULL;
	pthread_spin_lock(&queueLock);
	if (!myObjectQueue.empty())
	{
		nextPair = myObjectQueue.top();
		//Actually remove the pair from the queue
		myObjectQueue.pop();
		//Update the minimum queue time
		delete minimumQueueTime;
		minimumQueueTime = nextPair->getReceiveTime().clone();
	}
	pthread_spin_unlock(&queueLock);
	return nextPair;
}

const VTime *TimeWarpSimulationObjectLockedQueue::peekNext()
{
	const ObjectEventPair *topPair = NULL;
	pthread_spin_lock(&queueLock);
	if (!myObjectQueue.empty())
	{
		topPair = myObjectQueue.top();
		delete minimumQueueTime;
		minimumQueueTime = topPair->getReceiveTime().clone();
	}
	pthread_spin_unlock(&queueLock);
	return minimumQueueTime;
}

bool TimeWarpSimulationObjectLockedQueue::insert(SimulationObject *object,const Event *event)
{
	bool success=false;
	pthread_spin_lock(&queueLock);
	ASSERT(object!=0);
	ASSERT(event!=0);
	//Artificailly Mimic the Calendar Queue Operation and dont
	//allow inserts into the past
	//if (event->getReceiveTime()>*minimumQueueTime)
	{
		ObjectEventPair *dataPair = new ObjectEventPair(object, event);
		myObjectQueue.push(dataPair);
		success=true;
	}
	pthread_spin_unlock(&queueLock);
	return success;
}

void TimeWarpSimulationObjectLockedQueue::display()
{
	pthread_spin_lock(&queueLock);
	ObjectEventPair *dataPair;
	priority_queue< ObjectEventPair *, vector<ObjectEventPair *>, receiveTimeEventPair > tempQueue;
	cout << "ObjectQueue Display: " << myObjectQueue.size() << ": ";
	while (!myObjectQueue.empty())
	{
		dataPair = myObjectQueue.top();
		myObjectQueue.pop();
		dataPair->display();
		tempQueue.push(dataPair);
	}
	cout << endl;
	while (!tempQueue.empty())
	{
		dataPair = tempQueue.top();
		tempQueue.pop();
		myObjectQueue.push(dataPair);
	}
	pthread_spin_unlock(&queueLock);
}


