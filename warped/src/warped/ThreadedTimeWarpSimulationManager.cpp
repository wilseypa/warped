// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpSimulationManager.h"
#include "DefaultSchedulingManager.h"
#include "ThreadedLazyOutputManager.h"
#include "ThreadedDynamicOutputManager.h"
#include "ThreadedAggressiveOutputManager.h"
#include "StopWatch.h"
#include "ObjectStub.h"
#include "SimulationObjectProxy.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSimulationStream.h"
#include "CommunicationManager.h"
#include "TerminationManager.h"
#include "EventMessage.h"
#include "NegativeEvent.h"
#include "NegativeEventMessage.h"
#include "SingleTerminationManager.h"
#include "ThreadedCostAdaptiveStateManager.h"
#include "ThreadedOptFossilCollManager.h"
#include "ThreadedTimeWarpMultiSet.h"
#include "InitializationMessage.h"
#include "StartMessage.h"
#include "Application.h"
#include "TokenPassingTerminationManager.h"
#include "OptFossilCollManagerFactory.h"
#include "ThreadedTimeWarpEventSet.h"
#include "ThreadedTimeWarpMultiSetSchedulingManager.h"
#include "PartitionInfo.h"
#include "StragglerEvent.h"
#include "ThreadedMatternGVTManager.h"
#include "SimulationConfiguration.h"

int WorkerInformation::globalStillBusyCount = 0;
bool WorkerInformation::workRemaining = true;

static pthread_key_t threadKey;

ThreadedTimeWarpSimulationManager::ThreadedTimeWarpSimulationManager(
		unsigned int numberOfWorkerThreads, const string syncMechanism, 
		const string scheduleQScheme, unsigned int scheduleQCount, 
		Application *initApplication) :
			numberOfWorkerThreads(numberOfWorkerThreads), syncMechanism(syncMechanism), 
			scheduleQScheme(scheduleQScheme), scheduleQCount(scheduleQCount), masterID(0),
			coastForwardTime(0), myrealFossilCollManager(0), myStateManager(0),
			messageBuffer(new LockedQueue<KernelMessage*> ),
			workerStatus(new WorkerInformation*[numberOfWorkerThreads + 1]),
			myOutputManager(0), mySchedulingManager(0), checkGVT(false),
			GVTTimePeriodLock(new AtomicState()),
			LVTFlag(0), LVTFlagLock(new AtomicState()),
			numberOfRemoteAntimessages(0), numberOfNegativeEventMessage(0),
			numberOfLocalAntimessages(0),
			computeLVTStatus(new bool*[numberOfWorkerThreads + 1]),
			rollbackCompleted(new bool[numberOfObjects]), inRecovery(false),
			GVTTokenPending(false), TimeWarpSimulationManager(initApplication) {
	LVT = &getZero();
	LVTArray = new const VTime *[numberOfWorkerThreads + 1];
	sendMinTimeArray = new const VTime *[numberOfWorkerThreads + 1];

	for (int i = 0; i < numberOfWorkerThreads + 1; i++) {
		computeLVTStatus[i] = new bool(0);
		*(computeLVTStatus[i]) = 1;
		sendMinTimeArray[i] = NULL;
	}

	//used 0, since manager object has been constructed using the master
	pthread_key_create(&threadKey, NULL);
	pthread_setspecific(threadKey, (void*) &masterID);
	initiatedRecovery = false;
	lvtCount = 0;
	numCatastrophicRollbacks = 0;
}

ThreadedTimeWarpSimulationManager::~ThreadedTimeWarpSimulationManager() {
	//fossilCollect(getPositiveInfinity());
	delete[] workerStatus;
	delete messageBuffer;
	delete myOutputManager;
	delete mySchedulingManager;
	delete GVTTimePeriodLock;
	delete myrealFossilCollManager;
}

inline void ThreadedTimeWarpSimulationManager::sendMessage(KernelMessage *msg,
		unsigned int destSimMgrId) {
	messageBuffer->enqueue(msg, syncMechanism);
}

inline void ThreadedTimeWarpSimulationManager::sendPendingMessages() {
	KernelMessage *messageToBeSent = NULL;
	if ((messageToBeSent = messageBuffer->peekNext(syncMechanism)) != NULL) {
		utils::debug << "(" << mySimulationManagerID
				<< " ) In Sending Module: " << endl;
	}
	while ((messageToBeSent = messageBuffer->dequeue(syncMechanism)) != NULL) {
		utils::debug << "(" << mySimulationManagerID << " ) Sending Message: "
				<< endl;
		myCommunicationManager->sendMessage(messageToBeSent,
				messageToBeSent->getReceiver());
	}
}

void ThreadedTimeWarpSimulationManager::createWorkerThreads() {
	//Add the simulation manager to the number of worker threads
	numberOfWorkerThreads++;
	//Let the manager thread be thradID 0
	workerStatus[0] = new WorkerInformation(NULL);
	//Temporary pointer used in pthread_create
	pthread_t *pthread = NULL;
	//Use the default attributes which are NULL;
	pthread_attr_t *attributes = NULL;
	//Each thread will get its own set of arguments defined in the loop below
	void *threadArguments = NULL;
	//pthread_create return value
	int retVal = 0;
	//Start Remaining Thread count at 1
	for (unsigned int threadIndex = 1; threadIndex < numberOfWorkerThreads; threadIndex++) {
		//Setting Key for Each thread
		//Allocate some memory for a new thread
		pthread = new pthread_t;
		//Define worker status for this thread number including the threadId and numerical threadNumber
		workerStatus[threadIndex] = new WorkerInformation(pthread);
		//Each workerThread gets a pointer to this simulation manager and the index to its status information
		threadArguments = new thread_args(this, threadIndex);
		//Create the new worker thread
		retVal = pthread_create(pthread, attributes, startWorkerThread,
				threadArguments);
		if (retVal != 0) {
			cout << "pthread_create failed with error code: " << retVal << endl;
			cout << "Aborting!!!" << endl;
			abort();
		}
	}
}

void *ThreadedTimeWarpSimulationManager::startWorkerThread(void *arguments) {
	//Convert the arguments from void* back to thread_args
	thread_args* myArgs = static_cast<thread_args*> (arguments);
	//Start executing objects
	myArgs->simManager->workerThread(myArgs->threadIndex);
}

