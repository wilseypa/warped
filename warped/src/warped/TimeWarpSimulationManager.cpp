// See copyright notice in file Copyright in the root directory of this archive.

#include "StopWatch.h"
#include "ObjectStub.h"
#include "SimulationObjectProxy.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSimulationStream.h"
#include "TimeWarpSimulationObjectQueue.h"
#include "SchedulingManager.h"
#include "CommunicationManager.h"
#include "TerminationManager.h"
#include "AggressiveOutputManager.h"
#include "LazyOutputManager.h"
#include "AdaptiveOutputManager.h"
#include "Application.h"
#include "SchedulingData.h"
#include "PartitionInfo.h"
#include "TokenPassingTerminationManager.h"
#include "SingleTerminationManager.h"
#include "EventMessage.h"
#include "InitializationMessage.h"
#include "NegativeEventMessage.h"
#include "StartMessage.h"
#include "CostAdaptiveStateManager.h"
#include "EventFunctors.h"
#include "OptFossilCollManager.h"
#include "OptFossilCollManagerFactory.h"
#include "DVFSManager.h"
#include "DistributedDVFSManager.h"
#include "DVFSManagerFactory.h"
#include "SimulationConfiguration.h"
#include <utils/Debug.h>
#include <algorithm>
#include <sstream>
#include <time.h>
using std::istringstream;

TimeWarpSimulationManager::TimeWarpSimulationManager(Application *initApplication) :
    SimulationManagerImplementationBase(),
			mySimulationManagerID(0), simulationCompleteFlag(false),
			coastForwardTime(0), messageAggregation(false), myStateManager(0),
			myGVTManager(0), myCommunicationManager(0), mySchedulingManager(0),
			myOutputManager(0), myEventSet(0),
			mySchedulingData(new SchedulingData()), myTerminationManager(0),
			myApplication(initApplication), myFossilCollManager(0),
			usingOneAntiMsg(false), usingOptFossilCollection(false),
            inRecovery(false), numberOfRollbacks(0) {
}

TimeWarpSimulationManager::~TimeWarpSimulationManager() {
	if (myOutputManager != NULL) {// to skip this part of destructor when using Threaded Timewarp
		fossilCollect(getPositiveInfinity());
		if (usingOptFossilCollection) {
			vector<SimulationObject *> *objects = getElementVector(
					localArrayOfSimObjPtrs);
			for (unsigned int n = 0; n < objects->size(); n++) {
				myStateManager->fossilCollect((*objects)[n],
						getPositiveInfinity());
				myOutputManager->fossilCollect((*objects)[n],
						getPositiveInfinity());
				myEventSet->fossilCollect((*objects)[n], getPositiveInfinity());
				fossilCollectFileQueues((*objects)[n],
						getPositiveInfinity().getApproximateIntTime());
			}
		}
	}
	//Clean up the file queues.
	for (unsigned int i = 0; i < outFileQueues.size(); i++) {
		for (unsigned int j = 0; j < outFileQueues[i].size(); j++) {
			delete outFileQueues[i][j];
		}
	}

	for (unsigned int i = 0; i < inFileQueues.size(); i++) {
		for (unsigned int j = 0; j < inFileQueues[i].size(); j++) {
			delete inFileQueues[i][j];
		}
	}

	//Erases all objects including object proxies
	for (unsigned int simMgrID = 0; simMgrID < numberOfSimulationManagers; simMgrID++) {
		for (unsigned int objID = 0; objID
				< globalArrayOfSimObjIDs[simMgrID].size(); objID++) {
			delete globalArrayOfSimObjIDs[simMgrID][objID];
		}
	}

	// This simulation manager is responsible for the deletion of all
	// its components.
	delete myFossilCollManager;
	delete myTerminationManager;
	delete mySchedulingData;
	delete myEventSet;
	delete myOutputManager;
	delete mySchedulingManager;
	delete myCommunicationManager;
	delete myGVTManager;
	delete myStateManager;
	delete localArrayOfSimObjPtrs;
	delete mySimulationObjectQueue;
    delete myDVFSManager;
}

const VTime &
TimeWarpSimulationManager::getCommittedTime() {
	ASSERT(myGVTManager != 0);
	return myGVTManager->getGVT();
}

const VTime &
TimeWarpSimulationManager::getNextEventTime() {
	const Event *nextEvent = myEventSet->peekEvent(0);
	if (nextEvent != 0) {
		return nextEvent->getReceiveTime();
	} else {
		return getPositiveInfinity();
	}
}

const VTime *
TimeWarpSimulationManager::getCoastForwardTime(const unsigned int &objectID) const {
	return coastForwardTime[objectID];
}

bool TimeWarpSimulationManager::checkMessageAggregationFlag() {
	return messageAggregation;
}

void TimeWarpSimulationManager::setMessageAggregationFlag(bool flag) {
	messageAggregation = flag;
}

void TimeWarpSimulationManager::registerSimulationObjects() {
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
}

