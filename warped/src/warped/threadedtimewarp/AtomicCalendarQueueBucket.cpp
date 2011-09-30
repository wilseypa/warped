// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationManager.h"
#include "AtomicCalendarQueueBucket.h"
#include "TimeWarpSimulationObjectQueue.h"
#include "TimeWarpSimulationObjectCalendarQueue.h"
#include "warped.h"
#include "AtomicStack.h"
#include "SimulationObject.h"
#include "Event.h"
#include <iostream>
using std::cout;
using std::endl;

AtomicCalendarQueueBucket::AtomicCalendarQueueBucket(TimeWarpSimulationObjectCalendarQueue *calendar, unsigned int index, unsigned int bucketWidth)
	: myCalendar(calendar),
	  currentYearBucket(new AtomicStack<ObjectEventPair>),
	  futureYearsBucket(new AtomicStack<ObjectEventPair>),
	  reorganize(false),
	  myIndex(index),
	  myStartTime(myIndex*bucketWidth),
	  myEndTime(myIndex*bucketWidth + bucketWidth) {}

AtomicCalendarQueueBucket::~AtomicCalendarQueueBucket()
{
	delete currentYearBucket;
	delete futureYearsBucket;
}

ObjectEventPair *AtomicCalendarQueueBucket::getNext()
{
	ObjectEventPair *dataPair = NULL;
	//If bucket is not locked and the the currentYearBucket still refers to the calendars current year
	if (!reorganize)// && myCalendar->getFirstDayOfTheYear() < myEndTime)
	{
		//Attempt to grab an item from the current year
		dataPair = currentYearBucket->pop();
		//If the current year is empty
		if (dataPair==0)
		{
			ReorganizeBucket();
		}
	}
	//Return null indicating this bucket is empty or return useful data
	return dataPair;
}

ObjectEventPair *AtomicCalendarQueueBucket::peekNext()
{
	ObjectEventPair *dataPair = NULL;
	//This function should never be called when the bucket is being reorganized
	ASSERT(!reorganize);
	//Attempt to grab an item from the current year
	dataPair = currentYearBucket->peekNext();
	//If the current year is empty
	if (dataPair==NULL)
	{
		ReorganizeBucket();
	}
	//Return null indicating this bucket is empty or return useful data
	return dataPair;
}

//A failed insert means that the event is actually able to be executed now
bool AtomicCalendarQueueBucket::insert(const unsigned int &time, ObjectEventPair *dataPair)
{
	bool status=false;
	//In the off chance this bucket is being reorganized, halt here until it is finished
	while (reorganize);
	//Do not allow objects from previous years into either of the buckets
	if (time >= myStartTime)
	{
		//Check if this is a current or future object
		if (time < myEndTime)
		{
			currentYearBucket->push(dataPair);
			status=true;
		}
		else
		{
			futureYearsBucket->push(dataPair);
			status=true;
		}
	}
	//cout << "INSERT: " << (SimulationObject*)item->getName() << " : " << myStartTime << " <= " << time << " < " << myEndTime << " retVal=" << status << endl;
	return status;
}

/**Search the futureYearsBucket for objects that are now considered to be in the current year
 * in addition to that remove objects which events no longer match their top events
*/
inline void AtomicCalendarQueueBucket::ReorganizeBucket()
{
	//  Attempt to set the reorganize flag (lock the bucket)
	// If we are lucky enough to do this then we are responsible for
	// all of the bucket maintenance detailed below
	if (__sync_bool_compare_and_swap(&reorganize, false, true))
	{
		//  1. Update the startTime and endtime to be the times for nextYear
		//     this means no new items can be added to a time previous to now
		__sync_fetch_and_add(&myStartTime, myCalendar->getYearLength());
		__sync_fetch_and_add(&myEndTime, myCalendar->getYearLength());
		//  2. Move the bucket counter in the calendar so other threads can begin performing
		//     useful work in the next bucket
		myCalendar->moveToNextBucket();
		//  3. ReorganizeBucket: Pull nextYear objects from the future to put into the current bucket
		ObjectEventPair *tempPair=0;
		AtomicStack<ObjectEventPair> *newFutureBucket = new AtomicStack<ObjectEventPair>();
		bool success = false;
		const Event *topEvent=0;
		while ((tempPair = futureYearsBucket->pop())!=NULL)
		{
			//Check whether this top event is in the current year or in the new future year bucket
			if (tempPair->getReceiveTime().getApproximateIntTime()<myEndTime)
			{
				currentYearBucket->push(tempPair);
			}
			else
			{
				newFutureBucket->push(tempPair);
			}
		}
		delete futureYearsBucket;
		futureYearsBucket = newFutureBucket;
		//  4. Unset the reorganize flag, Their should be nothing preventing us from
		//     flipping the reorganize flag back to false
		__sync_bool_compare_and_swap(&reorganize, true, false);
	}
}
