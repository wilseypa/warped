// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpSimulationManager.h"
#include "ThreadedTimeWarpEventSet.h"
#include "DefaultTimeWarpEventContainer.h"
#include "Event.h"
#include "SimulationObject.h"
#include "TimeWarpSimulationManager.h"
#include "AtomicSimulationObjectState.h"
#include "LocalEventMessage.h"
#include "LocalNegativeEventMessage.h"
#include "LockedQueue.h"
#include "ObjectEventPair.h"
#include "TimeWarpSimulationObjectLockedQueue.h"
using std::cerr;
using std::cout;
using std::endl;

ThreadedTimeWarpEventSet::ThreadedTimeWarpEventSet(TimeWarpSimulationManager *initSimManager)
	: mySimulationManager(dynamic_cast<ThreadedTimeWarpSimulationManager *>(initSimManager)),
	  simulationObjectQueue(initSimManager->getSimulationObjectQueue()),
	  receivedMessagesQueue(new LockedQueue<LocalKernelMessage*> ()),
	  numberOfWorkerThreads(1),
	  numberOfMessages(0) {
	ASSERT( mySimulationManager!=NULL );
	ASSERT( simulationObjectQueue != NULL );
	unsigned int numberOfObjects = mySimulationManager->getNumberOfSimulationObjects();
	events = new DefaultTimeWarpEventContainer*[numberOfObjects];
	simulationObjectStates = new AtomicSimulationObjectState*[numberOfObjects];
	for (int index = 0; index < mySimulationManager->getNumberOfSimulationObjects(); index++) {
		events[index] = new DefaultTimeWarpEventContainer();
		simulationObjectStates[index] = new AtomicSimulationObjectState();
	}
	rollbackCounter=0;
	numberOfWorkerThreads = mySimulationManager->getNumberOfWorkerThreads();
	mySimulationManager->setEventSetType(this);
}

ThreadedTimeWarpEventSet::~ThreadedTimeWarpEventSet() {
	//Simulation Manager responsible for deleting objectQueue
	ASSERT(numberOfMessages==0);
	delete receivedMessagesQueue;
	for (int index = 0; index
			< mySimulationManager->getNumberOfSimulationObjects(); index++) {
		delete events[index];
		delete simulationObjectStates[index];
	}
	delete[] events;
	delete[] simulationObjectStates;
}

//Called by handle local event so the object we are concerned with
//is the events receiver which should already be locked for us
bool ThreadedTimeWarpEventSet::insert(const Event *event) {
	bool retval = false;
	ASSERT( event != 0 );
	SimulationObject *object = mySimulationManager->getObjectHandle(
			event->getReceiver());
	ASSERT( object != 0 );
	//This is not the first event for this object so the SimObjQueue should remain unchanged
	events[object->getObjectID()->getSimulationObjectID()]->insert(event);
	return true;
}

bool ThreadedTimeWarpEventSet::releaseObject(const unsigned int &threadNumber,
		SimulationObject *object) {
	bool success = false;
	ASSERT( object != NULL );
	unsigned int objID = object->getObjectID()->getSimulationObjectID();
	//You should own the object before being able to release it
	ASSERT(simulationObjectStates[objID]->hasLock(threadNumber));
	const Event *topEvent = peekEvent(object);
	//Check whether this insert is going to cause the ObjectQueue to change
	//If  their are already events in the list and this new event happens before
	//the nextEvent for this object
	if (topEvent == NULL) {
		//A true return means there is nothing else for this function to do
		//with this object, there are no events so nothing left to do.
		success = true;
	} else {
		//Make sure this object makes it into the queue even though it may be a duplicate entry
		success = simulationObjectQueue->insert(object, topEvent);
	}
	//If we successfully put the object back, we should now release our lock
	if (success) {
		//Wait here until we can unlock this object
		success = simulationObjectStates[objID]->releaseLock(threadNumber);
		ASSERT(success);
	}
	return success;
}


bool ThreadedTimeWarpEventSet::lockObject(const unsigned int &threadNumber,
		const OBJECT_ID &objectIDToBeLocked) {
	while (!simulationObjectStates[objectIDToBeLocked.getSimulationObjectID()]->setLock(threadNumber));
}

void ThreadedTimeWarpEventSet::insertLocalMessage(LocalKernelMessage *message) {
	__sync_add_and_fetch(&numberOfMessages, 1);
	//Insert the message
	receivedMessagesQueue->enqueue(message);
}

LocalKernelMessage *ThreadedTimeWarpEventSet::getNextObjectForMessageProc(
		const unsigned int &threadID) {
	//Try to grab the next message
	LocalKernelMessage *topMessage = receivedMessagesQueue->dequeue();
	//If successful lock the receiving object
	if (topMessage != NULL) {
		lockObject(threadID, topMessage->getObjectID());
		__sync_sub_and_fetch(&numberOfMessages, 1);
	}
	return topMessage;
}

