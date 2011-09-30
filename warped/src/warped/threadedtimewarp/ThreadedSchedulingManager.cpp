// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedSchedulingManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "LocalKernelMessage.h"
#include "warped/Event.h"
#include "AtomicQueue.h"
#include "AtomicSimulationObjectState.h"
#include "TimeWarpSimulationObjectCalendarQueue.h"
#include "ThreadedTimeWarpEventSet.h"

#include <iostream>
using std::cerr;
using std::endl;

ThreadedSchedulingManager::ThreadedSchedulingManager(  ThreadedTimeWarpSimulationManager *initSimulationManager )
  : mySimulationManager( initSimulationManager ),
    myThreadedEventSet(dynamic_cast<ThreadedTimeWarpEventSet*>(mySimulationManager->getEventSetManager())),
    lastScheduledTime( initSimulationManager->getZero().clone() )  {}

ThreadedSchedulingManager::~ThreadedSchedulingManager(){}

const Event *
ThreadedSchedulingManager::peekNextEvent(){
	mySimulationManager->processOnlyMessages();
	const VTime &earliestTime=myThreadedEventSet->getEarliestVTime();
	setLastScheduledEventTime( earliestTime.clone() );
	mySimulationManager->processAllQueues();
	return 0;
}

void
ThreadedSchedulingManager::setLastScheduledEventTime( const VTime *newTimeCopy ){
	delete lastScheduledTime;
	lastScheduledTime = newTimeCopy;
}

const VTime &
ThreadedSchedulingManager::getLastEventScheduledTime(){
  peekNextEvent();
  ASSERT( lastScheduledTime != NULL );
  return *lastScheduledTime;
}

void ThreadedSchedulingManager::configure(SimulationConfiguration &configuration)
{
	ASSERT( myThreadedEventSet != NULL );
}