bool ThreadedTimeWarpSimulationManager::executeObjects(
		const unsigned int &threadId) {
	bool iDidWork = false;
	if (!inRecovery) {
		const Event
				*nextEvent =
						(dynamic_cast<ThreadedTimeWarpMultiSetSchedulingManager *> (mySchedulingManager))->peekNextEvent(
								threadId);
		int objId;
		if (nextEvent != NULL && !inRecovery) {
			iDidWork = true;
			SimulationObject *nextObject = getObjectHandle(
					nextEvent->getReceiver());
			ASSERT(nextObject != 0);
			objId = nextObject->getObjectID()->getSimulationObjectID();

			if (usingOptFossilCollection && !inRecovery) {
				myrealFossilCollManager->checkpoint(
						nextEvent->getReceiveTime(),
						*nextObject->getObjectID(), threadId);
				myrealFossilCollManager->fossilCollect(nextObject,
						nextEvent->getReceiveTime(), threadId);
			}

			const Event* straggler = NULL;
			//updateLVTArray(threadId, objId);
			straggler
					= dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->getEventIfStraggler(
							nextObject, threadId);
			//Handle Straggler
			if (straggler != NULL && !inRecovery) {
				updateLVTArray(threadId, straggler->getReceiveTime().clone());
				if (!dynamic_cast<const StragglerEvent*> (straggler)) {
					utils::debug << "(" << mySimulationManagerID << " T "
							<< threadId << " )"
							<< "Processing  StragglerEvent for Object "
							<< nextObject->getName() << endl;

					rollback(nextObject, straggler->getReceiveTime(), threadId);
				} else if (dynamic_cast<const StragglerEvent*> (straggler)) {
					if (straggler->getReceiveTime()
							< nextObject->getSimulationTime()) {
						utils::debug << "(" << mySimulationManagerID << " T "
								<< threadId << " )"
								<< "Processing  Negative StragglerEvent for Object "
								<< nextObject->getObjectID()->getSimulationObjectID()
								<< endl;
						rollback(nextObject, straggler->getReceiveTime(),
								threadId);
					}
					if (!inRecovery) {
						utils::debug << "(" << mySimulationManagerID << " T "
								<< threadId << " )"
								<< "Handling Negative Message for the Object "
								<< nextObject->getName() << " is at : "
								<< nextObject->getSimulationTime() << endl;
						handleAntiMessageFromStraggler(straggler, threadId);
					}
				}
				//delete straggler;
			} else if (!inRecovery) {
				ASSERT( dynamic_cast<SimulationObjectProxy *>( nextObject ) == 0);
				//	updateLVTArray(threadId, objId);
				nextEvent
						= dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->peekEventLockUnprocessed(
								nextObject, threadId);
				if (outMgrType == LAZYMGR) {
					ThreadedLazyOutputManager
							*myLazyOutputManager =
									dynamic_cast<ThreadedLazyOutputManager *> (myOutputManager);
					ASSERT(myLazyOutputManager != NULL);
					if (nextEvent != NULL) {
						myLazyOutputManager->emptyLazyQueue(nextObject,
								nextEvent->getReceiveTime(), threadId);
					}
				} else if (outMgrType == ADAPTIVEMGR) {
					ThreadedDynamicOutputManager
							*myDynamicOutputManager =
									static_cast<ThreadedDynamicOutputManager *> (myOutputManager);
					ASSERT(myDynamicOutputManager != NULL);
					if (nextEvent != NULL)
						myDynamicOutputManager->emptyLazyQueue(nextObject,
								nextEvent->getReceiveTime(), threadId);
				}
				if (nextEvent != NULL) {
					nextObject->setSimulationTime(nextEvent->getReceiveTime());
					utils::debug << nextObject->getName()
							<< " Executing Event of Time :: "
							<< nextEvent->getReceiveTime() << " - " << threadId
							<< endl;
					EventId tempEventId = nextEvent->getEventId();
					unsigned int eventNumber = tempEventId.getEventNum();
					myStateManager->saveState(nextEvent->getReceiveTime(),
							eventNumber, nextObject, nextEvent->getSender(),
							threadId);
					updateLVTArray(threadId, objId);
					nextObject->executeProcess();
					// Only the master Simulation manager starts the GVT calculation process.
					// Fossil collection occurs when a new GVT value is set.
					if (mySimulationManagerID == 0 && !checkGVT) {
						this->getGVTTimePeriodLock(threadId);
						if (myGVTManager->checkGVTPeriod()) {
							//Set the GVT flag so the Manager thread can calculate GVT
							bool checkGVTOn = __sync_bool_compare_and_swap(
									&checkGVT, false, true);
							ASSERT(checkGVTOn);
						}
						this->releaseGVTTimePeriodLock(threadId);
					}
				}
			}
			//nextObject->finalize();
			if (!inRecovery)
				(dynamic_cast<ThreadedTimeWarpMultiSet *> (myEventSet))->updateScheduleQueueAfterExecute(
						objId, threadId);

		}
	}
	return iDidWork;
}

void ThreadedTimeWarpSimulationManager::workerThread(
		const unsigned int &threadId) {
	unsigned int tid = threadId;
	pthread_setspecific(threadKey, (void*) &tid);
	while (WorkerInformation::isWorkRemaining()) {
		//If no work was done and we had access to all of the queues
		//increment the idle count
		if (executeObjects(threadId)) {
			workerStatus[threadId]->resetIdleCount();
		} else {
			workerStatus[threadId]->incrementIdleCount();
			pthread_yield();
		}
	}
	pthread_exit(0);
}
void ThreadedTimeWarpSimulationManager::simulate(const VTime& simulateUntil) {
	cout << "SimulationManager(" << mySimulationManagerID
			<< "): Starting simulation - End time: " << simulateUntil << ")"
			<< endl;
	bool LTSFDestTemp = 1;
	printObjectMaaping();
	//Do ASSERT for all components of the kernel
	bool pastSimulationCompleteTime = false; // Use it to terminate
	getMessages();
	createWorkerThreads();
	StopWatch stopwatch;
	stopwatch.start();
	unsigned int threadID = *((unsigned int*) pthread_getspecific(threadKey));
	while (!simulationComplete(simulateUntil) || inRecovery) {
		if (inRecovery) {
			numCatastrophicRollbacks++;
			if (numberOfSimulationManagers > 1) {
				while (workerStatus[0]->getStillBusyCount() > 0)
					utils::debug << workerStatus[0]->getStillBusyCount()
							<< endl;
				if (initiatedRecovery) {
					myrealFossilCollManager->startRecovery();
					initiatedRecovery = false;
				}
				while (inRecovery)
					getMessages();
			} else {
				myrealFossilCollManager->purgeQueuesAndRecover();
			}
		}

		getMessages();
		//Calculate GVT
		if (!usingOptFossilCollection) {
			if (/*!(myGVTManager->getGVTTokenStatus()) &&*/checkGVT
					&& mySimulationManagerID == 0) {
				if (!GVTTokenPending) {
					initiateLocalGVT();
					setGVTTokenPending();
				}
				if (GVTTokenPending) {
					if (updateLVTfromArray()) {
						myGVTManager->calculateGVT();
						//myEventSet->moveLP(2,(int)LTSFDestTemp);
						//myEventSet->moveLP(4,(int)LTSFDestTemp);
						//myEventSet->moveLP(6,(int)LTSFDestTemp);
						//LTSFDestTemp = !LTSFDestTemp;
						//Reset the GVT flag so the Worker thread can increase GVT Period
						bool checkGVTOn = __sync_bool_compare_and_swap(
								&checkGVT, true, false);
						resetGVTTokenPending();
						if (myGVTManager->getGVT() >= simulateUntil) {
							pastSimulationCompleteTime = true;
						}
					}
				}
			}
			if (!checkGVT && GVTTokenPending && numberOfSimulationManagers > 1) {
				if (updateLVTfromArray()) {
					dynamic_cast<ThreadedMatternGVTManager*> (myGVTManager)->sendPendingGVTToken();
					resetGVTTokenPending();
				}
			}
		}
		//Clear message Buffer
		sendPendingMessages();
		// numberOfWorkerThreads includes manager thread for some reason...
		if (WorkerInformation::getStillBusyCount() < numberOfWorkerThreads - 1) { // Check if any workers are still sleeping
			//cout << WorkerInformation::getStillBusyCount() << "/" << numberOfWorkerThreads << endl;
			for (unsigned int ltsfIndex = 0; ltsfIndex < scheduleQCount; ltsfIndex++) {
				if (!myEventSet->isScheduleQueueEmpty(ltsfIndex)) { // Check if there are items in the scheduleQueue
					//cout << "Resuming threads attached to ltsf " << ltsfIndex << endl;
					for(unsigned int threadIndex = ltsfIndex; threadIndex < numberOfWorkerThreads - 1; threadIndex = threadIndex + scheduleQCount) {
						workerStatus[threadIndex+1]->resume();
					}
				}
			}
		}

		// Set Termination Manager according to busycount
		if (WorkerInformation::getStillBusyCount() > 0) {
			myTerminationManager->setStatusActive();
		} else if (numberOfSimulationManagers == 1) {
			//simulation complete only after cleaning all output queue
			if (checkTermination())
				dynamic_cast<SingleTerminationManager*> (myTerminationManager)->simulationComplete();
		} else {
			myTerminationManager->setStatusPassive();
		}
	}
	sendPendingMessages();
	stopwatch.stop();
	ostringstream oss;
	oss << "lp" << mySimulationManagerID << ".csv";
	ofstream file(oss.str().c_str(), ios_base::app);
	if (file)
		file << stopwatch.elapsed() << ',' << numberOfRollbacks << ',' << 0
				<< ',' << 1 << endl;

	//	cout << "After Simulation :: Event Count in Unprocessed Queue is = "
	//			<< dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->getMessageCount(
	//					0) << endl;

	cout << "(" << getSimulationManagerID()
			<< ") Number of Local AntiMessages sent "
			<< numberOfLocalAntimessages << endl;

	cout << "(" << getSimulationManagerID()
			<< ") Number of Remote AntiMessages sent "
			<< numberOfRemoteAntimessages << endl;

	cout << "(" << getSimulationManagerID()
			<< ") Number of Remote Negative Event Messages sent "
			<< numberOfNegativeEventMessage << endl;

	//kill all Workers
	WorkerInformation::killWorkerThreads();
	//join Worker threads
	for (unsigned int threadIndex = 1; threadIndex < numberOfWorkerThreads; threadIndex++) {
		//Verify the thread is not suspended
		workerStatus[threadIndex]->resume();
		//Rejoin with the worker upon its exit
		pthread_join(*(workerStatus[threadIndex])->getThread(), NULL);
	}
	if (myGVTManager->getGVT() >= simulateUntil) {
	}
	cout << "(" << getSimulationManagerID() << ") Simulation complete ("
			<< stopwatch.elapsed() << " secs), Number of Rollbacks: ("
			<< numberOfRollbacks << ")" << endl;
	cout << "Number of catastrophic rollbacks: " << numCatastrophicRollbacks
			<< endl;

}
const Event *
ThreadedTimeWarpSimulationManager::getEvent(SimulationObject *object) {
	ASSERT(myEventSet != NULL);
	unsigned int threadID = *((unsigned int*) pthread_getspecific(threadKey));
	const Event *retval;
	int id = object->getObjectID()->getSimulationObjectID();

	if (getCoastForwardTime(id) != 0) {
		retval = myEventSet->getEvent(object, *getCoastForwardTime(id),
				threadID);
	} else {

		retval = myEventSet->getEvent(object, threadID);
	}
	if (dynamic_cast<const StragglerEvent*> (retval)) {
		ASSERT(false);
	}

	return retval;
}