// this function constructs the map of simulation object names versus
// simulation object pointers by interacting with the application.
// It also performs random partitioning of objects.
SimulationManagerImplementationBase::typeSimMap *
TimeWarpSimulationManager::createMapOfObjects() {
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

// this function is used by the communication manager to send out the
// names of objects on this simulation manager in the form of an init
// message during the start-up of a distributed simulation. 
vector<string>*
TimeWarpSimulationManager::getSimulationObjectNames() {
	//Obtains all the keys from localArrayOfSimObjPtrs
	vector < string > *keys = getKeyVector(localArrayOfSimObjPtrs);
	return keys;
}

// this function is called by the communication manager to make the
// simulation manager add the set of simulation object proxies (a list
// of which is passed in as a parameter for a specific simulation
// manager)

void TimeWarpSimulationManager::registerSimulationObjectProxies(
		const vector<string> *arrayOfObjectProxies,
		unsigned int sourceSimulationManagerID,
		unsigned int destSimulationManagerID) {
	// allocate space of the second dimension of this 2-D array.
	globalArrayOfSimObjIDs[destSimulationManagerID].resize(
			arrayOfObjectProxies->size());

	unsigned int count = 0;
	// iterate through the vector ...
	for (vector<string>::const_iterator iter = (*arrayOfObjectProxies).begin(); iter
			< (*arrayOfObjectProxies).end(); iter++) {
		// create and store in the map
		OBJECT_ID *id = new OBJECT_ID(count, destSimulationManagerID);

		ASSERT(myCommunicationManager != NULL);

		// create the proxy simulation object.
		SimulationObjectProxy *object = new SimulationObjectProxy(*iter,
				sourceSimulationManagerID, destSimulationManagerID,
				myCommunicationManager);

		// store this proxy object's id for future reference
		object->setObjectID(id);

		// store a handle to our simulation manager in this proxy object
		object->setSimulationManager(this);

		// save in global map of names to ptrs
		globalArrayOfSimObjPtrs.insert(std::make_pair((*iter), object));

		// save in global map of ids to ptrs
		globalArrayOfSimObjIDs[destSimulationManagerID][count] = object;

		count++;
	}
}

// the TimeWarpSimulationManager must register the following message
// types with the communication manager:

// InitializationMessage, StartMessage, EventMessage, NegativeEventMessage,
// CheckIdleMessage, AbortSimulationMessage
void TimeWarpSimulationManager::registerWithCommunicationManager() {
	const char* messageTypes[] = { "InitializationMessage",
			"StartMessage", "EventMessage", "NegativeEventMessage",
			"CheckIdleMessage", "AbortSimulationMessage" };
	ASSERT(myCommunicationManager != NULL);
	int numberOfMessageTypes = sizeof(messageTypes) / sizeof(const char*);
	for (int count = 0; count < numberOfMessageTypes; count++) {
		myCommunicationManager->registerMessageType(messageTypes[count], this);
	}
}

/// Used in optimistic fossil collection to checkpoint the file queues.
void TimeWarpSimulationManager::saveFileQueuesCheckpoint(ofstream* outFile,
		const ObjectID &objId, unsigned int saveTime) {
	unsigned int i = objId.getSimulationObjectID();
	for (int j = 0; j < outFileQueues[i].size(); j++) {
		(outFileQueues[i][j])->saveCheckpoint(outFile, saveTime);
	}

	for (int j = 0; j < inFileQueues[i].size(); j++) {
		(inFileQueues[i][j])->saveCheckpoint(outFile, saveTime);
	}
}

/// Used in optimistic fossil collection to restore the file queues.
void TimeWarpSimulationManager::restoreFileQueues(ifstream* inFile,
		const ObjectID &objId, unsigned int restoreTime) {
	unsigned int i = objId.getSimulationObjectID();
	for (int j = 0; j < outFileQueues[i].size(); j++) {
		(outFileQueues[i][j])->restoreCheckpoint(inFile, restoreTime);
	}

	for (int j = 0; j < inFileQueues[i].size(); j++) {
		(inFileQueues[i][j])->restoreCheckpoint(inFile, restoreTime);
	}
}

unsigned int TimeWarpSimulationManager::getNumEventsExecuted() {
  return myEventSet->getNumEventsExecuted();
}

unsigned int TimeWarpSimulationManager::getNumEventsRolledBack() {
  return myEventSet->getNumEventsRolledBack();
}

bool TimeWarpSimulationManager::simulationComplete(const VTime &simulateUntil) {
	bool retval = false;
	if (myGVTManager->getGVT() >= simulateUntil) {
		utils::debug << "(" << getSimulationManagerID() << ") GVT = "
				<< myGVTManager->getGVT() << ", >= " << simulateUntil << endl;
		retval = true;
	} else if (myTerminationManager->terminateSimulation()) {
		utils::debug << "(" << getSimulationManagerID()
				<< ") Termination manager says we're complete." << endl;
		simulationCompleteFlag = true;
		retval = true;
	}
	return retval;
}

void TimeWarpSimulationManager::getMessages() {
	if (numberOfSimulationManagers > 1) {
		myCommunicationManager->checkPhysicalLayerForMessages(1000);
	}
}

bool TimeWarpSimulationManager::executeObjects(const VTime& simulateUntil) {
	bool hadAtLeastOne = false;

	// This will be used to break out of the loop if past the simulateUntil time.
	bool pastSimulationCompleteTime = false;
	unsigned int checkPeriod = 1;
	unsigned int currentCheck = 1;

	const Event *nextEvent = mySchedulingManager->peekNextEvent();

	while (nextEvent != 0 && !pastSimulationCompleteTime && !inRecovery) {
		hadAtLeastOne = true;

		if (myDVFSManager) {
		  myDVFSManager->poll();
		}

		SimulationObject *nextObject =
				getObjectHandle(nextEvent->getReceiver());
		ASSERT( nextObject != 0 );
		ASSERT( dynamic_cast<SimulationObjectProxy *>( nextObject ) == 0 );

		if (usingOptFossilCollection) {
			myFossilCollManager->checkpoint(nextEvent->getReceiveTime(),
					*nextObject->getObjectID());
			myFossilCollManager->fossilCollect(nextObject,
					nextEvent->getReceiveTime());
		}

		nextObject->setSimulationTime(nextEvent->getReceiveTime());
		myStateManager->saveState(nextEvent->getReceiveTime(), nextObject);

		/* Moving the function nextObject->executeProcess() after the calculateGVT() function
		 * of the SimulationManager with Id = 0. This is done to make LVT Calculation independent
		 * of the lowest timestamp of all events in the lazy queues. For more details see Sanchit
		 * Saxena's thesis.
		 */

		//	nextObject->executeProcess();

		// Only the master sim manager starts the GVT calculation process.
		// Fossil collection occurs when a new GVT value is set.
		if (mySimulationManagerID == 0) {
			if (myGVTManager->checkGVTPeriod()) {
				myGVTManager->calculateGVT();
				if (myGVTManager->getGVT() >= simulateUntil) {
					pastSimulationCompleteTime = true;
				}
			}
		}

		nextObject->executeProcess();

		// Clear any event from the lazy cancel queues that have lower timestamps than the
		// next event.
		nextEvent = mySchedulingManager->peekNextEvent();
		if (outMgrType == LAZYMGR) {
			LazyOutputManager *myLazyOutputManager =
					static_cast<LazyOutputManager *> (myOutputManager);
			ASSERT(myLazyOutputManager != NULL);
			if (nextEvent != NULL)
				myLazyOutputManager->emptyLazyQueues(
						nextEvent->getReceiveTime());
		} else if (outMgrType == ADAPTIVEMGR) {
			DynamicOutputManager *myDynamicOutputManager =
					static_cast<DynamicOutputManager *> (myOutputManager);
			ASSERT(myDynamicOutputManager != NULL);
			if (nextEvent != NULL)
				myDynamicOutputManager->emptyLazyQueues(
						nextEvent->getReceiveTime());
		}

		if (currentCheck >= checkPeriod) {
			myCommunicationManager->checkPhysicalLayerForMessages(1000);
			currentCheck = 1;
		}

		nextEvent = mySchedulingManager->peekNextEvent();
	}

	// This has to be done here, when it is certain there are no more events to process and as
	// a result, no way to regenerate the same events that remain to be checked.
	if (outMgrType == LAZYMGR) {
		LazyOutputManager *myLazyOutputManager =
				static_cast<LazyOutputManager *> (myOutputManager);
		ASSERT(myLazyOutputManager != NULL);
		myLazyOutputManager->emptyLazyQueues(getPositiveInfinity());
	} else if (outMgrType == ADAPTIVEMGR) {
		DynamicOutputManager *myDynamicOutputManager =
				static_cast<DynamicOutputManager *> (myOutputManager);
		ASSERT(myDynamicOutputManager != NULL);
		myDynamicOutputManager->emptyLazyQueues(getPositiveInfinity());
	}

	return hadAtLeastOne;
}

void TimeWarpSimulationManager::simulate(const VTime& simulateUntil) {
  cout << "SimulationManager(" << mySimulationManagerID
                  << "): Starting simulation - End time: " << simulateUntil << ")"
                  << endl;

	myStopwatch.start();
  while (!simulationComplete(simulateUntil)) {
          getMessages();
          while (inRecovery) {
                  getMessages();
          }

          ASSERT(mySchedulingManager != NULL);
          if (executeObjects(simulateUntil)) {
                  myTerminationManager->setStatusActive();
          } else if (getNumberOfSimulationManagers() == 1) {
                  dynamic_cast<SingleTerminationManager*> (myTerminationManager)->simulationComplete();
          } else {
                  myTerminationManager->setStatusPassive();
          }
  }
  myStopwatch.stop();

  cout << "(" << getSimulationManagerID() << ") Simulation complete ("
                  << myStopwatch.elapsed() << " secs), Number of Rollbacks: ("
                  << numberOfRollbacks << ")" << endl;

	// This is commented out by default. It is used along with the testParallelWarped script
	// to provide a quick and easy way to do large amounts of tests. Make sure to change the
	// filename to a proper one. The 'analyzer' program can then be run to format the results.
	/*
	 ofstream outFile;
	 ostringstream filename;
	 filename << "/home/kingr8/results/Results.LP." << mySimulationManagerID;
	 outFile.open(filename.str().c_str(), ios_base::app);
	 outFile << stopwatch.elapsed() << " " << numberOfRollbacks << endl;
	 */
}

const Event *
TimeWarpSimulationManager::getEvent(SimulationObject *object) {
	ASSERT(myEventSet != NULL);
	const Event *retval;
	int id = object->getObjectID()->getSimulationObjectID();

	if (getCoastForwardTime(id) != 0) {
		retval = myEventSet->getEvent(object, *getCoastForwardTime(id));
	} else {
		retval = myEventSet->getEvent(object);
	}

	return retval;
}

const Event *
TimeWarpSimulationManager::peekEvent(SimulationObject *object) {
	ASSERT(myEventSet != NULL);
	const Event *retval;
	int id = object->getObjectID()->getSimulationObjectID();

	if (getCoastForwardTime(id) != 0) {
		retval = myEventSet->peekEvent(object, *getCoastForwardTime(id));
	} else {
		retval = myEventSet->peekEvent(object);
	}

	return retval;
}

void TimeWarpSimulationManager::handleEvent(const Event *event) {
	ASSERT(myOutputManager != NULL);
	// first set the send time of the event
	ASSERT(event != NULL);

	bool shouldHandleEvent = true;
	int id = event->getSender().getSimulationObjectID();

	if (getCoastForwardTime(id) != NULL || inRecovery) {
		shouldHandleEvent = false;
		delete event;
		event = NULL;
	} else {
		if (contains(event->getSender())) {
			if (outMgrType == AGGRMGR) {
				myOutputManager->insert(event);
			} else if (outMgrType == LAZYMGR) {
				LazyOutputManager *myLazyOutputManager =
						static_cast<LazyOutputManager *> (myOutputManager);
				ASSERT(myLazyOutputManager != NULL);
				shouldHandleEvent = !myLazyOutputManager->lazyCancel(event);
			} else if (outMgrType == ADAPTIVEMGR) {
				DynamicOutputManager *myDynamicOutputManager =
						static_cast<DynamicOutputManager *> (myOutputManager);
				ASSERT(myDynamicOutputManager != NULL);
				shouldHandleEvent
						= !myDynamicOutputManager->checkDynamicCancel(event);
			}
		}
	}

	if (shouldHandleEvent) {
		//This function is replaced in the ThreadedSimulationManager
		handleEventReceiver(getObjectHandle(event->getReceiver()), event);
	}
}

void TimeWarpSimulationManager::handleEventReceiver(
		SimulationObject *currObject, const Event *event) {
	if (contains(event->getReceiver()) == true) {
		handleLocalEvent(event);
	} else {
		handleRemoteEvent(event);
	}
}

void TimeWarpSimulationManager::handleLocalEvent(const Event *event) {
	SimulationObject *receiver = getObjectHandle(event->getReceiver());
	//  if( !myEventSet->inThePast( event ) ){
	if (event->getReceiveTime() > receiver->getSimulationTime()) {
		myEventSet->insert(event);
	} else {
		rollback(receiver, event->getReceiveTime());
		myEventSet->insert(event);
	}
}

void TimeWarpSimulationManager::handleRemoteEvent(const Event *event) {
	int
			id =
					getObjectHandle(event->getSender())->getObjectID()->getSimulationObjectID();
	if (getCoastForwardTime(id) == 0) {
		SimulationObjectProxy *proxyObj;
		proxyObj = dynamic_cast<SimulationObjectProxy *> (getObjectHandle(
				event->getReceiver()));
		ASSERT( proxyObj != 0 );

		unsigned int destSimMgrId = proxyObj->getDestId();

		const string gVTInfo = myGVTManager->getGVTInfo(
				getSimulationManagerID(), destSimMgrId, event->getSendTime());

		KernelMessage *msg = new EventMessage(getSimulationManagerID(),
				destSimMgrId, event, gVTInfo);

		sendMessage(msg, destSimMgrId);
	}
}

void TimeWarpSimulationManager::cancelLocalEvents(const vector<
			   const NegativeEvent *> &eventsToCancel) {
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

	SimulationObject *receiver = getObjectHandle(
			eventsToCancel[0]->getReceiver());
	ASSERT( receiver != 0 );
	if (*lowTime <= receiver->getSimulationTime()) {
		rollback(receiver, *lowTime);
	}

	if (!inRecovery) {
		for (vector<const NegativeEvent *>::const_iterator i =
				eventsToCancel.begin(); i < eventsToCancel.end(); i++) {
			utils::debug << mySimulationManagerID << " - Cancelling: " << *(*i)
					<< "\n";
			ASSERT( (*i)->getReceiveTime() >= receiver->getSimulationTime() );
			myEventSet->handleAntiMessage(receiver, *i);
		}
	}
}

void TimeWarpSimulationManager::cancelRemoteEvents(const vector<
		const NegativeEvent *> &eventsToCancel) {
	ASSERT( eventsToCancel.size() > 0 );
	const ObjectID &receiverId = eventsToCancel[0]->getReceiver();
	const SimulationObject *forObject = getObjectHandle(receiverId);
	unsigned int destId = forObject->getObjectID()->getSimulationManagerID();

	utils::debug << mySimulationManagerID << " - Sending "
			<< eventsToCancel.size() << " anti-messages for " << receiverId
			<< endl;

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
		sendMessage(newMessage, destId);
	}

	// The negative events are no longer needed so reclaim them now.
	//	for (int i = 0; i < eventsToCancel.size(); i++) {
	//		delete eventsToCancel[i];
	//	}
}

