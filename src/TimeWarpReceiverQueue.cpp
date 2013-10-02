
#include "Event.h"
#include "SimulationObject.h"
#include "SenderQueueContainer.h"
#include "ReceiverQueueContainer.h"
#include "SchedulingData.h"
#include "TimeWarpMultiSet.h"
#include "TimeWarpAppendQueue.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include <iostream>
using std::cerr;
using std::endl;

TimeWarpReceiverQueue::TimeWarpReceiverQueue(TimeWarpSimulationManager* initSimMgr) :
    schedulingData(0),
    numberOfSimulationObjects(0),
    mySimulationManager(initSimMgr) {}

TimeWarpReceiverQueue::~TimeWarpReceiverQueue() {
    if (!myReceiverQueue.empty()) {
        vector<ReceiverQueueContainer*>::iterator iter_begin =
            myReceiverQueue.begin();
        vector<ReceiverQueueContainer*>::iterator iter_end = myReceiverQueue.end();
        while (iter_begin != iter_end) {
            ReceiverQueueContainer* ptr = *iter_begin;
            delete(*iter_begin)->eventSet;
            ++iter_begin;
            delete ptr;
        }
    }
}

// Insert an event into the event set
bool
TimeWarpReceiverQueue::insert(Event* event, SimulationObject* object) {
    ASSERT(event != NULL);
    ASSERT(object != NULL);
    const OBJECT_ID currentObjectID = mySimulationManager->getObjectId(event->getReceiver());
    ASSERT(myReceiverQueue[currentObjectID.getSimulationObjectID()] != NULL);
    ReceiverQueueContainer* receiverQContainer =
        myReceiverQueue[currentObjectID.getSimulationObjectID()];
    Event* headEvent = receiverQContainer->eventSet->peekEvent(object);
    bool inThePast = receiverQContainer->eventSet->insert(event, object);
    Event* currentHeadEvent = receiverQContainer->eventSet->peekEvent(object);

    if (currentHeadEvent == 0) {
        currentHeadEvent = DummyEvent::instance();
    }

    if (inThePast) {
        receiverQContainer->headEventPtr = currentHeadEvent;
        schedulingData->MakeHeapFlag(true);
    } else {
        if (headEvent != currentHeadEvent) {
            receiverQContainer->headEventPtr = currentHeadEvent;
            schedulingData->MakeHeapFlag(true);
        }
    }
    return inThePast;
}

void
TimeWarpReceiverQueue::remove(Event* event,
                              findMode mode,
                              SimulationObject* object) {
    ASSERT(event != NULL);
    const OBJECT_ID currentObjectID = mySimulationManager->getObjectId(event->getReceiver());
    ASSERT(myReceiverQueue[currentObjectID.getSimulationObjectID()] != NULL);
    myReceiverQueue[currentObjectID.getSimulationObjectID()]->eventSet->remove(event, mode, object);
}

// Returns the event to be processed
Event*
TimeWarpReceiverQueue::getEvent(SimulationObject* object) {
    ASSERT(object != NULL);
    Event* eventToProcess = 0;
    eventToProcess = peekEvent(object);

    if (eventToProcess != 0) {
        const OBJECT_ID receiverId = mySimulationManager->getObjectId(eventToProcess->getReceiver());

        ReceiverQueueContainer* receiverQContainer = myReceiverQueue[receiverId.getSimulationObjectID()];
        TimeWarpEventSet* receiverQ = receiverQContainer->eventSet;

        receiverQ->getEvent(object);
        schedulingData->PopHeapFlag(true);
        Event* nextEvent = receiverQ->peekEvent(object);
        if (nextEvent != 0) {
            //Add the current head Event into the
            //scheduleList
            receiverQContainer->headEventPtr = nextEvent;
        } else {
            receiverQContainer->headEventPtr = DummyEvent::instance();
        }
        push_heap(schedulingData->scheduleList->begin(), schedulingData->scheduleList->end(),
                  GreaterThan_ScheduleListContainerWithReceiverID());
    }
    //return the event
    return eventToProcess;
}

Event*
TimeWarpReceiverQueue::getEvent(SimulationObject* object,
                                const VTIME& time) {
    ASSERT(object != NULL);
    Event* eventToProcess = 0;
    eventToProcess = peekEvent(object, time);

    if (eventToProcess != 0) {
        const OBJECT_ID receiverId = mySimulationManager->getObjectId(eventToProcess->getReceiver());

        ReceiverQueueContainer* receiverQContainer = myReceiverQueue[receiverId.getSimulationObjectID()];
        TimeWarpEventSet* receiverQ = receiverQContainer->eventSet;

        receiverQ->getEvent(object);

        Event* nextEvent = receiverQ->peekEvent(object);
        if (nextEvent != 0) {
            //Add the current head Event into the
            //scheduleList
            receiverQContainer->headEventPtr = nextEvent;
        } else {
            receiverQContainer->headEventPtr = DummyEvent::instance();
        }
        schedulingData->MakeHeapFlag(true);
    }
    //return the event
    return eventToProcess;
} // End of getEvent(...)