const Event *
ThreadedTimeWarpSimulationManager::peekEvent(SimulationObject *object) {
	ASSERT(myEventSet != NULL);
	unsigned int threadID = *((unsigned int*) pthread_getspecific(threadKey));
	const Event *retval;
	int id = object->getObjectID()->getSimulationObjectID();

	if (getCoastForwardTime(id) != 0) {
		retval = myEventSet->peekEvent(object, *getCoastForwardTime(id),
				threadID);
	} else {
		retval = myEventSet->peekEvent(object, threadID);
	}

	if (retval != NULL)
		if (dynamic_cast<const StragglerEvent*> (retval)) {
			retval = NULL;
		}
	return retval;
}

void ThreadedTimeWarpSimulationManager::handleEvent(const Event *event) {
	ASSERT(myOutputManager != NULL);
	// first set the send time of the event
	ASSERT(event != NULL);
	unsigned int threadID = *((unsigned int*) pthread_getspecific(threadKey));
	bool shouldHandleEvent = true;
	int id = event->getSender().getSimulationObjectID();

	if (getCoastForwardTime(id) != NULL || (inRecovery && threadID != 0)) {
		shouldHandleEvent = false;
		delete event;
		event = NULL;
	} else {
		if (contains(event->getSender())) {
			if (outMgrType == AGGRMGR) {
				myOutputManager->insert(event, threadID);
			} else if (outMgrType == LAZYMGR) {
				ThreadedLazyOutputManager
						*myLazyOutputManager =
								static_cast<ThreadedLazyOutputManager *> (myOutputManager);
				ASSERT(myLazyOutputManager != NULL);
				shouldHandleEvent = !myLazyOutputManager->lazyCancel(event,
						threadID);
			} else if (outMgrType == ADAPTIVEMGR) {
				ThreadedDynamicOutputManager
						*myDynamicOutputManager =
								static_cast<ThreadedDynamicOutputManager *> (myOutputManager);
				ASSERT(myDynamicOutputManager != NULL);
				shouldHandleEvent
						= !myDynamicOutputManager->checkDynamicCancel(event,
								threadID);
			}
		}
	}

	if (shouldHandleEvent) {
		if (!usingOptFossilCollection)
			updateSendMinTime(threadID, &(event->getReceiveTime()));
		handleEventReceiver(getObjectHandle(event->getReceiver()), event,
				threadID);
	}
}

void ThreadedTimeWarpSimulationManager::handleEventReceiver(
		SimulationObject *currObject, const Event *event, int threadID) {
	if (contains(event->getReceiver()) == true) {
		handleLocalEvent(event, threadID);
	} else {
		handleRemoteEvent(event, threadID);
	}
}

void ThreadedTimeWarpSimulationManager::handleLocalEvent(const Event *event,
		int threadID) {
	SimulationObject *receiver = getObjectHandle(event->getReceiver());
	unsigned int objId = receiver->getObjectID()->getSimulationObjectID();
	myEventSet->insert(event, threadID);

}
void ThreadedTimeWarpSimulationManager::handleRemoteEvent(const Event *event,
		int threadID) {
	int
			id =
					getObjectHandle(event->getSender())->getObjectID()->getSimulationObjectID();
	if (getCoastForwardTime(id) == 0) {
		SimulationObjectProxy *proxyObj;
		proxyObj = dynamic_cast<SimulationObjectProxy *> (getObjectHandle(
				event->getReceiver()));
		ASSERT( proxyObj != 0);
		unsigned int destSimMgrId = proxyObj->getDestId();
		const string gVTInfo = myGVTManager->getGVTInfo(
				getSimulationManagerID(), destSimMgrId, event->getSendTime());
		utils::debug << "(" << mySimulationManagerID << " T " << threadID
				<< " )" << "Creating a Kernel Message for " << destSimMgrId
				<< endl;
		KernelMessage *msg = new EventMessage(getSimulationManagerID(),
				destSimMgrId, event, gVTInfo);
		sendMessage(msg, destSimMgrId);
	}
}