inline void TimeWarpSimulationManager::sendMessage(KernelMessage *msg,
		unsigned int destSimMgrId) {
	myCommunicationManager->sendMessage(msg, destSimMgrId);
}

void TimeWarpSimulationManager::handleNegativeEvents(
		const vector<const Event*> &negativeEvents) {
	cancelEvents(negativeEvents);
}

void TimeWarpSimulationManager::cancelEvents(
		const vector<const Event *> &eventsToCancel) {
	ASSERT( eventsToCancel.size() > 0 );

	if (usingOneAntiMsg) {
		// Array to determine if anti-message was already added to send.
		std::set<ObjectID> antiMsgAlreadyAdded;
		vector<const Event *> &eventsToCancelNonConst = const_cast<vector<
				const Event *>&> (eventsToCancel);
		vector<const NegativeEvent *> negEvents;

		// Sort the output events.
		std::sort(eventsToCancelNonConst.begin(), eventsToCancelNonConst.end(),
				receiveTimeLessThanEventIdLessThan());

		// Iterate over the eventsToCancel and add them to the cancel events vector.
		unsigned int oldSize = 0;

		for (vector<const Event *>::iterator it =
				eventsToCancelNonConst.begin(); it
				!= eventsToCancelNonConst.end(); it++) {
			antiMsgAlreadyAdded.insert((*it)->getReceiver());
			if (antiMsgAlreadyAdded.size() > oldSize) {
				negEvents.push_back(new NegativeEvent((*it)->getSendTime(),
						(*it)->getReceiveTime(), (*it)->getSender(),
						(*it)->getReceiver(), (*it)->getEventId()));

				oldSize = antiMsgAlreadyAdded.size();
			}
		}

		//This function is replaced when using the ThreadedSimulationManager
		for (int t = 0; t < negEvents.size(); t++) {
			SimulationObject *curObject = getObjectHandle(
					negEvents[t]->getReceiver());
			vector<const NegativeEvent *> temp;
			temp.push_back(negEvents[t]);
			cancelEventsReceiver(curObject, temp);
		}
	} else {
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
			cancelObjectIt = cancelEventObjects.find(getObjectHandle(
					(*cancelEventIt)->getReceiver()));
			if (cancelObjectIt != cancelEventObjects.end()) {
				(*cancelObjectIt).second.push_back(new NegativeEvent(
						(*cancelEventIt)->getSendTime(),
						(*cancelEventIt)->getReceiveTime(),
						(*cancelEventIt)->getSender(),
						(*cancelEventIt)->getReceiver(),
						(*cancelEventIt)->getEventId()));
			} else {
				vector<const NegativeEvent *> evntVec;
				evntVec.push_back(new NegativeEvent(
						(*cancelEventIt)->getSendTime(),
						(*cancelEventIt)->getReceiveTime(),
						(*cancelEventIt)->getSender(),
						(*cancelEventIt)->getReceiver(),
						(*cancelEventIt)->getEventId()));
				cancelEventObjects.insert(std::make_pair(getObjectHandle(
						(*cancelEventIt)->getReceiver()), evntVec));
			}
		}

		//This function is replaced when using the ThreadedSimulationManager
		cancelObjectIt = cancelEventObjects.begin();
		while (cancelObjectIt != cancelEventObjects.end()) {
			SimulationObject *curObject = cancelObjectIt->first;
			vector<const NegativeEvent*> &ne = cancelObjectIt->second;
			cancelEventsReceiver(curObject, ne);
			if(contains(*(curObject->getObjectID()))) {
				vector<const NegativeEvent*>::const_iterator it(ne.begin());
				while (it != ne.end())
					delete (*it++);
			}
			cancelObjectIt++;
		}
	}
}

