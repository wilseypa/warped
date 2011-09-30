#ifndef SIMULATIONOBJECTCALENDARQUEUE_H_
#define SIMULATIONOBJECTCALENDARQUEUE_H_

// See copyright notice in file Copyright in the root directory of this archive.

//******************************THIS CODE CAUSES PROBLEMS*********************************
// This code does not allow inserts into the past which severely disrupts the forward simulation
//progress. As it currently stands this is not a viable implementation for the object queue
//************************************************************************************************

#include "TimeWarpSimulationObjectQueue.h"
class TimeWarpSimulationManager;
class SimulationObject;
class AtomicCalendarQueueBucket;

class TimeWarpSimulationObjectCalendarQueue : public TimeWarpSimulationObjectQueue
{
public:
	TimeWarpSimulationObjectCalendarQueue(TimeWarpSimulationManager *mySimulationManager,
										  unsigned int numberOfBuckets,
										  unsigned int bucketWidth);
	~TimeWarpSimulationObjectCalendarQueue();

	const ObjectEventPair *getNext();
	const VTime *peekNext();
	bool insert(SimulationObject *object, const Event *event);
	void display();
	void moveToNextBucket();
	const unsigned int getYearLength() const;
	const unsigned int getBucketWidth() const;
private:
	inline void decrementItemsInCalendar()
	{
		ASSERT(__sync_fetch_and_sub(&itemsInCalendar,1)>0);
	}

	AtomicCalendarQueueBucket **buckets;
	unsigned int currentBucket;
	//Do not make these parameters constant because
	//someone may want them to dynamically change at some point
	unsigned int numberOfBuckets;
	unsigned int bucketWidth;
	unsigned int yearLength;
	unsigned int itemsInCalendar;
};

#endif /* SIMULATIONOBJECTCALENDARQUEUE_H_ */
