
#include "TimeWarpSenderQueue.h"
#include "TimeWarpSimulationManager.h"
#include "Event.h"
#include "SimulationObject.h"
using std::cerr;
using std::endl;

TimeWarpSenderQueue::TimeWarpSenderQueue(TimeWarpSimulationManager* initSimulationManager):
    TimeWarpCentralizedEventSet() {
    senderQMap = new
    std::unordered_map<OBJECT_ID, SenderQueueContainer*, hashObjectID, equal_to<OBJECT_ID> >;
    processedQMap = new
    std::unordered_map<OBJECT_ID, list<Event*>*, hashObjectID, equal_to<OBJECT_ID> >;
    sortedUnProcessedQ = new list<Event*>;
    scheduleList = new vector<ScheduleListContainer*>;
    peekedEvent=0;
    noOfCommittedEvents = 0;
    makeHeapFlag = false;
    popHeapFlag = false;
    mySimulationManager = initSimulationManager;
}


TimeWarpSenderQueue::~TimeWarpSenderQueue() {
    senderQMap->clear();
    processedQMap->clear();
    sortedUnProcessedQ->clear();
    scheduleList->clear();

    delete senderQMap;
    delete processedQMap;
    delete sortedUnProcessedQ;
    delete scheduleList;
    cerr << "Number of committed Events : " <<   noOfCommittedEvents << endl;
}

bool
TimeWarpSenderQueue::insert(Event* event, SimulationObject* object) {
    ASSERT(event != NULL);
    ASSERT(object != NULL);
    //  ASSERT(processedQ != NULL);

    const OBJECT_ID senderID = event->getSender();
    const OBJECT_ID receiverID = event->getReceiver();

    const VTime& receiveTime = event->getReceiveTime();

    if (senderQMap->count(senderID) == 0) {
        //Add the new sender's event
        SenderQueueContainer* senderQContainer = new SenderQueueContainer;

        senderQMap->insert(std::unordered_map<OBJECT_ID,
                           SenderQueueContainer*,
                           hashObjectID,
                           equal_to<OBJECT_ID> >::value_type(senderID, senderQContainer)
                          );


        ScheduleListContainer* scheduleListContainer = new ScheduleListContainer(&
                                                                                 (senderQContainer->headEventPtr));

        scheduleList->insert(scheduleList->end(), scheduleListContainer);
    }


    //ASSERT((*senderQMap)[senderID] != NULL);

    SenderQueueContainer* senderQContainer = ((*senderQMap)[senderID]);
    list<Event*>* senderQ = senderQContainer->senderQ;
    list<Event*>* processedQ = 0;

    if (processedQMap->count(receiverID) == 0) {
        list<Event*>* ptr = new list<Event*>;
        processedQMap->insert(std::unordered_map<OBJECT_ID,list<Event*>*,
                              hashObjectID,
                              equal_to<OBJECT_ID> >::value_type(receiverID, ptr)
                             );
        processedQ = ((*processedQMap)[receiverID]);
    }
    processedQ = ((*processedQMap)[receiverID]);
    //The simulation Time is modified by the simulation manager if the
    // event to be inserted is a straggler.
    //The event is in the past, if the event to be inserted is <=
    // to the last event processed by the receiver object.
    bool inThePast = isInThePast(event);
    if (inThePast) {

        list<Event*> tmpList;
        //push_back all events in processedQ with receiveTime >
        // new Events receiveTime into senderQ
        list<Event*>::iterator beginListIter = processedQ->begin();
        list<Event*>::iterator endListIter = processedQ->end();
        while (beginListIter != endListIter) {
            list<Event*>::iterator tmpIter = beginListIter++;
            Event* tmpEvent = *tmpIter;
            if (tmpEvent->getReceiveTime() >= receiveTime) {
                if (tmpEvent->getReceiver() == event->getReceiver()) {
                    tmpList.push_back(tmpEvent);
                    *tmpIter = 0;
                    processedQ->erase(tmpIter);
                }
            }
        }
        //insert event in the right place
        tmpList.push_back(event);
        //Some events have to be moved into the
        //sortedUnProcessedQ after rollback,
        //since infrequent state savings may be enabled
        //The State saving may have period != 1
        //So add all the events greater than the
        //simulation time into senderQ

        const VTime& simulationTime = object->getSimulationTime();
        while (beginListIter != endListIter) {
            list<Event*>::iterator tmpIter = beginListIter++;
            Event* tmpEvent = *tmpIter;
            if (event->getReceiveTime() > simulationTime) {
                if (tmpEvent->getReceiver() == event->getReceiver()) {
                    tmpList.push_back(tmpEvent);
                    *tmpIter = 0;
                    processedQ->erase(tmpIter);
                }
            } else {
                break;
            }
        }
        //Now tmpList has the elements from the
        //processedQ. This has to be merged into sortedUnProcessedQ
        sortedUnProcessedQ->merge(tmpList, SenderQElementLessThan());
    } else { //if inthePast == true
        bool headFlag = false;
        Event* prevHead = senderQ->back();
        if (prevHead != 0) {
            if (senderQ->front()->getReceiveTime() <= receiveTime) {
                senderQ->push_front(event);
            } else if ((senderQ->back()->getReceiveTime() <= receiveTime) ||
                       (sortedUnProcessedQ->empty() == true)) {
                prevHead = senderQ->back();
                list<Event*>::iterator beginListIter = senderQ->begin();
                list<Event*>::iterator endListIter = senderQ->end();
                while ((beginListIter != endListIter) &&
                        ((*beginListIter)->getReceiveTime() > receiveTime)) {
                    beginListIter++;
                }
                while ((beginListIter != endListIter) &&
                        ((*beginListIter)->getReceiveTime() == receiveTime) &&
                        ((*beginListIter)->getReceiver() > receiverID)) {
                    beginListIter++;
                }
                senderQ->insert(beginListIter,event);
                headFlag = true;
            } else {
                list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
                list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
                while ((beginListIter != endListIter) &&
                        ((*beginListIter)->getReceiveTime() > receiveTime)) {
                    beginListIter++;
                }
                while (beginListIter != endListIter &&
                        (*beginListIter)->getReceiveTime() == receiveTime &&
                        (*beginListIter)->getReceiver() > receiverID) {
                    beginListIter++;
                }
                sortedUnProcessedQ->insert(beginListIter,event);
            }
        } else {
            if (!sortedUnProcessedQ->empty() &&
                    (sortedUnProcessedQ->front()->getReceiveTime() > receiveTime)) {
                list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
                list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
                while ((beginListIter != endListIter) &&
                        ((*beginListIter)->getReceiveTime() > receiveTime)) {
                    beginListIter++;
                }
                while (beginListIter != endListIter &&
                        (*beginListIter)->getReceiveTime() == receiveTime &&
                        (*beginListIter)->getReceiver() > receiverID) {
                    beginListIter++;
                }
                sortedUnProcessedQ->insert(beginListIter,event);
            } else {
                senderQ->push_front(event);
                makeHeapFlag = true;
                senderQContainer->headEventPtr = event;
            }
        }
        if (headFlag) {
            Event* currentHead = senderQ->back();
            if (currentHead != prevHead) {
                //Add the currently inserted element into the
                //scheduleList
                senderQContainer->headEventPtr = currentHead;
                makeHeapFlag = true;
            }
        }
    }//inthepast == true

    peekedEvent = 0;

    return inThePast;
}

