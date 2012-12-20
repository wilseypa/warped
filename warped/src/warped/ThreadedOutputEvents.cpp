// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedOutputEvents.h"
#include "EventFunctors.h"
#include "Serializable.h"
#include "SerializedInstance.h"
#include "SetObject.h"
#include "ThreadedOptFossilCollManager.h"
#include <algorithm>

ThreadedOutputEvents::ThreadedOutputEvents(ThreadedTimeWarpSimulationManager *simMgr) :
	mySimulationManager(simMgr) {

	localQueueLock = new LockState();
	remoteQueueLock = new LockState();
	removedListLock = new LockState();
}

ThreadedOutputEvents::~ThreadedOutputEvents() {
	delete localQueueLock;
	delete remoteQueueLock;
	delete removedListLock;
}

vector<const Event *> *
ThreadedOutputEvents::getEventsSentAtOrAfter(const VTime &searchTime, int threadID) {
	vector<const Event *> *retval = new vector<const Event *> ;
	this->getLocalLock(threadID);
	vector<SetObject<Event>*>::iterator out = outputEventsLocal.end();
	if (out != outputEventsLocal.begin()) {
		out--;

		while (out != outputEventsLocal.begin() && (*out)->getMainTime()
				>= searchTime) {
			if (mySimulationManager->getOptFossilColl()) {
				int
						lastTime =
								mySimulationManager->getOptFossilCollManagerNew()->getLastCollectTime(
										(*out)->getReceiverID());
				int recvTime =
						(*out)->getSecondaryTime().getApproximateIntTime();
				if (recvTime <= lastTime) {
					utils::debug
							<< mySimulationManager->getSimulationManagerID()
							<< " - Cata Rollback in outputevents: " << recvTime
							<< ", " << lastTime << std::endl;
					mySimulationManager->getOptFossilCollManagerNew()->setRecovery(
							(*out)->getReceiverID(),
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

		if (out == outputEventsLocal.begin() && (*out)->getMainTime()
				>= searchTime) {
			retval->push_back((*out)->getElement());
		}
	}
	this->releaseLocalLock(threadID);

	this->getRemoteLock(threadID);
	vector<const Event *>::iterator it = outputEventsRemote.end();
	if (it != outputEventsRemote.begin()) {
		it--;

		while (it != outputEventsRemote.begin() && (*it)->getSendTime()
				>= searchTime) {
			retval->push_back((*it));
			it--;
		}

		if (it == outputEventsRemote.begin() && (*it)->getSendTime()
				>= searchTime) {
			retval->push_back((*it));
		}
	}
	this->releaseRemoteLock(threadID);

	return retval;
}

vector<const Event *> *
ThreadedOutputEvents::getEventsSentAtOrAfterAndRemove(const VTime &searchTime,
		int threadID) {
	vector<const Event *> *retval = new vector<const Event *> ;

	this->getLocalLock(threadID);
	vector<SetObject<Event>*>::iterator out = outputEventsLocal.end();
	if (out != outputEventsLocal.begin()) {
		out--;

		while (out != outputEventsLocal.begin() && (*out)->getMainTime()
				>= searchTime) {
			if (mySimulationManager->getOptFossilColl()) {
				int
						lastTime =
								mySimulationManager->getOptFossilCollManagerNew()->getLastCollectTime(
										(*out)->getReceiverID());
				int recvTime =
						(*out)->getSecondaryTime().getApproximateIntTime();
				if (recvTime <= lastTime) {
					utils::debug
							<< mySimulationManager->getSimulationManagerID()
							<< " - Cata Rollback in outputevents: " << recvTime
							<< ", " << lastTime << std::endl;
					mySimulationManager->getOptFossilCollManagerNew()->setRecovery(
							(*out)->getReceiverID(),
							(*out)->getMainTime().getApproximateIntTime());
					break;
				} else {
					retval->push_back((*out)->getElement());
					delete (*out);
					out = outputEventsLocal.erase(out);
				}
			} else {
				retval->push_back((*out)->getElement());
				delete (*out);
				out = outputEventsLocal.erase(out);
			}
			out--;
		}

		if (out == outputEventsLocal.begin() && (*out)->getMainTime()
				>= searchTime) {
            if ((*out)->getElement()->getSender()
                    != (*out)->getElement()->getReceiver())
                retval->push_back((*out)->getElement());
            outputEventsLocal.erase(out);
		}
	}
	this->releaseLocalLock(threadID);

	this->getRemoteLock(threadID);
	vector<const Event *>::iterator it = outputEventsRemote.end();
	if (it != outputEventsRemote.begin()) {
		it--;

		while (it != outputEventsRemote.begin() && (*it)->getSendTime()
				>= searchTime) {
			retval->push_back(*it);
			removedEventsRemote.push_back(*it);
			outputEventsRemote.erase(it);
			it--;
		}

		if (it == outputEventsRemote.begin() && (*it)->getSendTime()
				>= searchTime) {
			retval->push_back(*it);
			removedEventsRemote.push_back(*it);
			outputEventsRemote.erase(it);
			it--;
		}
	}
	this->releaseRemoteLock(threadID);

	return retval;
}

void ThreadedOutputEvents::fossilCollect(const VTime &gCollectTime, int threadID) {
	// The events sent to another simulation manager need to be deleted here.
	// The events sent to this same simulation manager (local events) will be deleted
	// in the event set and should not be deleted here.

	this->getRemoteLock(threadID);
	vector<const Event*>::iterator outRem = outputEventsRemote.begin();
	while (outRem != outputEventsRemote.end() && (*outRem)->getSendTime()
			< gCollectTime) {
		delete (*outRem);
		outRem++;
	}
	outputEventsRemote.erase(outputEventsRemote.begin(), outRem);
	this->releaseRemoteLock(threadID);

	this->getLocalLock(threadID);
	vector<SetObject<Event>*>::iterator outLoc = outputEventsLocal.begin();
	while (outLoc != outputEventsLocal.end() && (*outLoc)->getMainTime()
			< gCollectTime) {
		delete *outLoc;
		outLoc++;
	}
	outputEventsLocal.erase(outputEventsLocal.begin(), outLoc);
	this->releaseLocalLock(threadID);

	this->getRemovedLock(threadID);
	list<const Event*>::iterator rmRt = removedEventsRemote.begin();
	while (rmRt != removedEventsRemote.end()) {
		if ((*rmRt)->getSendTime() < gCollectTime) {
			delete *rmRt;
			rmRt = removedEventsRemote.erase(rmRt);
		} else {
			rmRt++;
		}
	}
	this->releaseRemovedLock(threadID);
}

void ThreadedOutputEvents::fossilCollect(int gCollectTime, int threadID) {
	// The events sent to another simulation manager need to be deleted here.
	// The events sent to this same simulation manager (local events) will be deleted
	// in the event set and should not be deleted here.

	this->getRemoteLock(threadID);
	vector<const Event*>::iterator outRem = outputEventsRemote.begin();
	while (outRem != outputEventsRemote.end()
			&& (*outRem)->getSendTime().getApproximateIntTime() < gCollectTime) {
		delete (*outRem);
		outRem++;
	}
	outputEventsRemote.erase(outputEventsRemote.begin(), outRem);
	this->releaseRemoteLock(threadID);

	this->getLocalLock(threadID);
	vector<SetObject<Event>*>::iterator outLoc = outputEventsLocal.begin();
	while (outLoc != outputEventsLocal.end()
			&& (*outLoc)->getMainTime().getApproximateIntTime() < gCollectTime) {
		delete *outLoc;
		outLoc++;
	}
	outputEventsLocal.erase(outputEventsLocal.begin(), outLoc);
	this->releaseLocalLock(threadID);

	this->getRemovedLock(threadID);
	list<const Event*>::iterator rmRt = removedEventsRemote.begin();
	while (rmRt != removedEventsRemote.end()) {
		if ((*rmRt)->getSendTime().getApproximateIntTime() < gCollectTime) {
			delete *rmRt;
			rmRt = removedEventsRemote.erase(rmRt);
		} else {
			rmRt++;
		}
	}
	this->releaseRemovedLock(threadID);
}

const Event*
ThreadedOutputEvents::getOldestEvent(unsigned int size, int threadID) {
	const Event *retval = NULL;
	this->getRemoteLock(threadID);
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
	this->releaseRemoteLock(threadID);

	return retval;
}

void ThreadedOutputEvents::insert(const Event *newEvent, int threadID) {
	bool isLocal = mySimulationManager->contains(newEvent->getReceiver());
	vector<const Event *>::iterator i;

	if (isLocal) {
		this->getLocalLock(threadID);
		outputEventsLocal.push_back(
				new SetObject<Event> (newEvent->getSendTime(),
						newEvent->getReceiveTime(),
						newEvent->getReceiver().getSimulationObjectID(),
						newEvent));
		this->releaseLocalLock(threadID);
	} else {
		this->getRemoteLock(threadID);
		list<const Event *>::iterator it;
		it = std::find(removedEventsRemote.begin(), removedEventsRemote.end(),
				newEvent);
		if (it != removedEventsRemote.end()) {
			removedEventsRemote.erase(it);
		}
		outputEventsRemote.push_back(newEvent);
		this->releaseRemoteLock(threadID);
	}
}

// Only used for memory management in optimistic fossil collection.
void ThreadedOutputEvents::fossilCollectEvent(const Event *toRemove, int threadID) {
	vector<SetObject<Event>*>::iterator sit;
	vector<const Event *>::iterator it;
	bool foundMatch = false;

	this->getLocalLock(threadID);
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
	this->releaseLocalLock(threadID);

	this->getRemoteLock(threadID);
	it = outputEventsRemote.begin();
	while (it != outputEventsRemote.end()) {
		if (toRemove->getEventId() == (*it)->getEventId()
				&& toRemove->getSender() == (*it)->getSender()) {
			outputEventsRemote.erase(it);
			foundMatch = true;
			break;
		} else {
			it++;
		}
	}
	this->releaseRemoteLock(threadID);
}

void ThreadedOutputEvents::remove(const Event *toRemove, int threadID) {
	vector<const Event *>::iterator i;
	this->getLocalLock(threadID);
	vector<SetObject<Event>*>::iterator sit = outputEventsLocal.begin();
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
	this->releaseLocalLock(threadID);

	this->getRemoteLock(threadID);
	if (!found) {
		i = std::find(outputEventsRemote.begin(), outputEventsRemote.end(),
				toRemove);

		ASSERT( i != outputEventsRemote.end() );
		outputEventsRemote.erase(i);
	}
	this->releaseRemoteLock(threadID);
}
//Dont know who calls this Function.. As of now its not thread safe
void ThreadedOutputEvents::remove(const vector<const Event *> &toRemove, int threadID) {
	for (vector<const Event *>::const_iterator i = toRemove.begin(); i
			!= toRemove.end(); i++) {
		remove(*i, threadID);
	}
}

void ThreadedOutputEvents::saveOutputCheckpoint(ofstream* outFile,
		unsigned int saveTime, int threadID) {
	this->getRemoteLock(threadID);
	vector<const Event*>::iterator outRem = outputEventsRemote.begin();
	char del = '_';
	unsigned int eveSize = 0;
	const char *charPtr = NULL;
	SerializedInstance *toWrite = NULL;

	while (outRem != outputEventsRemote.end()) {
		if ((*outRem)->getSendTime().getApproximateIntTime() < saveTime
				&& (*outRem)->getReceiveTime().getApproximateIntTime()
						>= saveTime) {
			toWrite = new SerializedInstance((*outRem)->getDataType());
			(*outRem)->serialize(toWrite);
			charPtr = &toWrite->getData()[0];
			eveSize = toWrite->getSize();

			outFile->write((char*) (&eveSize), sizeof(eveSize));
			outFile->write(&del, sizeof(del));
			outFile->write(charPtr, eveSize);
		}
		outRem++;
	}
	this->releaseRemoteLock(threadID);
	this->getLocalLock(threadID);
	vector<SetObject<Event>*>::iterator outLoc = outputEventsLocal.begin();
	while (outLoc != outputEventsLocal.end()) {
		if ((*outLoc)->getMainTime().getApproximateIntTime() < saveTime
				&& (*outLoc)->getSecondaryTime().getApproximateIntTime()
						>= saveTime) {
			int
					lastTime =
							mySimulationManager->getOptFossilCollManagerNew()->getLastCollectTime(
									(*outLoc)->getReceiverID());
			int recvTime =
					(*outLoc)->getSecondaryTime().getApproximateIntTime();
			if (recvTime <= lastTime) {
				utils::debug << mySimulationManager->getSimulationManagerID()
						<< " - Cata Rollback in ThreadedOutputEvents::checkpoint: "
						<< recvTime << ", " << lastTime << std::endl;
				mySimulationManager->getOptFossilCollManagerNew()->setRecovery(
						(*outLoc)->getReceiverID(),
						(*outLoc)->getMainTime().getApproximateIntTime());
				break;
			} else {

				toWrite = new SerializedInstance(
						(*outLoc)->getElement()->getDataType());
				(*outLoc)->getElement()->serialize(toWrite);
				charPtr = &toWrite->getData()[0];
				eveSize = toWrite->getSize();

				outFile->write((char*) (&eveSize), sizeof(eveSize));
				outFile->write(&del, sizeof(del));
				outFile->write(charPtr, eveSize);
				delete toWrite;
			}
		}
		outLoc++;
	}
	this->releaseLocalLock(threadID);
}

void ThreadedOutputEvents::ofcPurge(int threadID) {
	// Directly call the destructor and release the memory to avoid the
	// overloaded delete. Delete remote events and remove them from the queues.
	// Just clear the local events.

	this->getRemoteLock(threadID);
	vector<const Event*>::iterator outRem = outputEventsRemote.begin();
	while (outRem != outputEventsRemote.end()) {
		(*outRem)->~Event();
		::operator delete((void*) (*outRem));
		outRem++;
	}
	outputEventsRemote.clear();
	this->releaseRemoteLock(threadID);

	this->getLocalLock(threadID);
	vector<SetObject<Event>*>::iterator outLoc = outputEventsLocal.begin();
	while (outLoc != outputEventsLocal.end()) {
		delete *outLoc;
		outLoc++;
	}
	outputEventsLocal.clear();
	this->releaseLocalLock(threadID);

	this->getRemovedLock(threadID);
	list<const Event*>::iterator rmRt = removedEventsRemote.begin();
	while (rmRt != removedEventsRemote.end()) {
		delete *rmRt;
		rmRt = removedEventsRemote.erase(rmRt);
	}
	this->releaseRemovedLock(threadID);
}

bool ThreadedOutputEvents::getLocalLock(int threadId) {
	while (!localQueueLock->setLock(threadId, mySimulationManager->getSyncMechanism()))
		;
	ASSERT(localQueueLock->hasLock(threadId, mySimulationManager->getSyncMechanism()));
}

bool ThreadedOutputEvents::releaseLocalLock(int threadId) {
	ASSERT(localQueueLock->hasLock(threadId, mySimulationManager->getSyncMechanism()));
	localQueueLock->releaseLock(threadId, mySimulationManager->getSyncMechanism());
}

bool ThreadedOutputEvents::getRemoteLock(int threadId) {
	while (!remoteQueueLock->setLock(threadId, mySimulationManager->getSyncMechanism()))
		;
	ASSERT(remoteQueueLock->hasLock(threadId, mySimulationManager->getSyncMechanism()));
}

bool ThreadedOutputEvents::releaseRemoteLock(int threadId) {
	ASSERT(remoteQueueLock->hasLock(threadId, mySimulationManager->getSyncMechanism()));
	remoteQueueLock->releaseLock(threadId, mySimulationManager->getSyncMechanism());
}

bool ThreadedOutputEvents::getRemovedLock(int threadId) {
	while (!removedListLock->setLock(threadId, mySimulationManager->getSyncMechanism()))
		;
	ASSERT(removedListLock->hasLock(threadId, mySimulationManager->getSyncMechanism()));
}

bool ThreadedOutputEvents::releaseRemovedLock(int threadId) {
	ASSERT(removedListLock->hasLock(threadId, mySimulationManager->getSyncMechanism()));
	removedListLock->releaseLock(threadId, mySimulationManager->getSyncMechanism());
}
