
#include <stddef.h>                     // for NULL
#include <algorithm>                    // for find
#include <list>

#include "Event.h"                      // for Event
#include "EventId.h"                    // for EventId
#include "ObjectID.h"                   // for ObjectID
#include "OptFossilCollManager.h"       // for OptFossilCollManager
#include "OutputEvents.h"
#include "SerializedInstance.h"         // for SerializedInstance
#include "SetObject.h"                  // for SetObject, ostream
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "VTime.h"                      // for VTime
#include "WarpedDebug.h"                // for debugout
#include "warped.h"                     // for ASSERT

OutputEvents::OutputEvents(TimeWarpSimulationManager* simMgr):
    mySimulationManager(simMgr) {
}

OutputEvents::~OutputEvents() {
}

vector<const Event*>*
OutputEvents::getEventsSentAtOrAfter(const VTime& searchTime) {
    vector<const Event*>* retval = new vector<const Event*>;

    vector< SetObject<Event>* >::iterator out = outputEventsLocal.end();
    if (out != outputEventsLocal.begin()) {
        out--;

        while (out != outputEventsLocal.begin() && (*out)->getMainTime() >= searchTime) {
            if (mySimulationManager->getOptFossilColl()) {
                int lastTime = mySimulationManager->getFossilCollManager()->getLastCollectTime((
                                                                                                   *out)->getReceiverID());
                int recvTime = (*out)->getSecondaryTime().getApproximateIntTime();
                if (recvTime <= lastTime) {
                    debug::debugout << mySimulationManager->getSimulationManagerID() <<
                                    " - Cata Rollback in outputevents: " << recvTime
                                    << ", " << lastTime << std::endl;
                    mySimulationManager->getFossilCollManager()->startRecovery((*out)->getReceiverID(),
                                                                               (*out)->getMainTime().getApproximateIntTime());
                    break;
                } else {
                    retval->push_back((*out)->getElement());
                }
            } else {
                retval->push_back((*out)->getElement());
            }
            out--;
        }

        if (out == outputEventsLocal.begin() && (*out)->getMainTime() >= searchTime) {
            retval->push_back((*out)->getElement());
        }
    }

    vector<const Event*>::iterator it = outputEventsRemote.end();
    if (it != outputEventsRemote.begin()) {
        it--;

        while (it != outputEventsRemote.begin() && (*it)->getSendTime() >= searchTime) {
            retval->push_back((*it));
            it--;
        }

        if (it == outputEventsRemote.begin() && (*it)->getSendTime() >= searchTime) {
            retval->push_back((*it));
        }
    }

    return retval;
}

vector<const Event*>*
OutputEvents::getEventsSentAtOrAfterAndRemove(const VTime& searchTime) {
    vector<const Event*>* retval = new vector<const Event*>;

    vector< SetObject<Event>* >::iterator out = outputEventsLocal.end();
    if (out != outputEventsLocal.begin()) {
        out--;

        while (out != outputEventsLocal.begin() && (*out)->getMainTime() >= searchTime) {
            if (mySimulationManager->getOptFossilColl()) {
                int lastTime = mySimulationManager->getFossilCollManager()->getLastCollectTime((
                                                                                                   *out)->getReceiverID());
                int recvTime = (*out)->getSecondaryTime().getApproximateIntTime();
                if (recvTime <= lastTime) {
                    debug::debugout << mySimulationManager->getSimulationManagerID() <<
                                    " - Cata Rollback in outputevents: " << recvTime
                                    << ", " << lastTime << std::endl;
                    mySimulationManager->getFossilCollManager()->startRecovery((*out)->getReceiverID(),
                                                                               (*out)->getMainTime().getApproximateIntTime());
                    break;
                } else {
                    retval->push_back((*out)->getElement());
                    delete(*out);
                    out = outputEventsLocal.erase(out);
                }
            } else {
                retval->push_back((*out)->getElement());
                delete(*out);
                out = outputEventsLocal.erase(out);
            }
            out--;
        }

        if (out == outputEventsLocal.begin() && (*out)->getMainTime() >= searchTime) {
            retval->push_back((*out)->getElement());
            outputEventsLocal.erase(out);
        }
    }

    vector<const Event*>::iterator it = outputEventsRemote.end();
    if (it != outputEventsRemote.begin()) {
        it--;

        while (it != outputEventsRemote.begin() && (*it)->getSendTime() >= searchTime) {
            retval->push_back(*it);
            removedEventsRemote.push_back(*it);
            outputEventsRemote.erase(it);
            it--;
        }

        if (it == outputEventsRemote.begin() && (*it)->getSendTime() >= searchTime) {
            retval->push_back(*it);
            removedEventsRemote.push_back(*it);
            outputEventsRemote.erase(it);
            it--;
        }
    }

    return retval;
}