void
TimeWarpSenderQueue::handleAntiMessage(Event* event,
                                       SimulationObject* object) {
    ASSERT(event != NULL);
    ASSERT(object != NULL);

    const OBJECT_ID senderID = event->getSender();
    const OBJECT_ID receiverID = event->getReceiver();
    const VTime& receiveTime = event->getReceiveTime();
    const VTime& sendTime = event->getSendTime();
    bool inThePast = isInThePast(event);


    SenderQueueContainer* senderQContainer = ((*senderQMap)[senderID]);
    list<Event*>* senderQ = senderQContainer->senderQ;
    list<Event*>* processedQ = ((*processedQMap)[receiverID]);

    if (inThePast == true) {

        list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
        list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
        while (beginListIter != endListIter) {
            if ((*beginListIter)->getReceiveTime() >= receiveTime) {
                if (((*beginListIter)->getSender() == event->getSender()) &&
                        ((*beginListIter)->getSendTime() >= sendTime) &&
                        ((*beginListIter)->getReceiver() == event->getReceiver())) {
                    list<Event*>::iterator tmpIter = beginListIter++;
                    object->reclaimEvent(*tmpIter);
                    *tmpIter = 0;
                    sortedUnProcessedQ->erase(tmpIter);
                } else {
                    beginListIter++;
                }
            } else {
                break;
            }
        }

        list<Event*> tmpList;
        //push_back all events in processedQ with receiveTime >
        // new Events receiveTime into senderQ
        beginListIter = processedQ->begin();
        endListIter = processedQ->end();
        while ((beginListIter != endListIter) &&
                ((*beginListIter)->getReceiveTime() >= receiveTime)) {
            list<Event*>::iterator tmpIter = beginListIter++;
            Event* tmpEvent = *tmpIter;
            if (tmpEvent->getReceiver() == event->getReceiver()) {
                if (tmpEvent->getSender() != event->getSender()) {
                    tmpList.push_back(tmpEvent);
                } else if (tmpEvent->getSendTime() < sendTime) { //senderid ==event's sender
                    tmpList.push_back(tmpEvent);
                } else {
                    object->reclaimEvent(tmpEvent);
                }
                *tmpIter = 0;
                processedQ->erase(tmpIter);
            } // else do nothing
        }

        const VTime& simulationTime = object->getSimulationTime();
        //The State saving may have period != 1
        //So add all the events greater than the
        //simulation time into senderQ
        while ((beginListIter != endListIter) &&
                ((*beginListIter)->getReceiveTime() > simulationTime)) {
            list<Event*>::iterator tmpIter = beginListIter++;
            Event* tmpEvent = *tmpIter;
            if (tmpEvent->getReceiver() == event->getReceiver()) {
                tmpList.push_back(tmpEvent);
                *tmpIter = 0;
                processedQ->erase(tmpIter);
            }
        }
        sortedUnProcessedQ->merge(tmpList, SenderQElementLessThan());

        //Now Destroy all events in the senderQ of the
        //anti-message's sender
        //rest of the senderQ's events stays
        list<Event*>::iterator beginIter = senderQ->begin();
        list<Event*>::iterator endIter = senderQ->end();
        while ((beginIter != endIter) &&
                ((*beginIter)->getReceiveTime() >= receiveTime)) {
            if ((*beginIter)->getReceiver() == event->getReceiver()) {

                if ((*beginIter)->getSendTime() >= sendTime) {
                    list<Event*>::iterator tmpIter = beginIter++;
                    object->reclaimEvent(*tmpIter);
                    *tmpIter = 0;
                    senderQ->erase(tmpIter);
                } else {
                    beginIter++;
                }

            } else {
                beginIter++;
            }//if((*beginIter)->getReceiiver() == receiverID)
        }//while(beginIter != endIter)
    } else {

        list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
        list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
        while (beginListIter != endListIter) {
            if ((*beginListIter)->getReceiveTime() >= receiveTime) {
                if ((*beginListIter)->getSender() == event->getSender() &&
                        (*beginListIter)->getSendTime() >= sendTime &&
                        (*beginListIter)->getReceiver() == event->getReceiver()) {
                    list<Event*>::iterator tmpIter = beginListIter++;
                    object->reclaimEvent(*tmpIter);
                    *tmpIter = 0;
                    sortedUnProcessedQ->erase(tmpIter);
                } else {
                    beginListIter++;
                }
            } else {
                break;
            }
        }

        //    Insert the antimessage into the sender queue
        if ((!senderQ->empty()) &&
                (senderQ->front()->getReceiveTime() < receiveTime)) {
            //Ignore the Fast Anti Message
            cerr << "Warning!!! Received FAST ANTIMESSAGE" << endl;
            cerr << "Ignoring FAST ANTIMESSAGE and Proceeding with the Simulation... " << endl;
        } else {
            list<Event*>::iterator beginListIter = senderQ->begin();
            list<Event*>::iterator endListIter = senderQ->end();
            //search and remove the messages
            while (beginListIter != endListIter) {
                list<Event*>::iterator tmpIter = beginListIter++;
                Event* tmpEvent = (*tmpIter);
                if (tmpEvent->getReceiveTime() >= receiveTime) {
                    if (tmpEvent->getReceiver() == event->getReceiver() &&
                            tmpEvent->getSendTime() >= sendTime) {
                        object->reclaimEvent(tmpEvent);
                        *tmpIter = 0;
                        senderQ->erase(tmpIter);
                    }
                } else {
                    break;
                }
            }//while(beginListIter != endListIter)
        }//else of !senderQ->empty()
    }//else of inThePast == true

    if (!senderQ->empty()) {
        //Add the currently inserted element into the
        //scheduleList
        Event* currentHead = senderQ->back();
        senderQContainer->headEventPtr = currentHead;
    } else {
        senderQContainer->headEventPtr = 0;
    }
    makeHeapFlag = true;
    peekedEvent = 0;
}

