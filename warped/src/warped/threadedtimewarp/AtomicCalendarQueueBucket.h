#ifndef CALENDARQUEUE_H_
#define CALENDARQUEUE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "ObjectEventPair.h"
class TimeWarpSimulationObjectCalendarQueue;
template<class element> class AtomicStack;

/**Atomic Calendar Queue Bucket
  In the original implementation of the AtomicCalendarQueue
the bucket was a single sorted linked list.

This implementation divides the objects into two groups
  The first is the currentYear which is a basic queue
  The second is all future years
As the last item is removed from the bucket, a maintenance routine
is called to move next year's objects from the futureYears bucket
and put them into the current years bucket
*/
class AtomicCalendarQueueBucket
{
public:
	AtomicCalendarQueueBucket(TimeWarpSimulationObjectCalendarQueue *calendar, unsigned int index, unsigned int bucketWidth);
	~AtomicCalendarQueueBucket();
	ObjectEventPair *getNext();
	ObjectEventPair *peekNext();
	bool insert(const unsigned int &time, ObjectEventPair *dataPair);
private:
	///Search the futureYearsBucket for objects that are now considered to be in the current year
	void ReorganizeBucket();
	TimeWarpSimulationObjectCalendarQueue *myCalendar;
	AtomicStack<ObjectEventPair> *currentYearBucket;
	AtomicStack<ObjectEventPair> *futureYearsBucket;
	bool reorganize;
	unsigned int myIndex;
	unsigned int myStartTime;
	unsigned int myEndTime;
};

#endif /* CALENDARQUEUE_H_ */