void ThreadedTimeWarpSimulationManager::cancelLocalEvents(
		const vector<const NegativeEvent *> &eventsToCancel, int threadID) {
	const NegativeEvent *curEvent = NULL;
	const VTime *lowTime = &(eventsToCancel[0]->getReceiveTime());

	//Find the lowest receive time to determine if rollback needs to occur.
	for (vector<const NegativeEvent*>::const_iterator it =
			eventsToCancel.begin(); it != eventsToCancel.end(); it++) {
		curEvent = *it;
		if (*lowTime > curEvent->getReceiveTime()) {
			//delete lowTime;
			lowTime = &(curEvent->getReceiveTime());
		}
	}
	__sync_fetch_and_add(&numberOfLocalAntimessages, eventsToCancel.size());
	SimulationObject *receiver = getObjectHandle(
			eventsToCancel[0]->getReceiver());
	unsigned int objId = receiver->getObjectID()->getSimulationObjectID();
	ASSERT( receiver != 0);
	const ObjectID senderZero = (eventsToCancel[0])->getSender();
	const ObjectID receiverZero = (eventsToCancel[0])->getReceiver();
	const EventId eventIdZero = (eventsToCancel[0])->getEventId();
	const VTime* sendTimeZero = &((eventsToCancel[0])->getSendTime());
	const NegativeEvent* copiedEvent = new NegativeEvent(*sendTimeZero,
			*lowTime->clone(), senderZero, receiverZero, eventIdZero);
	const StragglerEvent* stragEvent = new StragglerEvent(copiedEvent, 0,
			eventsToCancel);
	utils::debug << "(" << mySimulationManagerID << " T " << threadID << " )"
			<< "Inserting only a Negative Event of Time : " << *lowTime
			<< " for object " << objId << " at Time : " << endl;
	updateSendMinTime(threadID, &(stragEvent->getReceiveTime()));
	myEventSet->insert(stragEvent, threadID);
}
void ThreadedTimeWarpSimulationManager::cancelRemoteEvents(
		const vector<const NegativeEvent *> &eventsToCancel, int threadID) {
	ASSERT( eventsToCancel.size() > 0);
	const ObjectID &receiverId = eventsToCancel[0]->getReceiver();
	const SimulationObject *forObject = getObjectHandle(receiverId);
	unsigned int destId = forObject->getObjectID()->getSimulationManagerID();

	utils::debug << "(" << mySimulationManagerID << " T " << threadID
			<< " ) - Sending " << eventsToCancel.size()
			<< " anti-messages for " << receiverId << endl;

	vector<const NegativeEvent *>::const_iterator cur = eventsToCancel.begin();
	const VTime *min = NULL;

	// The negative events are aggregated together and sent in a negative event
	// message. However, if too many are aggregated and TCP is being used as
	// the communication manager, then the buffer will be exceeded. To prevent
	// this, the number of negative messages aggregated is limited.
	if (eventsToCancel.size() > 10) {
		vector<const NegativeEvent *>::const_iterator start;
		vector<const NegativeEvent *>::const_iterator end =
				eventsToCancel.end();
		while (cur != end) {
			start = cur;
			int s = 0;
			min = &((*start)->getSendTime());

			while (s < 10 && cur != end) {
				if (*min > (*cur)->getSendTime()) {
					min = &((*cur)->getSendTime());
				}
				cur++;
				s++;
			}
			const string gVTInfo = myGVTManager->getGVTInfo(
					getSimulationManagerID(), destId, *min);

			vector<const NegativeEvent*> partToCancel(start, cur);
			NegativeEventMessage *newMessage = new NegativeEventMessage(
					getSimulationManagerID(), destId, partToCancel, gVTInfo);
			__sync_fetch_and_add(&numberOfNegativeEventMessage, 1);
			__sync_fetch_and_add(&numberOfRemoteAntimessages,
					partToCancel.size());
			sendMessage(newMessage, destId);
		}
	} else {
		min = &((*cur)->getSendTime());
		while (cur != eventsToCancel.end()) {
			if (*min > (*cur)->getSendTime()) {
				min = &((*cur)->getSendTime());
			}
			cur++;
		}
		const string gVTInfo = myGVTManager->getGVTInfo(
				getSimulationManagerID(), destId, *min);
		NegativeEventMessage *newMessage = new NegativeEventMessage(
				getSimulationManagerID(), destId, eventsToCancel, gVTInfo);
		__sync_fetch_and_add(&numberOfNegativeEventMessage, 1);
		__sync_fetch_and_add(&numberOfRemoteAntimessages, eventsToCancel.size());
		sendMessage(newMessage, destId);
	}
	/*
	 // The negative events are no longer needed so reclaim them now.
	 for( int i = 0; i < eventsToCancel.size(); i++){
	 delete eventsToCancel[i];*/
}

void ThreadedTimeWarpSimulationManager::handleNegativeEvents(
		const vector<const Event*> &negativeEvents, int threadID) {
	cancelEvents(negativeEvents);
}

void ThreadedTimeWarpSimulationManager::cancelEvents(
		const vector<const Event *> &eventsToCancel) {
	ASSERT( eventsToCancel.size() > 0);
	unsigned int threadID = *((unsigned int*) pthread_getspecific(threadKey));

	/* Go through the eventsToCancel list.
	 Check the event's object's simulation manager id.
	 Every time there is a new object, add it to a map and add the event to list for that object.
	 Then go through the map, determining whether to cancelLocal or cancelRemote.
	 */
	unordered_map<SimulationObject *, vector<const NegativeEvent *> >
			cancelEventObjects;
	unordered_map<SimulationObject *, vector<const NegativeEvent *> >::iterator
			cancelObjectIt;

	for (vector<const Event *>::const_iterator cancelEventIt =
			eventsToCancel.begin(); cancelEventIt != eventsToCancel.end(); cancelEventIt++) {
		cancelObjectIt = cancelEventObjects.find(
				getObjectHandle((*cancelEventIt)->getReceiver()));
		if (cancelObjectIt != cancelEventObjects.end()) {
			(*cancelObjectIt).second.push_back(
					new NegativeEvent((*cancelEventIt)->getSendTime(),
							(*cancelEventIt)->getReceiveTime(),
							(*cancelEventIt)->getSender(),
							(*cancelEventIt)->getReceiver(),
							(*cancelEventIt)->getEventId()));
		} else {
			vector<const NegativeEvent *> evntVec;
			evntVec.push_back(
					new NegativeEvent((*cancelEventIt)->getSendTime(),
							(*cancelEventIt)->getReceiveTime(),
							(*cancelEventIt)->getSender(),
							(*cancelEventIt)->getReceiver(),
							(*cancelEventIt)->getEventId()));
			cancelEventObjects.insert(
					std::make_pair(
							getObjectHandle((*cancelEventIt)->getReceiver()),
							evntVec));
		}
	}
	cancelObjectIt = cancelEventObjects.begin();
	while (cancelObjectIt != cancelEventObjects.end()) {
		SimulationObject *curObject = cancelObjectIt->first;
		cancelEventsReceiver(curObject, cancelObjectIt->second, threadID);
		cancelObjectIt++;
	}
}

void ThreadedTimeWarpSimulationManager::cancelEventsReceiver(
		SimulationObject *curObject,
		vector<const NegativeEvent *> &cancelObjects, int threadID) {
	if (contains(curObject->getName())) {
		cancelLocalEvents(cancelObjects, threadID);
	} else {
		cancelRemoteEvents(cancelObjects, threadID);
	}
}
void ThreadedTimeWarpSimulationManager::rollback(SimulationObject *object,
		const VTime &rollbackTime, int threadID) {
	ASSERT( object != 0);
	ASSERT( dynamic_cast<SimulationObjectProxy *>(object) == 0);
	utils::debug << "(" << mySimulationManagerID << " T " << threadID << " )"
			<< object->getName() << " rollback from "
			<< object->getSimulationTime() << " to " << rollbackTime << endl;

	if (rollbackTime < myGVTManager->getGVT()) {
		cerr << object->getName() << " Rollback beyond the Global Virtual Time"
				<< endl;
		cerr << "rollbackTime = " << rollbackTime << " GVT = "
				<< myGVTManager->getGVT() << endl;
		abort();
	}
	//Can be removed if accurate rollback count is not needed.
	__sync_fetch_and_add(&numberOfRollbacks, 1);

	unsigned int objId = object->getObjectID()->getSimulationObjectID();
	const VTime *restoredTime = &myStateManager->restoreState(rollbackTime,
			object, threadID);

	if (usingOptFossilCollection) {
		if (!inRecovery) {
			myrealFossilCollManager->sampleRollback(object, *restoredTime);
			myrealFossilCollManager->updateCheckpointTime(objId,
					rollbackTime.getApproximateIntTime());
			if (*restoredTime > rollbackTime) {

				// Only start recovery if this is an actual fault, otherwise continue normal rollback.
				if (myrealFossilCollManager->checkFault(object)) {
					utils::debug << mySimulationManagerID
							<< " - Catastrophic Rollback: Restored State Time: "
							<< *restoredTime << ", Rollback Time: "
							<< rollbackTime << ", Starting Recovery." << endl;

					myrealFossilCollManager->setRecovery(objId,
							rollbackTime.getApproximateIntTime());
				} else {
					restoredTime = &getZero();
				}
			}
		}
	}

	if (!inRecovery) {
		object->setSimulationTime(*restoredTime);

		utils::debug << "(" << mySimulationManagerID << " T " << threadID
				<< " )" << "object "
				<< object->getObjectID()->getSimulationObjectID()
				<< " rolled state back to time: " << *restoredTime << endl;

		ASSERT( *restoredTime <= rollbackTime);

		// now rollback the file queues
		for (vector<TimeWarpSimulationStream*>::iterator i =
				inFileQueues[objId].begin(); i != inFileQueues[objId].end(); i++) {
			(*i)->rollbackTo(*restoredTime);
		}

		for (vector<TimeWarpSimulationStream*>::iterator i =
				outFileQueues[objId].begin(); i != outFileQueues[objId].end(); i++) {
			(*i)->rollbackTo(*restoredTime);
		}

		// send out the cancel messages
		myEventSet->rollback(object, *restoredTime, threadID);
		myOutputManager->rollback(object, rollbackTime, threadID);

		setRollbackCompletedStatus(objId);
		coastForward(*restoredTime, rollbackTime, object, threadID);
	}
	dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->releaseunProcessedLock(
			threadID, objId);
}

