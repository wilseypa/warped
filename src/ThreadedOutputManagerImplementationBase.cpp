
#include <stddef.h>                     // for NULL
#include <iostream>                     // for cerr, cout

#include "Event.h"                      // for Event
#include "SimulationObject.h"           // for SimulationObject
#include "ThreadedOutputEvents.h"       // for ThreadedOutputEvents
#include "ThreadedOutputManagerImplementationBase.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "VTime.h"                      // for VTime
#include "warped.h"                     // for ASSERT

using std::cout;
using std::cerr;
using std::endl;

ThreadedOutputManagerImplementationBase::ThreadedOutputManagerImplementationBase(
    ThreadedTimeWarpSimulationManager* simMgr) :
    mySimulationManager(simMgr),
    myOutputEvents(simMgr->getNumberOfSimulationObjects(), NULL) {
    for (int i = 0; i < myOutputEvents.size(); i++) {
        myOutputEvents[i] = new ThreadedOutputEvents(mySimulationManager);
    }
}

ThreadedOutputManagerImplementationBase::~ThreadedOutputManagerImplementationBase() {
    for (int i = 0; i < myOutputEvents.size(); i++) {
        delete myOutputEvents[i];
    }
}

ThreadedOutputEvents&
ThreadedOutputManagerImplementationBase::getOutputEventsFor(const ObjectID& objectId) {
    return *(myOutputEvents[objectId.getSimulationObjectID()]);
}

void ThreadedOutputManagerImplementationBase::insert(const Event* event, int threadID) {
    getOutputEventsFor(event->getSender()).insert(event, threadID);
}

void ThreadedOutputManagerImplementationBase::fossilCollect(SimulationObject* object,
                                                            const VTime& fossilCollectTime, int threadID) {
    ASSERT(object != NULL);
    /*  debug::debugout << "( " << mySimulationManager->getSimulationManagerID()
                << " )" << "OutputManager fossil collecting for object "
                << object->getObjectID()->getSimulationObjectID() << " for events "
                << "prior to time " << fossilCollectTime << endl;*/
    getOutputEventsFor(*(object->getObjectID())).fossilCollect(
        fossilCollectTime, threadID);
}

void ThreadedOutputManagerImplementationBase::fossilCollect(SimulationObject* object,
                                                            int fossilCollectTime, int threadID) {
    ASSERT(object != NULL);
    /*  debug::debugout << "OutputManager fossil collecting " << object->getName()
     << " for events " << "prior to time " << fossilCollectTime << endl;*/
    getOutputEventsFor(*(object->getObjectID())).fossilCollect(
        fossilCollectTime, threadID);
}

void ThreadedOutputManagerImplementationBase::fossilCollectEvent(
    const Event* toRemove, int threadID) {
    getOutputEventsFor(toRemove->getSender()).fossilCollectEvent(toRemove,
                                                                 threadID);
}

const Event*
ThreadedOutputManagerImplementationBase::getOldestEvent(unsigned int size,
                                                        int threadID) {
    const Event* objOldest = NULL;
    const Event* retval = NULL;
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

void ThreadedOutputManagerImplementationBase::saveOutputCheckpoint(std::ofstream* outFile,
                                                                   const ObjectID& objId, unsigned int saveTime, int threadID) {
    unsigned int i = objId.getSimulationObjectID();
    myOutputEvents[i]->saveOutputCheckpoint(outFile, saveTime, threadID);
}

void ThreadedOutputManagerImplementationBase::ofcPurge(int threadID) {
    for (int i = 0; i < myOutputEvents.size(); i++) {
        myOutputEvents[i]->ofcPurge(threadID);
    }
}