void TimeWarpSimulationManager::cancelEventsReceiver(
		SimulationObject *curObject,
		vector<const NegativeEvent *> &cancelObjects) {
	if (contains(curObject->getName())) {
		cancelLocalEvents(cancelObjects);
	} else {
		cancelRemoteEvents(cancelObjects);
	}
}

void TimeWarpSimulationManager::rollback(SimulationObject *object,
		const VTime &rollbackTime) {
	ASSERT( object != 0 );
	ASSERT( dynamic_cast<SimulationObjectProxy *>(object) == 0 );
	utils::debug << "(" << mySimulationManagerID << ")" << object->getName()
			<< " rollback from " << object->getSimulationTime() << " to "
			<< rollbackTime << endl;

	if (rollbackTime < myGVTManager->getGVT()) {
		cerr << object->getName() << " Rollback beyond the Global Virtual Time"
				<< endl;
		cerr << "rollbackTime = " << rollbackTime << " GVT = "
				<< myGVTManager->getGVT() << endl;

		abort();
	}

	numberOfRollbacks++;

	unsigned int objId = object->getObjectID()->getSimulationObjectID();

	// the object's local virtual time is the timestamp of the restored
	// state.  after insert/handleAntiMessage, the next event to
	// execute will posses a timestamp greater than the object's lVT.
	const VTime *restoredTime = &myStateManager->restoreState(rollbackTime,
			object);

	if (usingOptFossilCollection) {
		if (!inRecovery) {
			myFossilCollManager->sampleRollback(object, *restoredTime);
			myFossilCollManager->updateCheckpointTime(objId,
					rollbackTime.getApproximateIntTime());
			if (*restoredTime > rollbackTime) {

				// Only start recovery if this is an actual fault, otherwise continue normal rollback.
				if (myFossilCollManager->checkFault(object)) {
					utils::debug << mySimulationManagerID
							<< " - Catastrophic Rollback: Restored State Time: "
							<< restoredTime << ", Rollback Time: "
							<< rollbackTime << ", Starting Recovery." << endl;

					myFossilCollManager->startRecovery(objId,
							rollbackTime.getApproximateIntTime());
				} else {
					restoredTime = &getZero();
				}
			}
		}
	}

	if (!inRecovery) {
		object->setSimulationTime(*restoredTime);

		utils::debug << object->getName() << " rolled state back to time: "
				<< *restoredTime << endl;

		ASSERT( *restoredTime <= rollbackTime );

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
		myEventSet->rollback(object, *restoredTime);
		myOutputManager->rollback(object, rollbackTime);

		// if we are using an infequent state saving policy then we must
		// regenerate the current state since an old state will have been
		// restored.
		//if(myStateManager->getStatePeriod() != 0){
		// Note: If an infrequent state saving scheme has been utilized,
		// then the currentPos pointer in the input queue will have been
		// repositioned to the first event that needs to be re-executed
		// during the coastforward phase by the earlier call to insert
		// or handleAntiMessage. If a frequent state saving scheme is
		// utilized, then the currentPos pointer will point to the next
		// event to execute (which will be the straggler event).
		//  coastForward( restoredTime, rollbackTime, object );
		//}
		//RK_NOTE: I changed the save state to occur before execute process,
		//so even with a period of 0, the coast forward phase needs to occur.
		coastForward(*restoredTime, rollbackTime, object);
	}
}