void ThreadedTimeWarpSimulationManager::coastForward(
		const VTime &coastForwardFromTime, const VTime &moveUpToTime,
		SimulationObject *object, int threadID) {
	ASSERT( moveUpToTime >= coastForwardFromTime);
	utils::debug << "(" << mySimulationManagerID << " T " << threadID << " ) "
			<< "object " << object->getObjectID()->getSimulationObjectID()
			<< " coasting forward from " << coastForwardFromTime << " to "
			<< moveUpToTime << endl;

	int objId = object->getObjectID()->getSimulationObjectID();

	setCoastForwardTime(objId, moveUpToTime);
	StopWatch stopWatch;
	stopWatch.start();

	// go to the first event to coastforward from and call the object's
	// execute process until the rollbackToTime is reached.
	const Event *findEvent =
			dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->peekEvent(
					object, moveUpToTime, threadID);
	while (findEvent != 0 && findEvent->getReceiveTime() < moveUpToTime) {
		utils::debug << "(" << mySimulationManagerID << " T " << threadID
				<< " ) - coasting forward, skipping " << "event "
				<< findEvent->getEventId() << " at time << "
				<< findEvent->getReceiveTime() << endl;
		object->setSimulationTime(findEvent->getReceiveTime());
		EventId tempEventId = findEvent->getEventId();
		unsigned int eventNumber = tempEventId.getEventNum();

		myStateManager->updateStateWhileCoastForward(
				findEvent->getReceiveTime(), eventNumber, object,
				findEvent->getSender(), threadID);
		object->executeProcess();
		findEvent
				= dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->peekEvent(
						object, moveUpToTime, threadID);
	}

	stopWatch.stop();

	if (stateMgrType == ADAPTIVESTATE) {
		ThreadedCostAdaptiveStateManager
				*CAStateManager =
						static_cast<ThreadedCostAdaptiveStateManager *> (myStateManager);
		CAStateManager->coastForwardTiming(objId, stopWatch.elapsed());
	}

	utils::debug << "(" << mySimulationManagerID << " T " << threadID
			<< " ) - Finished Coasting Forward for: " << "object "
			<< object->getObjectID()->getSimulationObjectID() << endl;

	clearCoastForwardTime(objId);
}
void ThreadedTimeWarpSimulationManager::receiveKernelMessage(KernelMessage *msg) {
	ASSERT(msg != NULL);
	int threadID = 0;
	myTerminationManager->setStatusActive();

	if (dynamic_cast<EventMessage *> (msg) != 0) {
		EventMessage *eventMsg = dynamic_cast<EventMessage *> (msg);
		const Event *event = eventMsg->getEvent();
		utils::debug << "(" << mySimulationManagerID << " T " << threadID
				<< " ) Received a Normal Event :: " << event->getReceiveTime()
				<< endl;
		// have to take care of some gvt specific actions here
		if (!usingOptFossilCollection)
			myGVTManager->updateEventRecord(eventMsg->getGVTInfo(),
					mySimulationManagerID);
		handleEventReceiver(getObjectHandle(event->getReceiver()), event,
				threadID);
	} else if (dynamic_cast<NegativeEventMessage *> (msg)) {
		NegativeEventMessage *negEventMsg =
				dynamic_cast<NegativeEventMessage *> (msg);
		utils::debug << "(" << mySimulationManagerID << " T " << threadID
				<< " ) Received a Negative Event: " << endl;
		vector<const NegativeEvent*> eventsToCancel = negEventMsg->getEvents();
		if (!usingOptFossilCollection)
			myGVTManager->updateEventRecord(negEventMsg->getGVTInfo(),
					mySimulationManagerID);

		const ObjectID senderZero = (eventsToCancel[0])->getSender();
		const ObjectID receiverZero = (eventsToCancel[0])->getReceiver();
		const EventId eventIdZero = (eventsToCancel[0])->getEventId();
		const VTime* sendTimeZero = (eventsToCancel[0])->getSendTime().clone();
		const VTime* receiveTimeZero =
				(eventsToCancel[0])->getReceiveTime().clone();
		const NegativeEvent* copiedEvent = new NegativeEvent(*sendTimeZero,
				*receiveTimeZero, senderZero, receiverZero, eventIdZero);
		const StragglerEvent* stragEvent = new StragglerEvent(copiedEvent, 0,
				eventsToCancel);

	} else if (dynamic_cast<InitializationMessage *> (msg) != 0) {
		InitializationMessage *initMsg =
				dynamic_cast<InitializationMessage *> (msg);
		utils::debug << "(" << mySimulationManagerID << " T " << threadID
				<< " ) Received a Initi Event: " << endl;
		registerSimulationObjectProxies(&initMsg->getObjectNames(),
				initMsg->getReceiver(), initMsg->getSender());
	} else if (dynamic_cast<StartMessage *> (msg) != 0) {
		utils::debug << "(" << mySimulationManagerID << " T " << threadID
				<< " ): Starting Simulation" << endl;
	} else if (msg->getDataType() == "AbortSimulationMessage") {
		cerr
				<< "ThreadedTimeWarpSimulationManager is going to abort simulation"
				<< endl;
	} else {
		cerr
				<< "ThreadedTimeWarpSimulationManager::receiveKernelMessage() received"
				<< " unknown (" << msg->getDataType() << ") message type"
				<< endl;
		cerr << "Aborting simulation ..." << endl;
		abort();
	}
	// we are done with this kernel message
	delete msg;
}

void ThreadedTimeWarpSimulationManager::fossilCollect(
		const VTime& fossilCollectTime) {
  
	ASSERT( localArrayOfSimObjPtrs != 0);
	//Hard Coded ZERO, since this function is always called by the Master
	int threadID = 0;
	//Obtains all the objects from localArrayOfSimObjPtrs
	vector<SimulationObject *> *objects = getElementVector(
			localArrayOfSimObjPtrs);

	//If the number of LP's > number of Objects
	//Then its possible this simulation has no objects assigned to it
	if (!objects->empty()) {
		if (!usingOptFossilCollection) {
			//Used to determine the fossil collection times for the objects.
			vector<const VTime *> minTimes(objects->size(), NULL);
			for (unsigned int i = 0; i < objects->size(); i++) {
				minTimes[i] = &(myStateManager->fossilCollect((*objects)[i],
						fossilCollectTime, threadID));
				// call fossil collect on the file queues
				unsigned int objID =
						(*objects)[i]->getObjectID()->getSimulationObjectID();

				if (!inFileQueues[objID].empty()) {
					vector<TimeWarpSimulationStream*>::iterator iter =
							inFileQueues[objID].begin();
					vector<TimeWarpSimulationStream*>::iterator iter_end =
							inFileQueues[objID].end();
					while (iter != iter_end) {
						(*iter)->fossilCollect(*(minTimes[i]));
						++iter;
					}
				}

				if (!outFileQueues[objID].empty()) {
					vector<TimeWarpSimulationStream*>::iterator iter =
							outFileQueues[objID].begin();
					vector<TimeWarpSimulationStream*>::iterator iter_end =
							outFileQueues[objID].end();
					while (iter != iter_end) {
						(*iter)->fossilCollect(*(minTimes[i]));
						++iter;
					}
				}
			}

			const VTime *min = minTimes[0];
			for (unsigned int t = 1; t < minTimes.size(); t++) {
				if (*(minTimes[t]) < *min) {
					min = minTimes[t];
				}
			}

			// Events are only reclaimed from the event set. Event set fossil collection
			// needs to occur after all other fossil collection to prevent accesses to
			// already reclaimed events (specifically, to prevent accessing events in
			// the output queue that have already been reclaimed).
			for (unsigned int j = 0; j < objects->size(); j++) {
				myOutputManager->fossilCollect((*objects)[j], *min, threadID);
			}

			for (unsigned int j = 0; j < objects->size(); j++) {
				myEventSet->fossilCollect((*objects)[j], *min, threadID);
			}
		}
		delete objects;
	}
}
void ThreadedTimeWarpSimulationManager::initialize() {
	cout << "SimulationManager(" << mySimulationManagerID
			<< "): Initializing Objects" << endl;

	//Obtains all the objects from localArrayOfSimObjPtrs
	vector<SimulationObject *> *objects = getElementVector(
			localArrayOfSimObjPtrs);

	for (unsigned int i = 0; i < objects->size(); i++) {
		// call initialize on the object
		(*objects)[i]->initialize();
		// save the initial state in the state queue
		ASSERT(myStateManager != NULL);
		myStateManager->saveState(myApplication->getZero(), (*objects)[i], 0); //Zero is the thread id for manager
	}
	delete objects;

	if (usingOptFossilCollection) {
		myrealFossilCollManager->makeInitialCheckpoint();
	}

	if (numberOfSimulationManagers > 1) {
		if (mySimulationManagerID == 0) {
			// send out simulation start messages
			myCommunicationManager->sendStartMessage(mySimulationManagerID);
		} else {
			// else wait for the start message
			myCommunicationManager->waitForStart();
		}
	}
}