void
OutputEvents::fossilCollect(const VTime& gCollectTime) {
    // The events sent to another simulation manager need to be deleted here.
    // The events sent to this same simulation manager (local events) will be deleted
    // in the event set and should not be deleted here.
    vector<const Event*>::iterator outRem = outputEventsRemote.begin();
    while (outRem != outputEventsRemote.end() && (*outRem)->getSendTime() < gCollectTime) {
        delete(*outRem);
        outRem++;
    }
    outputEventsRemote.erase(outputEventsRemote.begin(), outRem);

    vector< SetObject<Event>* >::iterator outLoc = outputEventsLocal.begin();
    while (outLoc != outputEventsLocal.end() && (*outLoc)->getMainTime() < gCollectTime) {
        delete *outLoc;
        outLoc++;
    }
    outputEventsLocal.erase(outputEventsLocal.begin(), outLoc);

    std::list<const Event*>::iterator rmRt = removedEventsRemote.begin();
    while (rmRt != removedEventsRemote.end()) {
        if ((*rmRt)->getSendTime() < gCollectTime) {
            delete *rmRt;
            rmRt = removedEventsRemote.erase(rmRt);
        } else {
            rmRt++;
        }
    }
}

void
OutputEvents::fossilCollect(int gCollectTime) {
    // The events sent to another simulation manager need to be deleted here.
    // The events sent to this same simulation manager (local events) will be deleted
    // in the event set and should not be deleted here.
    vector<const Event*>::iterator outRem = outputEventsRemote.begin();
    while (outRem != outputEventsRemote.end() &&
            (*outRem)->getSendTime().getApproximateIntTime() < gCollectTime) {
        delete(*outRem);
        outRem++;
    }
    outputEventsRemote.erase(outputEventsRemote.begin(), outRem);

    vector< SetObject<Event>* >::iterator outLoc = outputEventsLocal.begin();
    while (outLoc != outputEventsLocal.end() &&
            (*outLoc)->getMainTime().getApproximateIntTime() < gCollectTime) {
        delete *outLoc;
        outLoc++;
    }
    outputEventsLocal.erase(outputEventsLocal.begin(), outLoc);

    std::list<const Event*>::iterator rmRt = removedEventsRemote.begin();
    while (rmRt != removedEventsRemote.end()) {
        if ((*rmRt)->getSendTime().getApproximateIntTime() < gCollectTime) {
            delete *rmRt;
            rmRt = removedEventsRemote.erase(rmRt);
        } else {
            rmRt++;
        }
    }
}

const Event*
OutputEvents::getOldestEvent(unsigned int size) {
    const Event* retval = NULL;

    vector<const Event*>::iterator it = outputEventsRemote.begin();
    while (it != outputEventsRemote.end() && (*it)->getEventSize() != size) {
        it++;
    }

    if (it != outputEventsRemote.end()) {
        retval = *it;
        removedEventToRemove = false;
    }

    if (retval != NULL) {
        eventToRemove = it;
    }

    return retval;
}

void
OutputEvents::insert(const Event* newEvent) {
    bool isLocal = mySimulationManager->contains(newEvent->getReceiver());
    vector<const Event*>::iterator i;

    if (isLocal) {
        outputEventsLocal.push_back(new SetObject<Event>(newEvent->getSendTime(),
                                                         newEvent->getReceiveTime(),
                                                         newEvent->getReceiver().getSimulationObjectID(),
                                                         newEvent));
    } else {
        std::list<const Event*>::iterator it;
        it = std::find(removedEventsRemote.begin(), removedEventsRemote.end(), newEvent);
        if (it != removedEventsRemote.end()) {
            removedEventsRemote.erase(it);
        }
        outputEventsRemote.push_back(newEvent);
    }
}

// Only used for memory management in optimistic fossil collection.
void
OutputEvents::fossilCollectEvent(const Event* toRemove) {
    vector< SetObject<Event>* >::iterator sit;
    vector<const Event*>::iterator it;
    bool foundMatch = false;

    sit = outputEventsLocal.begin();
    while (sit != outputEventsLocal.end()) {
        if ((*sit)->getElement() == toRemove) {
            delete *sit;
            outputEventsLocal.erase(sit);
            foundMatch = true;
            break;
        } else {
            sit++;
        }
    }

    it = outputEventsRemote.begin();
    while (it != outputEventsRemote.end()) {
        if (toRemove->getEventId() == (*it)->getEventId() && toRemove->getSender() == (*it)->getSender()) {
            outputEventsRemote.erase(it);
            foundMatch = true;
            break;
        } else {
            it++;
        }
    }
}