SimulationObject *ThreadedTimeWarpEventSet::getNextObjectForEventProc(
		const unsigned int &threadID) {
	const ObjectEventPair *nextPair = NULL;
	SimulationObject *nextObject = NULL;
	int count = 0;
	while ((nextPair = simulationObjectQueue->getNext()) != NULL) {
		//Peel the receiver object off of the objectEventPair
		nextObject = nextPair->getObject();
		//Get the receiver objects objectID
		ObjectID *nextObjectId = nextObject->getObjectID();
		//Lock this object before calling peekEvent
		lockObject(threadID, *nextObjectId);
		//peekEvent will sort the event set if it is currently unsorted to find the top event
		const Event *topEvent = mySimulationManager->peekEvent(
				nextPair->getObject());
		//If topEvent doesnt match what is stored in the objectEventPair
		//this check will fail and we will throw this pair out because it is outdated
		if (topEvent != NULL && nextPair->equals(topEvent)) {
			delete nextPair;
			//Break this loop upon finding an upto date object entry
			break;
		} else {
			//release ther lock on the object and try again
			simulationObjectStates[nextObjectId->getSimulationObjectID()]->releaseLock(
					threadID);
			//This pair is no good because the top events dont match
			delete nextPair;
			//We need to make sure we dont accidently send a object on that was from a bad pair
			nextObject = NULL;
		}
	}
	return nextObject;
}

bool ThreadedTimeWarpEventSet::handleAntiMessage(SimulationObject *object,
		const NegativeEvent *cancelEvent) {
	return events[object->getObjectID()->getSimulationObjectID()]->handleAntiMessage(object, cancelEvent);
}

const Event *
ThreadedTimeWarpEventSet::getEvent(SimulationObject *object) {
	ASSERT( object != 0 );
	const Event *returnEvent = 0;
	//Grab this object from the simulationObjectQueue
	//The time is sent as a hint for where to look
	if (peekEvent(object) != 0) {
		returnEvent = events[object->getObjectID()->getSimulationObjectID()]->getNextEvent();
	}
	return returnEvent;
}

const Event *
ThreadedTimeWarpEventSet::getEvent(SimulationObject *object,
		const VTime &minimumTime) {
	const Event *retval = 0;
	ASSERT( object != 0 );
	const Event *peeked = peekEvent(object, minimumTime);
	if (peeked != 0) {
		retval = getEvent(object);
		ASSERT( peeked == retval );
	}
	return retval;
}

const Event *
ThreadedTimeWarpEventSet::peekEvent(SimulationObject *object) {
	ASSERT( object != 0 );
	return events[object->getObjectID()->getSimulationObjectID()]->nextEvent();
}

const Event *
ThreadedTimeWarpEventSet::peekEvent(SimulationObject *object,
		const VTime &minimumTime) {
	ASSERT( object != 0 );
	const Event *retval = events[object->getObjectID()->getSimulationObjectID()]->nextEvent();
	if (retval != 0) {
		if (!(retval->getReceiveTime() < minimumTime)) {
			retval = 0;
		}
	}
	return retval;
}

void ThreadedTimeWarpEventSet::fossilCollect(SimulationObject *object,
		const VTime &collectTime) {
	events[object->getObjectID()->getSimulationObjectID()]->fossilCollect(object, collectTime);
}

void ThreadedTimeWarpEventSet::fossilCollect(SimulationObject *object,
                int collectTime) {
        events[object->getObjectID()->getSimulationObjectID()]->fossilCollect(object, collectTime);
}

void ThreadedTimeWarpEventSet::fossilCollect(const Event *event) {
	events[event->getReceiver().getSimulationObjectID()]->fossilCollect(event);
}

void ThreadedTimeWarpEventSet::rollback(SimulationObject *object,
		const VTime &rollbackTime) {
  __sync_fetch_and_add(&rollbackCounter,1);
  events[object->getObjectID()->getSimulationObjectID()]->rollback(rollbackTime);
}

bool ThreadedTimeWarpEventSet::inThePast(const Event *toCheck){
	return events[toCheck->getReceiver().getSimulationObjectID()]->isInThePast( toCheck );
}

void ThreadedTimeWarpEventSet::debugDump(const string &objectName, ostream &os) {
	os << "\n---------------\n" << objectName << "\n";
	events[mySimulationManager->getObjectId(objectName).getSimulationObjectID()]->debugDump(os);
}

const AtomicSimulationObjectState *ThreadedTimeWarpEventSet::getSimulationObjectState(
		const SimulationObject * const object) const {
	return simulationObjectStates[object->getObjectID()->getSimulationObjectID()];
}

const VTime &ThreadedTimeWarpEventSet::getEarliestVTime() {
	const VTime &earliestOutput = mySimulationManager->getEarliestOutputMessage();
    const VTime *minTime = simulationObjectQueue->peekNext();
    if (minTime != NULL) {
		if (earliestOutput < *minTime) {
			minTime = &earliestOutput;
		}
    } else {
    	minTime = &earliestOutput;
    }
    return *minTime;
}

void ThreadedTimeWarpEventSet::ofcPurge() {
  SimulationObject *obj = NULL;

  for(int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++){
    obj = mySimulationManager->getObjectHandle(ObjectID(i,mySimulationManager->getSimulationManagerID()));
    (events[i])->ofcPurge(obj);
  }
}