void ThreadedTimeWarpSimulationManager::configure(
		SimulationConfiguration &configuration) {
	const CommunicationManagerFactory *myCommFactory =
			CommunicationManagerFactory::instance();

	myCommunicationManager
			= dynamic_cast<CommunicationManager *> (myCommFactory->allocate(
					configuration, this));
	ASSERT( myCommunicationManager != 0);
	// update the number of the simulation managers with the number
	// provided by the physical communication layer
	numberOfSimulationManagers = myCommunicationManager->getSize();

	myCommunicationManager->configure(configuration);
	mySimulationManagerID = myCommunicationManager->getId();

	ASSERT( myApplication != 0);
	//setNumberOfObjects( myApplication->getNumberOfSimulationObjects(mySimulationManagerID) );

	// register simulation manager's message types with the comm. manager
	registerWithCommunicationManager();

	/*
	 Currently the only termination manager available is the
	 TokenPassingTerminationManager.
	 */
	if (getNumberOfSimulationManagers() > 1) {
		myTerminationManager = new TokenPassingTerminationManager(this);
	} else {
		myTerminationManager = new SingleTerminationManager(this);
	}
	// the createMapOfObjects function does the following:
	// a. registers the correct set of objects for this simulation
	//    manager
	// b. resets the value of numberOfObjects to the number of objects
	//    actually resident on this simulation manager.
	localArrayOfSimObjPtrs = createMapOfObjects();

	// configure the event set manager
	const TimeWarpEventSetFactory *myEventSetFactory =
			TimeWarpEventSetFactory::instance();
	Configurable *eventSet = myEventSetFactory->allocate(configuration, this);
	myEventSet = dynamic_cast<ThreadedTimeWarpEventSet *> (eventSet);
	ASSERT( myEventSet != 0);
	myEventSet->configure(configuration);

	// lets now set up and configure the state manager
	const StateManagerFactory *myStateFactory = StateManagerFactory::instance();
	myStateManager
			= dynamic_cast<ThreadedStateManager *> (myStateFactory->allocate(
					configuration, this));
	ASSERT( myStateManager != 0);
	myStateManager->configure(configuration);

	// lets now set up and configure the scheduler
	const SchedulingManagerFactory *mySchedulingFactory =
			SchedulingManagerFactory::instance();
	mySchedulingManager
			= dynamic_cast<SchedulingManager *> (mySchedulingFactory->allocate(
					configuration, this));
	ASSERT( mySchedulingManager != 0);
	mySchedulingManager->configure(configuration);

	// lets now set up and configure the output manager
	const OutputManagerFactory *myOutputManagerFactory =
			OutputManagerFactory::instance();
	myOutputManager
			= dynamic_cast<ThreadedOutputManager *> (myOutputManagerFactory->allocate(
					configuration, this));
	ASSERT( myOutputManager != 0);
	myOutputManager->configure(configuration);

	// lets now set up and configure the gvt manager
	const GVTManagerFactory *myGVTFactory = GVTManagerFactory::instance();
	myGVTManager = dynamic_cast<GVTManager *> (myGVTFactory->allocate(
			configuration, this));
	ASSERT( myGVTFactory != 0);
	myGVTManager->configure(configuration);

	// lets now set up and configure the fossil collection manager
	const OptFossilCollManagerFactory *myFossilCollFactory =
			OptFossilCollManagerFactory::instance();
	myrealFossilCollManager
			= dynamic_cast<ThreadedOptFossilCollManager *> (myFossilCollFactory->allocate(
					configuration, this));
	if (myrealFossilCollManager != 0) {
		myrealFossilCollManager->configure(configuration);
		usingOptFossilCollection = true;
		//Event::myOptFosColMan = myFossilCollManager;
		//Event::usingOptFossilCollMan = true;
	} else {
		usingOptFossilCollection = false;
	}

	registerSimulationObjects();

	ASSERT(myCommunicationManager != NULL);
	// call the communication manager to send out initialization
	// messages and wait for all initialization messages to arrive
	// note: we dont need an initialization message from ourself; and
	// hence the n - 1.
	myCommunicationManager->waitForInitialization(
			numberOfSimulationManagers - 1);
	ostringstream oss;
	oss << "lp" << mySimulationManagerID << ".csv";
	ofstream file(oss.str().c_str());
	if (file) {
		const vector<string>& args = configuration.getArguments();
		vector<string>::const_iterator it(args.begin());
		for (; it != args.end(); ++it)
			file << " " << *it;
		file << endl;
	}

}

// this function constructs the map of simulation object names versus
// simulation object pointers by interacting with the application.
// It also performs random partitioning of objects.
SimulationManagerImplementationBase::typeSimMap *
ThreadedTimeWarpSimulationManager::createMapOfObjects() {
	typeSimMap *retval = 0;

	const PartitionInfo *appPartitionInfo = myApplication->getPartitionInfo(
			numberOfSimulationManagers);

	vector<SimulationObject *> *localObjects;

	for (int n = 0; n < numberOfSimulationManagers; n++) {
		if (n == getSimulationManagerID()) {
			localObjects = appPartitionInfo->getObjectSet(n);
		} else {
			// Delete the remote objects, they will not be used on this sim manager.
			vector<SimulationObject *> *remoteObjects =
					appPartitionInfo->getObjectSet(n);
			for (int d = 0; d < remoteObjects->size(); d++) {
				delete (*remoteObjects)[d];
			}
		}
	}

	for (vector<SimulationObject *>::iterator i = localObjects->begin(); i
			!= localObjects->end(); i++) {
		(*i)->setSimulationManager(this);
	}
	retval = partitionVectorToHashMap(localObjects);

	setNumberOfObjects(retval->size());

	delete appPartitionInfo;

	return retval;
}
void ThreadedTimeWarpSimulationManager::getGVTTimePeriodLock(int threadId) {
	while (!GVTTimePeriodLock->setLock(threadId))
		;
	ASSERT(GVTTimePeriodLock->hasLock(threadId));
}
void ThreadedTimeWarpSimulationManager::releaseGVTTimePeriodLock(int threadId) {
	ASSERT(GVTTimePeriodLock->hasLock(threadId));
	GVTTimePeriodLock->releaseLock(threadId);
}