void
TimeWarpSenderQueue::remove(Event* eventToRemove,
                            findMode mode,
                            SimulationObject* simObj) {
}

Event*
TimeWarpSenderQueue::getEvent(SimulationObject* object) {

    ASSERT(object != NULL);
    Event* eventToProcess = 0;

    eventToProcess = peekEvent(object);

    if (eventToProcess != 0) {
        if (!sortedUnProcessedQ->empty()) {
            //remove it from sortedUnProcessed Q
            sortedUnProcessedQ->pop_back();
        } else {
            const OBJECT_ID senderQId = eventToProcess->getSender();

            SenderQueueContainer* senderQContainer = ((*senderQMap)[senderQId]);
            list<Event*>* senderQ = senderQContainer->senderQ;

            senderQ->pop_back();
            popHeapFlag = true;
            if (!senderQ->empty()) {
                //Add the currently inserted element into the
                //scheduleList
                Event* currentHead = senderQ->back();
                senderQContainer->headEventPtr = currentHead;
            } else {
                senderQContainer->headEventPtr = 0;
            }
            push_heap(scheduleList->begin(), scheduleList->end(),
                      GreaterThan_ScheduleListContainerWithReceiverID());
        }

        OBJECT_ID receiverID = *(object->getObjectID());
        list<Event*>* processedQ = 0;
        if (processedQMap->count(receiverID) == 0) {
            list<Event*>* ptr = new list<Event*>;
            processedQMap->insert(std::unordered_map<OBJECT_ID,
                                  list<Event*>*,
                                  hashObjectID,
                                  equal_to<OBJECT_ID> >::value_type(receiverID, ptr)
                                 );
            processedQ = ((*processedQMap)[receiverID]);
        }
        processedQ = ((*processedQMap)[receiverID]);
        ASSERT(processedQ != 0);
        //Add the event to the processed Q
        processedQ->push_front(eventToProcess);
    }

    //return the event
    return eventToProcess;
}