void
OutputEvents::remove(const Event* toRemove) {
    vector<const Event*>::iterator i;
    vector< SetObject<Event>* >::iterator sit = outputEventsLocal.begin();
    bool found = false;

    while (sit != outputEventsLocal.end()) {
        if ((*sit)->getElement() == toRemove) {
            delete *sit;
            outputEventsLocal.erase(sit);
            found = true;
            break;
        } else {
            sit++;
        }
    }

    if (!found) {
        i = std::find(outputEventsRemote.begin(), outputEventsRemote.end(), toRemove);

        ASSERT(i != outputEventsRemote.end());
        outputEventsRemote.erase(i);
    }
}

void
OutputEvents::remove(const vector<const Event*>& toRemove) {
    for (vector<const Event*>::const_iterator i = toRemove.begin(); i != toRemove.end(); i++) {
        remove(*i);
    }
}

void
OutputEvents::saveOutputCheckpoint(std::ofstream* outFile, unsigned int saveTime) {
    vector<const Event*>::iterator outRem = outputEventsRemote.begin();
    char del = '_';
    unsigned int eveSize = 0;
    const char* charPtr = NULL;
    SerializedInstance* toWrite = NULL;

    while (outRem != outputEventsRemote.end()) {
        if ((*outRem)->getSendTime().getApproximateIntTime() < saveTime &&
                (*outRem)->getReceiveTime().getApproximateIntTime() >= saveTime) {

            toWrite = new SerializedInstance((*outRem)->getDataType());
            (*outRem)->serialize(toWrite);
            charPtr = &toWrite->getData()[0];
            eveSize = toWrite->getSize();

            outFile->write((char*)(&eveSize), sizeof(eveSize));
            outFile->write(&del, sizeof(del));
            outFile->write(charPtr, eveSize);
        }
        outRem++;
    }

    vector< SetObject<Event>* >::iterator outLoc = outputEventsLocal.begin();
    while (outLoc != outputEventsLocal.end()) {
        if ((*outLoc)->getMainTime().getApproximateIntTime() < saveTime &&
                (*outLoc)->getSecondaryTime().getApproximateIntTime() >= saveTime) {

            int lastTime = mySimulationManager->getFossilCollManager()->getLastCollectTime((
                                                                                               *outLoc)->getReceiverID());
            int recvTime = (*outLoc)->getSecondaryTime().getApproximateIntTime();
            if (recvTime <= lastTime) {
                debug::debugout << mySimulationManager->getSimulationManagerID() <<
                                " - Cata Rollback in OutputEvents::checkpoint: " << recvTime
                                << ", " << lastTime << std::endl;
                mySimulationManager->getFossilCollManager()->startRecovery((*outLoc)->getReceiverID(),
                                                                           (*outLoc)->getMainTime().getApproximateIntTime());
                break;
            } else {

                toWrite = new SerializedInstance((*outLoc)->getElement()->getDataType());
                (*outLoc)->getElement()->serialize(toWrite);
                charPtr = &toWrite->getData()[0];
                eveSize = toWrite->getSize();

                outFile->write((char*)(&eveSize), sizeof(eveSize));
                outFile->write(&del, sizeof(del));
                outFile->write(charPtr, eveSize);
                delete toWrite;
            }
        }
        outLoc++;
    }
}

void
OutputEvents::ofcPurge() {
    // Directly call the destructor and release the memory to avoid the
    // overloaded delete. Delete remote events and remove them from the queues.
    // Just clear the local events.
    vector<const Event*>::iterator outRem = outputEventsRemote.begin();
    while (outRem != outputEventsRemote.end()) {
        (*outRem)->~Event();
        ::operator delete((void*)(*outRem));
        outRem++;
    }
    outputEventsRemote.clear();

    vector< SetObject<Event>* >::iterator outLoc = outputEventsLocal.begin();
    while (outLoc != outputEventsLocal.end()) {
        delete *outLoc;
        outLoc++;
    }
    outputEventsLocal.clear();

    std::list<const Event*>::iterator rmRt = removedEventsRemote.begin();
    while (rmRt != removedEventsRemote.end()) {
        delete *rmRt;
        rmRt = removedEventsRemote.erase(rmRt);
    }
}