void ThreadedTimeWarpSimulationManager::getLVTFlagLock(unsigned int threadId) {
	while (!LVTFlagLock->setLock(threadId))
		;
	ASSERT(LVTFlagLock->hasLock(threadId));
}
void ThreadedTimeWarpSimulationManager::releaseLVTFlagLock(
		unsigned int threadId) {
	ASSERT(LVTFlagLock->hasLock(threadId));
	LVTFlagLock->releaseLock(threadId);
}

// the Sim Manager must register the following message
// types with the communication manager:

// InitializationMessage, StartMessage, EventMessage, NegativeEventMessage,
// CheckIdleMessage, AbortSimulationMessage
void ThreadedTimeWarpSimulationManager::registerWithCommunicationManager() {

	const int numberOfMessageTypes = 6;
	string messageType[numberOfMessageTypes] = { "InitializationMessage",
			"StartMessage", "EventMessage", "NegativeEventMessage",
			"CheckIdleMessage", "AbortSimulationMessage" };
	ASSERT(myCommunicationManager != NULL);
	for (int count = 0; count < numberOfMessageTypes; count++) {
		myCommunicationManager->registerMessageType(messageType[count], this);
	}
}

bool ThreadedTimeWarpSimulationManager::simulationComplete(
		const VTime &simulateUntil) {
	bool retval = false;
	if (!usingOptFossilCollection) {
		if (myGVTManager->getGVT() >= simulateUntil) {
			utils::debug << "(" << mySimulationManagerID << " ) GVT = "
					<< myGVTManager->getGVT() << ", >= " << simulateUntil
					<< endl;
			retval = true;
		} else if (myTerminationManager->terminateSimulation()) {
			utils::debug << "(" << mySimulationManagerID
					<< " ) Termination manager says we're complete." << endl;
			simulationCompleteFlag = true;
			retval = true;
		}
	} else {
		if (myTerminationManager->terminateSimulation()) {
			utils::debug << "(" << mySimulationManagerID
					<< " ) Termination Manager says we're complete." << endl;
			retval = true;
		} else if (myrealFossilCollManager->getLeastCollectTime()
				> simulateUntil.getApproximateIntTime()) {
			utils::debug << "(" << mySimulationManagerID
					<< " ) Minimum Fossil Collected Time = "
					<< myrealFossilCollManager->getLeastCollectTime()
					<< ", >= " << simulateUntil << endl;
			retval = true;
		}
	}
	return retval;
}

const VTime *
ThreadedTimeWarpSimulationManager::getCoastForwardTime(
		const unsigned int &objectID) const {
	return coastForwardTime[objectID];
}
void ThreadedTimeWarpSimulationManager::registerSimulationObjects() {
	// allocate memory for our reverse map
	localArrayOfSimObjIDs.resize(numberOfObjects);

	// allocate memory for our global reverse map - first dimension
	globalArrayOfSimObjIDs.resize(numberOfSimulationManagers);

	// allocate memory for the second dimension of our 2-D array
	globalArrayOfSimObjIDs[mySimulationManagerID].resize(numberOfObjects);

	//Obtains all the keys from localArrayOfSimObjPtrs
	vector < string > *keys = getKeyVector(localArrayOfSimObjPtrs);
	//Obtains all the objects from localArrayOfSimObjPtrs
	vector<SimulationObject *> *objects = getElementVector(
			localArrayOfSimObjPtrs);

	for (unsigned int i = 0; i < objects->size(); i++) {
		// create and store in the map a relation between ids and object names
		OBJECT_ID *id = new OBJECT_ID(i, mySimulationManagerID);

		SimulationObject *object = (*objects)[i];

		// store in the global map
		globalArrayOfSimObjPtrs.insert(std::make_pair((*keys)[i], object));

		// store this objects id for future reference
		object->setObjectID(id);

		// store a handle to our simulation manager in the object
		object->setSimulationManager(this);

		// lets allocate the initial state here
		// No longer used for optimistic fossil collection.
		/*if(!usingOptFossilCollection){
		 object->setInitialState(object->allocateState());
		 }
		 else{
		 object->setInitialState(myFossilCollManager->newState(object));
		 }*/
		object->setInitialState(object->allocateState());

		// save map of ids to ptrs
		localArrayOfSimObjIDs[i] = object;
		globalArrayOfSimObjIDs[mySimulationManagerID][i] = object;

		// initialize the coast forward vector element for the object
		coastForwardTime.push_back(0);

		// initialize the input and output file queues.
		inFileQueues.push_back(vector<TimeWarpSimulationStream*> ());
		outFileQueues.push_back(vector<TimeWarpSimulationStream*> ());
	}
	delete objects;
	delete keys;
	for (int i = 0; i < numberOfObjects; i++) {
		resetRollbackCompletedStatus(i);
	}
}

void ThreadedTimeWarpSimulationManager::handleAntiMessageFromStraggler(
		const Event* stragglerEvent, int threadId) {
	const vector<const NegativeEvent *>
			eventsToCancel =
					dynamic_cast<const StragglerEvent*> (stragglerEvent)->getEventsToCancel();
	SimulationObject *receiver = getObjectHandle(
			eventsToCancel[0]->getReceiver());
	for (vector<const NegativeEvent *>::const_iterator i =
			eventsToCancel.begin(); i < eventsToCancel.end(); i++) {
		ASSERT(
				(*i)->getReceiveTime() >= receiver->getSimulationTime());

		//If the event was found and the sender is remote, delete the negative event
		//in the case the event is not found it will be stored in the negativeEventsQueue for this object
		//until the corresponding positive event arrives
		if (myEventSet->handleAntiMessage(receiver, *i, threadId)) {
			//	delete (*i);
		}
		delete (*i);
	}
	utils::debug << "(" << mySimulationManagerID << " T " << threadId
			<< " ) - Finished Handling Negative Events " << "\n";
	delete dynamic_cast<const StragglerEvent*> (stragglerEvent)->getPositiveEvent();
	delete stragglerEvent;
}
void ThreadedTimeWarpSimulationManager::printObjectMaaping() {
	vector<SimulationObject *> *objects = getElementVector(
			localArrayOfSimObjPtrs);

	for (unsigned int i = 0; i < objects->size(); i++) {
		SimulationObject *object = (*objects)[i];
		utils::debug << object->getName() << " -- "
				<< object->getObjectID()->getSimulationObjectID() << endl;
	}
}
bool ThreadedTimeWarpSimulationManager::checkTermination() {
	return (myEventSet->isScheduleQueueEmpty(scheduleQCount));
}

void ThreadedTimeWarpSimulationManager::updateLVTArray(unsigned int threadId,
		unsigned int objId) {
	if (*(computeLVTStatus[threadId]) == 0) {
		const VTime
				* nextEventTime =
						dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->getMinEventTime(
								threadId, objId);
		if (sendMinTimeArray[threadId] != NULL && nextEventTime != NULL) {
			utils::debug << "(" << mySimulationManagerID << "," << threadId
					<< " ) UP Min Time ::" << *nextEventTime << endl;
			utils::debug << "(" << mySimulationManagerID << "," << threadId
					<< " ) send Min Time ::" << *(sendMinTimeArray[threadId])
					<< endl;
			LVTArray[threadId]
					= *nextEventTime > *(sendMinTimeArray[threadId]) ? sendMinTimeArray[threadId]
							: nextEventTime;
			decrementLVTFlag(threadId);
			sendMinTimeArray[threadId] = NULL;
			*(computeLVTStatus[threadId]) = 1;
		}
	}

}

