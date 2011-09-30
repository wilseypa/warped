// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationManager.h"
#include "TimeWarpSimulationObjectCalendarQueue.h"
#include "threadedtimewarp/AtomicCalendarQueueBucket.h"
#include "SimulationObject.h"
#include "Event.h"
#include <iostream>
using std::cout;
using std::endl;
TimeWarpSimulationObjectCalendarQueue::TimeWarpSimulationObjectCalendarQueue(TimeWarpSimulationManager *initSimManager,
																			 unsigned int numberOfBuckets,
																			 unsigned int bucketWidth)
	: TimeWarpSimulationObjectQueue(initSimManager),
	  currentBucket(0),
	  numberOfBuckets(numberOfBuckets),
	  bucketWidth(bucketWidth),
	  yearLength(numberOfBuckets*bucketWidth),
	  itemsInCalendar(0)
{
	ASSERT(mySimulationManager!=NULL);
	ASSERT(numberOfBuckets>0);
	buckets = new AtomicCalendarQueueBucket*[numberOfBuckets];
	//Initialize all of the buckets in the calendar
	for (unsigned int index=0; index <numberOfBuckets; index++)
	{
		buckets[index] = new AtomicCalendarQueueBucket(this, index, bucketWidth);
	}
}

TimeWarpSimulationObjectCalendarQueue::~TimeWarpSimulationObjectCalendarQueue() {
	for (unsigned int index=0; index <numberOfBuckets; index++)	{
		delete buckets[index];
	}
	delete []buckets;
}

const ObjectEventPair *TimeWarpSimulationObjectCalendarQueue::getNext() {
	const ObjectEventPair *nextPair= NULL;
	//Keep attempting to grab the next item in the calendar while there are still items left
	while ((itemsInCalendar > 0) && (nextPair = buckets[currentBucket]->getNext()) == NULL);
	if (nextPair!=NULL)	{
		//decrement the number of items in the calendar
		decrementItemsInCalendar();
	}
	return nextPair;
}

const VTime *TimeWarpSimulationObjectCalendarQueue::peekNext() {
	ObjectEventPair *topPair = NULL;
	const VTime *minimumTime = NULL;
	//Keep attempting to grab the next item in the calendar while there are still items left
	while ((itemsInCalendar > 0) && (topPair = buckets[currentBucket]->peekNext()) == NULL);
	//This is not completely safe, someone could dequeue this topPair before we grab our value
	if (topPair != NULL) {
		minimumTime = &topPair->getReceiveTime();
	}
	return minimumTime;
}

bool TimeWarpSimulationObjectCalendarQueue::insert(SimulationObject *object,const Event *event) {
	bool success=false;
	//Get the VTime, and store as an unsigned int for indexing
	const unsigned int time = event->getReceiveTime().getApproximateIntTime();
	//Calculate the ideal bucketIndex by:
	//   idealIndex = time / bucketWidth;
	//and the actual by:
	//   actualIndex = idealIndex % numberOfBuckets;
	const unsigned int bucketIndex = (time / bucketWidth) % numberOfBuckets;
	ObjectEventPair *dataPair = new ObjectEventPair(object, event);
	if (success = buckets[bucketIndex]->insert(time, dataPair))	{
		//increment the number of items in the calendar
		__sync_fetch_and_add(&itemsInCalendar,1);
	}
	else {
		delete dataPair;
	}
	return success;
}

const unsigned int TimeWarpSimulationObjectCalendarQueue::getYearLength() const {
	return yearLength;
}

const unsigned int TimeWarpSimulationObjectCalendarQueue::getBucketWidth() const {
	return bucketWidth;
}

void TimeWarpSimulationObjectCalendarQueue::moveToNextBucket() {
	unsigned int current = currentBucket;
	unsigned int nextBucket = (current + 1) % numberOfBuckets;
	//This change is done behind a reorganize "lock" so the use of compare_and_swap is so no one
	//is reading it while we make our change
	__sync_bool_compare_and_swap(&currentBucket, current, nextBucket);
}

void TimeWarpSimulationObjectCalendarQueue::display() {

}