void TimeWarpSimulationManager::coastForward(const VTime &coastForwardFromTime,
		const VTime &moveUpToTime, SimulationObject *object) {
	ASSERT( moveUpToTime >= coastForwardFromTime );
	utils::debug << "(" << mySimulationManagerID << ") " << object->getName()
			<< " coasting forward from " << coastForwardFromTime << " to "
			<< moveUpToTime << endl;

	int objId = object->getObjectID()->getSimulationObjectID();

	// Needed when the application calls
	// TimeWarpSimulationManager::getEvent()
	setCoastForwardTime(objId, moveUpToTime);
	StopWatch stopWatch;
	stopWatch.start();

	// go to the first event to coastforward from and call the object's
	// execute process until the rollbackToTime is reached.
	const Event *findEvent = myEventSet->peekEvent(object, moveUpToTime);
	while (findEvent != 0 && findEvent->getReceiveTime() < moveUpToTime) {
		utils::debug << "(" << mySimulationManagerID
				<< ") - coasting forward, skipping " << "event "
				<< findEvent->getEventId() << " at time << "
				<< findEvent->getReceiveTime() << endl;
		object->setSimulationTime(findEvent->getReceiveTime());
		object->executeProcess();
		findEvent = myEventSet->peekEvent(object, moveUpToTime);
	}

	// If using the cost adaptive state manager, set the time it took to perform the
	// coast forward process.
	stopWatch.stop();

	if (stateMgrType == ADAPTIVESTATE) {
		CostAdaptiveStateManager *CAStateManager =
				static_cast<CostAdaptiveStateManager *> (myStateManager);
		CAStateManager->coastForwardTiming(objId, stopWatch.elapsed());
	}

	utils::debug << "(" << mySimulationManagerID
			<< ") - Finished Coasting Forward for: " << object->getName()
			<< endl;

	// finally turn on message transmission
	clearCoastForwardTime(objId);
}