void ThreadedTimeWarpSimulationManager::updateLVTArray(unsigned int threadId,
		VTime* nextEventTime) {
	if (*(computeLVTStatus[threadId]) == 0) {
		if (sendMinTimeArray[threadId] != NULL && nextEventTime != NULL) {
			utils::debug << "(" << mySimulationManagerID << "," << threadId
					<< " ) UP Min Time ::" << *nextEventTime << endl;
			utils::debug << "(" << mySimulationManagerID << "," << threadId
					<< " ) send Min Time ::" << *(sendMinTimeArray[threadId])
					<< endl;
			LVTArray[threadId]
					= *nextEventTime > *(sendMinTimeArray[threadId]) ? sendMinTimeArray[threadId]
							: nextEventTime;
			decrementLVTFlag(threadId);
			sendMinTimeArray[threadId] = NULL;
			*(computeLVTStatus[threadId]) = 1;
		}
	}
}

inline void ThreadedTimeWarpSimulationManager::updateSendMinTime(
		unsigned int threadId, const VTime* sendTime) {
	if (*(computeLVTStatus[threadId]) == 0) {
		if (sendMinTimeArray[threadId] == NULL) {
			sendMinTimeArray[threadId] = sendTime->clone();
		} else if (sendMinTimeArray[threadId] != NULL
				&& *sendMinTimeArray[threadId] > *sendTime) {
			//delete(sendMinTimeArray[threadId]);
			sendMinTimeArray[threadId] == sendTime->clone();
		}
	}

}

void ThreadedTimeWarpSimulationManager::decrementLVTFlag(unsigned int threadId) {
	getLVTFlagLock(threadId);
	LVTFlag--;
	utils::debug << "(" << mySimulationManagerID << " T " << threadId << " )"
			<< "Decrementing LVTFlag .. Current value :: " << LVTFlag << endl;
	ASSERT(LVTFlag >= 0);
	releaseLVTFlagLock(threadId);
}

void ThreadedTimeWarpSimulationManager::resetComputeLVTStatus() {
	for (int i = 1; i < numberOfWorkerThreads; i++) {
		*(computeLVTStatus[i]) = 0;
	}
}
void ThreadedTimeWarpSimulationManager::setComputeLVTStatus() {
	for (int i = 0; i < numberOfWorkerThreads + 1; i++) {
		computeLVTStatus[i] = new bool(0);
		*(computeLVTStatus[i]) = 1;
		//	sendMinTimeArray[i] = NULL;
	}
}
bool ThreadedTimeWarpSimulationManager::updateLVTfromArray() {
	bool ret = false;
	getLVTFlagLock(0);
	if (LVTFlag == 0) {
		lvtCount++;
		const VTime* minimum = &(getPositiveInfinity());
		for (int i = 1; i < numberOfWorkerThreads; i++) {
			if (LVTArray[i] != 0 && *LVTArray[i] < *minimum)
				minimum = LVTArray[i];
		}
		utils::debug << "(" << mySimulationManagerID << " ) Computed LVT ="
				<< *minimum << ":::::" << myGVTManager->getGVT() << "::::::"
				<< endl;
		switch (lvtCount) {
		case 1:
			LVT = minimum->clone();
			LVTFlag = (numberOfWorkerThreads - 1);
			for (int i = 1; i < numberOfWorkerThreads; i++) {
				delete (LVTArray[i]);
			}
			resetComputeLVTStatus();
			break;
		case 2:
			if (*LVT > *minimum) {
				LVT = minimum->clone();
			}
			LVTFlag = (numberOfWorkerThreads - 1);
			for (int i = 1; i < numberOfWorkerThreads; i++) {
				delete (LVTArray[i]);
			}
			resetComputeLVTStatus();
			break;
			/*		case 3:
			 if (*LVT > *minimum) {
			 LVT = minimum->clone();
			 }
			 LVTFlag = (numberOfWorkerThreads - 1);
			 for (int i = 1; i < numberOfWorkerThreads; i++) {
			 delete (LVTArray[i]);
			 }
			 resetComputeLVTStatus();
			 break;*/
		case 3:
			if (*LVT > *minimum) {
				LVT = minimum->clone();
			}
			ret = true;
			lvtCount = 0;
			for (int i = 1; i < numberOfWorkerThreads; i++) {
				delete (LVTArray[i]);
			}

		}
	}
	releaseLVTFlagLock(0);
	return ret;
}
const VTime* ThreadedTimeWarpSimulationManager::getLVT() {
	return LVT->clone();
}
/// Used in optimistic fossil collection to checkpoint the file queues.
void ThreadedTimeWarpSimulationManager::saveFileQueuesCheckpoint(
		ofstream* outFile, const ObjectID &objId, unsigned int saveTime) {
	unsigned int i = objId.getSimulationObjectID();
	for (int j = 0; j < outFileQueues[i].size(); j++) {
		(outFileQueues[i][j])->saveCheckpoint(outFile, saveTime);
	}
	for (int j = 0; j < inFileQueues[i].size(); j++) {
		(inFileQueues[i][j])->saveCheckpoint(outFile, saveTime);
	}
}

/// Used in optimistic fossil collection to restore the file queues.
void ThreadedTimeWarpSimulationManager::restoreFileQueues(ifstream* inFile,
		const ObjectID &objId, unsigned int restoreTime) {
	unsigned int i = objId.getSimulationObjectID();
	for (int j = 0; j < outFileQueues[i].size(); j++) {
		(outFileQueues[i][j])->restoreCheckpoint(inFile, restoreTime);
	}

	for (int j = 0; j < inFileQueues[i].size(); j++) {
		(inFileQueues[i][j])->restoreCheckpoint(inFile, restoreTime);
	}
}

void ThreadedTimeWarpSimulationManager::fossilCollectFileQueues(
		SimulationObject *object, int fossilCollectTime) {
	unsigned int objID = object->getObjectID()->getSimulationObjectID();

	if (!inFileQueues[objID].empty()) {
		vector<TimeWarpSimulationStream*>::iterator iter =
				inFileQueues[objID].begin();
		vector<TimeWarpSimulationStream*>::iterator iter_end =
				inFileQueues[objID].end();
		while (iter != iter_end) {
			(*iter)->fossilCollect(fossilCollectTime);
			++iter;
		}
	}

	if (!outFileQueues[objID].empty()) {
		vector<TimeWarpSimulationStream*>::iterator iter =
				outFileQueues[objID].begin();
		vector<TimeWarpSimulationStream*>::iterator iter_end =
				outFileQueues[objID].end();
		while (iter != iter_end) {
			(*iter)->fossilCollect(fossilCollectTime);
			++iter;
		}
	}
}
bool ThreadedTimeWarpSimulationManager::initiateLocalGVT() {
	bool ret = false;
	getLVTFlagLock(0);
	if (LVTFlag == 0) {
		resetComputeLVTStatus();
		LVTFlag = (numberOfWorkerThreads - 1);
		ret = true;
	}
	releaseLVTFlagLock(0);
	return ret;
}

bool ThreadedTimeWarpSimulationManager::setGVTTokenPending() {
	return __sync_bool_compare_and_swap(&GVTTokenPending, false, true);
}

bool ThreadedTimeWarpSimulationManager::resetGVTTokenPending() {
	return __sync_bool_compare_and_swap(&GVTTokenPending, true, false);
}

bool ThreadedTimeWarpSimulationManager::isRollbackJustCompleted(int objId) {
	return rollbackCompleted[objId];
}

void ThreadedTimeWarpSimulationManager::setRollbackCompletedStatus(int objId) {
	rollbackCompleted[objId] = 1;
}
void ThreadedTimeWarpSimulationManager::resetRollbackCompletedStatus(int objId) {
	rollbackCompleted[objId] = 0;
}
// Releasing all the object locks during recovery once all the threads stop execution
// This happens only during a catastrophic rollback.
void ThreadedTimeWarpSimulationManager::releaseObjectLocksRecovery() {
	dynamic_cast<ThreadedTimeWarpMultiSet*> (myEventSet)->releaseObjectLocksRecovery();
}

void ThreadedTimeWarpSimulationManager::clearMessageBuffer() {
	KernelMessage *tobedeleted = NULL;
	while ((tobedeleted = messageBuffer->dequeue(syncMechanism)) != NULL) {
		utils::debug << "Deleted message from buffer." << endl;
	}
}