// Do I have an event to execute ?
Event*
TimeWarpReceiverQueue::peekEvent(SimulationObject* object) {
    ASSERT(object != NULL);
    Event* eventToProcess = 0;
    if (schedulingData->scheduleList->empty()) {
        return 0;
    }

    if (schedulingData->MakeHeapFlag() == true) {
        make_heap(schedulingData->scheduleList->begin(), schedulingData->scheduleList->end(),
                  GreaterThan_ScheduleListContainerWithReceiverID());
    }

    if ((schedulingData->PopHeapFlag()== true) ||
            (schedulingData->MakeHeapFlag() == true)) {
        pop_heap(schedulingData->scheduleList->begin(), schedulingData->scheduleList->end(),
                 GreaterThan_ScheduleListContainerWithReceiverID());
        schedulingData->PopHeapFlag(false);
    }
    schedulingData->MakeHeapFlag(false);

    ScheduleListContainer* scheduleListContainer = schedulingData->scheduleList->back();

    if (*(scheduleListContainer->headEventPtr) == DummyEvent::instance()) {
        return 0;
    } else {
        eventToProcess = *(scheduleListContainer->headEventPtr);
    }

    if (eventToProcess->getReceiver() != object->getName()) {
        return 0;
    }
    return eventToProcess;
}

Event*
TimeWarpReceiverQueue::peekEvent(SimulationObject* object,
                                 const VTIME& time) {
    ASSERT(object != NULL);
    OBJECT_ID* currentObjectID = object->getObjectID();
    ASSERT(myReceiverQueue[currentObjectID->getSimulationObjectID()]!=NULL);
    Event* event =
        myReceiverQueue[currentObjectID->getSimulationObjectID()]->eventSet->peekEvent(object);

    schedulingData->MakeHeapFlag(true);

    if (event != NULL && event->getReceiveTime() < time) {
        return event;
    } else {
        return NULL;
    }
} // End of peekEvent(...).

// locate an event in the queue
Event*
TimeWarpReceiverQueue::find(const VTIME& time, findMode mode,
                            SimulationObject* object) {
    ASSERT(object != NULL);
    OBJECT_ID* currentObjectID = object->getObjectID();
    ASSERT(myReceiverQueue[currentObjectID->getSimulationObjectID()]!=NULL);
    return myReceiverQueue[currentObjectID->getSimulationObjectID()]->eventSet->find(time, mode,
                                                                                     object);
}

// delete any unwanted elements
void
TimeWarpReceiverQueue::fossilCollect(const VTIME& fossilCollectTime,
                                     SimulationObject* object) {

    ASSERT(object != NULL);
    OBJECT_ID* objID = object->getObjectID();
    ASSERT(myReceiverQueue[objID->getSimulationObjectID()] != NULL);
    myReceiverQueue[objID->getSimulationObjectID()]->eventSet->fossilCollect(fossilCollectTime, object);
}

void
TimeWarpReceiverQueue::handleAntiMessage(Event* event,
                                         SimulationObject* object) {
    ASSERT(object != NULL);
    OBJECT_ID* objID = object->getObjectID();
    ASSERT(myReceiverQueue[objID->getSimulationObjectID()] != NULL);
    ReceiverQueueContainer* receiverQContainer = myReceiverQueue[objID->getSimulationObjectID()];
    receiverQContainer->eventSet->handleAntiMessage(event, object);
    Event* currentHeadEvent = receiverQContainer->eventSet->peekEvent(object);
    if (currentHeadEvent != 0) {
        receiverQContainer->headEventPtr = currentHeadEvent;
    } else {
        receiverQContainer->headEventPtr = DummyEvent::instance();
    }
    schedulingData->MakeHeapFlag(true);
}


void
TimeWarpReceiverQueue::configure(SimulationConfiguration& configuration) {
    std::string eventListValue = configuration.get_string({"TimeWarp", "EventList", "Type"},
                                                          "MultiSet");
    for (unsigned int count = 0; count < numberOfSimulationObjects; count++) {
        ReceiverQueueContainer* receiverQContainer = new ReceiverQueueContainer();

        if (eventListValue == "MultiSet") {
            receiverQContainer->eventSet = new TimeWarpMultiSet(mySimulationManager);
        } else if (eventListValue == "AppendQ") {
            receiverQContainer->eventSet = new TimeWarpAppendQueue(mySimulationManager);
        } else {
            cerr << "Invalid EventList setting" << endl;
            exit(-1);
        }

        this->myReceiverQueue.push_back(receiverQContainer);
        ScheduleListContainer* scheduleListContainer = new ScheduleListContainer(
            &receiverQContainer->headEventPtr);
        this->schedulingData->scheduleList->push_back(scheduleListContainer);
    }
}


void
TimeWarpReceiverQueue::setSchedulingData(SchedulingData* data) {
    schedulingData = data;
}

SchedulingData*
TimeWarpReceiverQueue::getSchedulingData(void) {
    return schedulingData;
}

void
TimeWarpReceiverQueue::setNumberOfSimulationObjects(unsigned int numObjs) {
    numberOfSimulationObjects = numObjs;
}

unsigned int
TimeWarpReceiverQueue::getNumberOfSimulationObjects(void) {
    return this->numberOfSimulationObjects;
}