void TimeWarpSimulationManager::receiveKernelMessage(KernelMessage *msg) {
	ASSERT(msg != NULL);

	myTerminationManager->setStatusActive();

	if (dynamic_cast<EventMessage *> (msg) != 0) {
		EventMessage *eventMsg = dynamic_cast<EventMessage *> (msg);
		const Event *event = eventMsg->getEvent();

		// have to take care of some gvt specific actions here
		myGVTManager->updateEventRecord(eventMsg->getGVTInfo(),
				mySimulationManagerID);
		handleEventReceiver(getObjectHandle(event->getReceiver()), event);
	} else if (dynamic_cast<NegativeEventMessage *> (msg)) {
		NegativeEventMessage *negEventMsg =
				dynamic_cast<NegativeEventMessage *> (msg);
		vector<const NegativeEvent*> events = negEventMsg->getEvents();
		myGVTManager->updateEventRecord(negEventMsg->getGVTInfo(),
				mySimulationManagerID);
		SimulationObject *receiverObject = getObjectHandle(
				events[0]->getReceiver());
		cancelEventsReceiver(receiverObject, events);
	} else if (dynamic_cast<InitializationMessage *> (msg) != 0) {
		InitializationMessage *initMsg =
				dynamic_cast<InitializationMessage *> (msg);
		registerSimulationObjectProxies(&initMsg->getObjectNames(),
				initMsg->getReceiver(), initMsg->getSender());
	} else if (dynamic_cast<StartMessage *> (msg) != 0) {
		utils::debug << "SimulationManager(" << mySimulationManagerID
				<< "): Starting Simulation" << endl;
	} else if (msg->getDataType() == "AbortSimulationMessage") {
		cerr << "TimeWarpSimulationManager is going to abort simulation"
				<< endl;
	} else {
		cerr << "TimeWarpSimulationManager::receiveKernelMessage() received"
				<< " unknown (" << msg->getDataType() << ") message type"
				<< endl;
		cerr << "Aborting simulation ..." << endl;
		abort();
	}
	// we are done with this kernel message
	delete msg;
}

