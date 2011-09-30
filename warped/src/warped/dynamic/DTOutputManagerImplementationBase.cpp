// See copyright notice in file Copyright in the root directory of this archive.

#include "warped/dynamic/DTOutputEvents.h"
#include "DTOutputManagerImplementationBase.h"
#include "DTTimeWarpSimulationManager.h"
#include "Event.h"

using std::cout;
using std::cerr;
using std::endl;

DTOutputManagerImplementationBase::DTOutputManagerImplementationBase(
		DTTimeWarpSimulationManager *simMgr) :
	mySimulationManager(simMgr),
			myOutputEvents(simMgr->getNumberOfSimulationObjects(), NULL) {
	for (int i = 0; i < myOutputEvents.size(); i++) {
		myOutputEvents[i] = new DTOutputEvents(mySimulationManager);
	}
}

DTOutputManagerImplementationBase::~DTOutputManagerImplementationBase() {
	for (int i = 0; i < myOutputEvents.size(); i++) {
		delete myOutputEvents[i];
	}
}

DTOutputEvents &
DTOutputManagerImplementationBase::getOutputEventsFor(const ObjectID &objectId) {
	return *(myOutputEvents[objectId.getSimulationObjectID()]);
}

void DTOutputManagerImplementationBase::insert(const Event *event, int threadID) {
	getOutputEventsFor(event->getSender()).insert(event, threadID);
}

void DTOutputManagerImplementationBase::fossilCollect(SimulationObject *object,
		const VTime &fossilCollectTime, int threadID) {
	ASSERT(object != NULL);
/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
			<< " )" << "OutputManager fossil collecting for object "
			<< object->getObjectID()->getSimulationObjectID() << " for events "
			<< "prior to time " << fossilCollectTime << endl;*/
	getOutputEventsFor(*(object->getObjectID())).fossilCollect(
			fossilCollectTime, threadID);
}

void DTOutputManagerImplementationBase::fossilCollect(SimulationObject *object,
		int fossilCollectTime, int threadID) {
	ASSERT(object != NULL);
	/*	utils::debug << "OutputManager fossil collecting " << object->getName()
	 << " for events " << "prior to time " << fossilCollectTime << endl;*/
	getOutputEventsFor(*(object->getObjectID())).fossilCollect(
			fossilCollectTime, threadID);
}

void DTOutputManagerImplementationBase::fossilCollectEvent(
		const Event *toRemove, int threadID) {
	getOutputEventsFor(toRemove->getSender()).fossilCollectEvent(toRemove,
			threadID);
}

const Event*
DTOutputManagerImplementationBase::getOldestEvent(unsigned int size,
		int threadID) {
	const Event *objOldest = NULL;
	const Event *retval = NULL;
	retval = myOutputEvents[0]->getOldestEvent(size, threadID);
	for (int i = 1; i < myOutputEvents.size(); i++) {
		objOldest = myOutputEvents[i]->getOldestEvent(size, threadID);
		if (objOldest != NULL) {
			if (retval != NULL) {
				if (objOldest->getSendTime() < retval->getSendTime()) {
					retval = objOldest;
				}
			} else {
				retval = objOldest;
			}
		}
	}
	return retval;
}

void DTOutputManagerImplementationBase::saveOutputCheckpoint(ofstream* outFile,
		const ObjectID &objId, unsigned int saveTime, int threadID) {
	unsigned int i = objId.getSimulationObjectID();
	myOutputEvents[i]->saveOutputCheckpoint(outFile, saveTime, threadID);
}

void DTOutputManagerImplementationBase::ofcPurge(int threadID) {
	for (int i = 0; i < myOutputEvents.size(); i++) {
		myOutputEvents[i]->ofcPurge(threadID);
	}
}