Event*
TimeWarpSenderQueue::peekEvent(SimulationObject* object) {
    Event* eventToProcess = 0;
    OBJECT_ID receiverID;

    if (object != 0) {
        receiverID = mySimulationManager->getObjectId(object->getName());
    }

    if (!sortedUnProcessedQ->empty()) {
        eventToProcess = sortedUnProcessedQ->back();
        if (object == 0) {
            return eventToProcess;
        }
        if (eventToProcess->getReceiver() != *(object->getObjectID())) {
            return (peekedEvent = 0);
        } else {
            return (peekedEvent=eventToProcess);
        }
    } else { //search in the schedule list
        if (scheduleList->empty()) {
            return (peekedEvent=0);
        }

        if (makeHeapFlag) {
            make_heap(scheduleList->begin(), scheduleList->end(),
                      GreaterThan_ScheduleListContainerWithReceiverID());
        }

        if (popHeapFlag || makeHeapFlag) {
            pop_heap(scheduleList->begin(), scheduleList->end(),
                     GreaterThan_ScheduleListContainerWithReceiverID());
            popHeapFlag = false;
        }
        makeHeapFlag = false;

        ScheduleListContainer* scheduleListContainer = scheduleList->back();

        if (object == 0) {
            return *(scheduleListContainer->headEventPtr);
        }

        if (*(scheduleListContainer->headEventPtr) == 0) {
            eventToProcess = 0;
        } else {
            if ((*(scheduleListContainer->headEventPtr))->getReceiver() == *(object->getObjectID())) {
                eventToProcess = *(scheduleListContainer->headEventPtr);
            } else {
                eventToProcess = 0;
            }
        }
    }

    //return the event
    return (peekedEvent=eventToProcess);
}

Event*
TimeWarpSenderQueue::find(const VTime&, findMode, SimulationObject*) {

    cerr << "find() : Currently not supported for Sender Queues" << endl;
    return 0;
}

void
TimeWarpSenderQueue::fossilCollect(const VTime& fossilCollectTime,
                                   SimulationObject* simObj) {
    Event* event = 0;
    OBJECT_ID receiverID = (*simObj->getObjectID());
    list<Event*>* processedQ = ((*processedQMap)[receiverID]);

    while (!processedQ->empty()) {
        event = processedQ->back();
        if (event->getReceiveTime() < fossilCollectTime) {
            simObj->reclaimEvent(event);
            processedQ->pop_back();
            noOfCommittedEvents++;
        } else {
            break;
        }
    }
}


bool
TimeWarpSenderQueue::isInThePast(const Event* event) {
    const VTime& receiveTime = event->getReceiveTime();
    const OBJECT_ID receiverID = event->getReceiver();

    list<Event*>* processedQ = 0;
    if (processedQMap->count(receiverID) == 0) {
        list<Event*>* ptr = new list<Event*>;
        processedQMap->insert(std::unordered_map<OBJECT_ID,
                              list<Event*>*,
                              hashObjectID,
                              equal_to<OBJECT_ID> >::value_type(receiverID, ptr)
                             );
        processedQ = ((*processedQMap)[receiverID]);
        return false;
    }
    processedQ = ((*processedQMap)[receiverID]);
    if (processedQ->empty()) {
        return false;
    } else {
        if (processedQ->front()->getReceiveTime() >= receiveTime) {
            return true;
        } else {
            return false;
        }
    }
}