bool TimeWarpSimulationManager::checkIdleStatus() {
	ASSERT(mySchedulingManager != NULL);
	const Event *topEvent = mySchedulingManager->peekNextEvent();
	if (topEvent == NULL)
		return true;
	else
		return false;
}

void TimeWarpSimulationManager::fossilCollect(const VTime& fossilCollectTime) {
	ASSERT( localArrayOfSimObjPtrs != 0 );
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
						fossilCollectTime));
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
				myOutputManager->fossilCollect((*objects)[j], *min);
			}

			for (unsigned int j = 0; j < objects->size(); j++) {
				myEventSet->fossilCollect((*objects)[j], *min);
			}
		}
		delete objects;
	}
}

void TimeWarpSimulationManager::fossilCollectFileQueues(
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

void TimeWarpSimulationManager::initialize() {
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
		myStateManager->saveState(myApplication->getZero(), (*objects)[i]);
	}
	delete objects;

	if (usingOptFossilCollection) {
		myFossilCollManager->makeInitialCheckpoint();
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

void TimeWarpSimulationManager::finalize() {
	utils::debug << "Finalizing Simulation Manager: "
			<< this->getSimulationManagerID() << std::endl;
	//Obtains all the objects from localArrayOfSimObjPtrs
	vector<SimulationObject *> *objects = getElementVector(
			localArrayOfSimObjPtrs);

	for (unsigned int i = 0; i < objects->size(); i++) {
		SimulationObject *object = (*objects)[i];
		// call finalize on the object
		object->finalize();
	}
	delete objects;

	myCommunicationManager->finalize();

	cout.flush();
	//  fossilCollect(currentTime);

	if (myEventSet!=NULL) {
		int numEventsRolledBack = myEventSet->getNumEventsRolledBack();
		int numEventsExecuted = myEventSet->getNumEventsExecuted();

		ostringstream oss;
		oss << "lp" << mySimulationManagerID << ".csv";
		ofstream file(oss.str().c_str(), ios_base::app);
		if(file)
			file << myStopwatch.elapsed() << ',' << numberOfRollbacks
				<< ',' << numEventsExecuted - numEventsRolledBack
				<< ',' << numEventsExecuted << endl;
	}

}

SimulationStream*
TimeWarpSimulationManager::getIFStream(const string &fileName,
		SimulationObject *object) {
	TimeWarpSimulationStream *simStream = new TimeWarpSimulationStream(
			fileName, ios::in, object);
	// store for later reference
	unsigned int objID = object->getObjectID()->getSimulationObjectID();
	inFileQueues[objID].push_back(simStream);
	return simStream;
}

SimulationStream*
TimeWarpSimulationManager::getOFStream(const string &fileName,
		SimulationObject *object, ios::openmode mode) {
	TimeWarpSimulationStream *simStream = new TimeWarpSimulationStream(
			fileName, mode, object);
	// store for later reference
	unsigned int objID = object->getObjectID()->getSimulationObjectID();
	outFileQueues[objID].push_back(simStream);
	return simStream;
}

SimulationStream*
TimeWarpSimulationManager::getIOFStream(const string &fileName,
		SimulationObject *object) {
	// this function is currently not implemented fully as I dont
	// see the utility of this function -- ramanan [05/12/00]
	TimeWarpSimulationStream *simStream = new TimeWarpSimulationStream(
			fileName, ios::in | ios::app, object);
	return simStream;
}

// print out the name to simulation object ptr map
void TimeWarpSimulationManager::displayGlobalObjectMap(ostream& out) {
	if (!globalArrayOfSimObjPtrs.empty()) {
		vector < string > *keys = getKeyVector(localArrayOfSimObjPtrs);
		vector<SimulationObject *> *objects = getElementVector(
				localArrayOfSimObjPtrs);

		for (unsigned int i = 0; i < objects->size(); i++) {
			out << (*keys)[i] << ": " << (*objects)[i]->getObjectID();
		}
		delete objects;
		delete keys;
	} else {
		out << "Object Names to Object Pointers Map is empty" << endl;
	}
}

void TimeWarpSimulationManager::configure(
		SimulationConfiguration &configuration) {
	const CommunicationManagerFactory *myCommFactory =
			CommunicationManagerFactory::instance();

	myCommunicationManager
			= dynamic_cast<CommunicationManager *> (myCommFactory->allocate(
					configuration, this));
	ASSERT( myCommunicationManager != 0 );
    // update the number of the simulation managers with the number
    // provided by the physical communication layer
    numberOfSimulationManagers = myCommunicationManager->getSize();

	myCommunicationManager->configure(configuration);
	mySimulationManagerID = myCommunicationManager->getId();

	ASSERT( myApplication != 0 );
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

	// For threaded simulation types, an objectQueue is allocated
	const TimeWarpSimulationObjectQueueFactory
			*myTimeWarpSimulationObjectQueueFactory =
					TimeWarpSimulationObjectQueueFactory::instance();
	mySimulationObjectQueue
			= dynamic_cast<TimeWarpSimulationObjectQueue *> (myTimeWarpSimulationObjectQueueFactory->allocate(
					configuration, this));
	if (mySimulationObjectQueue != NULL) {
		mySimulationObjectQueue->configure(configuration);
	}

	// configure the event set manager
	const TimeWarpEventSetFactory *myEventSetFactory =
			TimeWarpEventSetFactory::instance();
	Configurable *eventSet = myEventSetFactory->allocate(configuration, this);
	myEventSet = dynamic_cast<TimeWarpEventSet *> (eventSet);
	ASSERT( myEventSet != 0 );
	myEventSet->configure(configuration);

	// lets now set up and configure the state manager
	const StateManagerFactory *myStateFactory = StateManagerFactory::instance();
	myStateManager = dynamic_cast<StateManager *> (myStateFactory->allocate(
			configuration, this));
	ASSERT( myStateManager != 0 );
	myStateManager->configure(configuration);

	// lets now set up and configure the scheduler
	const SchedulingManagerFactory *mySchedulingFactory =
			SchedulingManagerFactory::instance();
	mySchedulingManager
			= dynamic_cast<SchedulingManager *> (mySchedulingFactory->allocate(
					configuration, this));
	ASSERT( mySchedulingManager != 0 );
	mySchedulingManager->configure(configuration);

	// lets now set up and configure the output manager
	const OutputManagerFactory *myOutputManagerFactory =
			OutputManagerFactory::instance();
	myOutputManager
			= dynamic_cast<OutputManager *> (myOutputManagerFactory->allocate(
					configuration, this));
	ASSERT( myOutputManager != 0 );
	myOutputManager->configure(configuration);

	// lets now set up and configure the gvt manager
	const GVTManagerFactory *myGVTFactory = GVTManagerFactory::instance();
	myGVTManager = dynamic_cast<GVTManager *> (myGVTFactory->allocate(
			configuration, this));
	ASSERT( myGVTFactory != 0 );
	myGVTManager->configure(configuration);

	// lets now set up and configure the fossil collection manager
	//  const OptFossilCollManagerFactory *myFossilCollFactory = OptFossilCollManagerFactory::instance();
	//  myFossilCollManager = dynamic_cast<OptFossilCollManager *>(myFossilCollFactory->allocate( configuration, this ));
	//  ASSERT( myFossilCollManager != 0 );
	//  myFossilCollManager->configure( configuration );

	// lets now set up and configure the fossil collection manager
	const OptFossilCollManagerFactory *myFossilCollFactory =
			OptFossilCollManagerFactory::instance();
	myFossilCollManager
			= dynamic_cast<OptFossilCollManager *> (myFossilCollFactory->allocate(
					configuration, this));
	if (myFossilCollManager != 0) {
		myFossilCollManager->configure(configuration);
		usingOptFossilCollection = true;
		//Event::myOptFosColMan = myFossilCollManager;
		//Event::usingOptFossilCollMan = true;
	} else {
		usingOptFossilCollection = false;
	}

	// setup and configure clock frequency manager
	const DVFSManagerFactory* dvfsFactory =
	    DVFSManagerFactory::instance();
	myDVFSManager = dynamic_cast<DVFSManager*>(
	    dvfsFactory->allocate(configuration, this));
	if(myDVFSManager)
      myDVFSManager->configure(configuration);

	registerSimulationObjects();

	ASSERT(myCommunicationManager != NULL);
	// call the communication manager to send out initialization
	// messages and wait for all initialization messages to arrive
	// note: we dont need an initialization message from ourself; and
	// hence the n - 1.
	if (numberOfSimulationManagers > 1) {
    myCommunicationManager->waitForInitialization(
        numberOfSimulationManagers - 1);
	}

  ostringstream oss;
  oss << "lp" << mySimulationManagerID << ".csv";
  ofstream file(oss.str().c_str());
  if(file) {
      if(myDVFSManager)
          file << *myDVFSManager;
      const vector<string>& args = configuration.getArguments();
      vector<string>::const_iterator it(args.begin());
      for(; it != args.end(); ++it)
        file << " " << *it;
      file << endl;
  }

}

bool TimeWarpSimulationManager::contains(const string &object) const {
	SimulationObject *simObject = getObjectHandle(object);
	if (simObject->getObjectID()->getSimulationManagerID()
			== mySimulationManagerID) {
		return true;
	} else {
		return false;
	}
}

bool TimeWarpSimulationManager::contains(const ObjectID &object) const {
	return (object.getSimulationManagerID() == mySimulationManagerID);
}

const VTime &
TimeWarpSimulationManager::getPositiveInfinity() const {
	return myApplication->getPositiveInfinity();
}

const VTime &
TimeWarpSimulationManager::getZero() const {
	return myApplication->getZero();
}

void TimeWarpSimulationManager::shutdown(const string &errorMessage) {
	// We SHOULD send out a message and shut things down nicely - for the moment we're
	// essentially going to crash.
	cerr << errorMessage << endl;
	exit(-1);
}
