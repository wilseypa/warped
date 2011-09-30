// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationObjectQueueFactory.h"
#include "TimeWarpSimulationObjectQueue.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationObject.h"

TimeWarpSimulationObjectQueue::TimeWarpSimulationObjectQueue(TimeWarpSimulationManager *initSimulationManager)
	  : mySimulationManager(initSimulationManager)//,
	   // inQueueArray(new bool[initSimulationManager->getNumberOfSimulationObjects()])
{
	/*for (unsigned int index = 0; index < mySimulationManager->getNumberOfSimulationObjects(); index++)
	{
		inQueueArray[index]=false;
	}*/
}

TimeWarpSimulationObjectQueue::~TimeWarpSimulationObjectQueue() {}

/*bool TimeWarpSimulationObjectQueue::isInQueue(const SimulationObject *object)
{
	return inQueueArray[object->getObjectID()->getSimulationObjectID()];
}

bool TimeWarpSimulationObjectQueue::setInQueue(const SimulationObject *object) {
	//If currently not in queue and we can set it to in queue then return true else return false;
	return  __sync_bool_compare_and_swap(&inQueueArray[object->getObjectID()->getSimulationObjectID()], false, true);
}
bool TimeWarpSimulationObjectQueue::setNotInQueue(const SimulationObject *object) {
	//If currently in queue and we can set it to not inqueue then return true else return false;
	return __sync_bool_compare_and_swap(&inQueueArray[object->getObjectID()->getSimulationObjectID()], true, false);;
}*/
