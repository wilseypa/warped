// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpSimulationManager.h"
#include "DefaultSchedulingManager.h"
#include "StopWatch.h"
#include "ObjectStub.h"
#include "SimulationObjectProxy.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSimulationStream.h"
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
#include "LocalKernelMessage.h"
#include "LocalEventMessage.h"
#include "LocalFossilCollectMessage.h"
#include "LocalNegativeEventMessage.h"
#include "ThreadedTimeWarpEventSet.h"
#include "AtomicMatternGVTManager.h"
#include "AtomicSimulationObjectState.h"
#include "VTime.h"
#include "OptFossilCollManager.h"
#include "WorkerInformation.h"
#include "LockedQueue.h"
#include "NegativeEvent.h"
#include <string>
#include <fstream>

#include <pthread.h>

#include <utils/Debug.h>
using utils::debug;

//Define the static stilBusyCount outside of the class, there is only one per simulation manager
int WorkerInformation::globalStillBusyCount = 0;
bool WorkerInformation::workRemaining = true;
static const unsigned int SIMULATION_MANAGER_THREADID = 0;

ThreadedTimeWarpSimulationManager::ThreadedTimeWarpSimulationManager( unsigned int numberOfWorkerThreads,
																	  Application *initApplication )
	: myThreadedEventSet(NULL),
	  workerStatus(new WorkerInformation*[numberOfWorkerThreads+1]),
	  numberOfWorkerThreads(numberOfWorkerThreads),
	  outputMgrsCleaned(NULL),
	  outputCleanersSent(false),
	  checkGVT(false),
	  emptyAllQueues(true),
	  outgoingMessages(new LockedQueue<KernelMessage*>),
      TimeWarpSimulationManager(initApplication)
{
	pthread_spin_init(&jobLock, NULL);
}

ThreadedTimeWarpSimulationManager::~ThreadedTimeWarpSimulationManager() {
	delete []workerStatus;
	delete outgoingMessages;
}


void ThreadedTimeWarpSimulationManager::simulate( const VTime& simulateUntil )
{
	ASSERT(dynamic_cast<AtomicMatternGVTManager*>(myGVTManager)!=0);
	ASSERT(myThreadedEventSet!=0);
	//cout << "SimulationManager(" << mySimulationManagerID << "): Starting Worker Threads" << endl;
	createWorkerThreads();
	//****************************Simulation Manager Routine***************************************
	StopWatch stopwatch;
	stopwatch.start();
	bool managerDidWork = true;
	cout << "SimulationManager(" << mySimulationManagerID
	   << "): Starting simulation - End time: " << simulateUntil << ")" << endl;
	//Start Worker Threads
	while( !simulationComplete( simulateUntil ) ){
		getMessages();
		//check the GVT flag
		if(checkGVT && mySimulationManagerID==0){
			//This if statement is only ever true for simulation manager 0
			//Dont allow any of the workers to remove items from the calendar queue until finished
			myGVTManager->calculateGVT();
			//reset the checkGVT flag
			bool checkGVTFlagIsOff = __sync_bool_compare_and_swap(&checkGVT,true,false);
			ASSERT(checkGVTFlagIsOff);
		}
		//Perform  some real work if configured to do so
	    //This maybe should loop here if
		//the simulation manager is not very busy
		managerDidWork = executeObjects(SIMULATION_MANAGER_THREADID);
	    sendPendingMessages();
	
		//If Messages are accumulating make sure all threads are active
		if (myThreadedEventSet->getMessageCount()>0) {
			if (WorkerInformation::getStillBusyCount() < numberOfWorkerThreads) {
				for (unsigned int threadIndex=0; threadIndex<numberOfWorkerThreads; threadIndex++) {
					workerStatus[threadIndex]->resume();
				}
			}
		}
		//The simulation stays active while the threads are busy or while the outputMgrs are still not clean
		if (managerDidWork || WorkerInformation::getStillBusyCount() > 0) {
			myTerminationManager->setStatusActive();
		}
		//Send out output manager clean-up messages to all objects
		//before allowing the simulation to end
		else if (!outputCleanersSent) {
			myTerminationManager->setStatusActive();
			__sync_bool_compare_and_swap(&outputCleanersSent, false, true);
			__sync_fetch_and_add(&outputMgrsCleaned,numberOfObjects);
			LocalKernelMessage *outMgrCleanupMsg=0;
			for (unsigned int objID = 0; objID<numberOfObjects; objID++) {
				outMgrCleanupMsg = new LocalKernelMessage(LKM_OUTPUT_CLEANUP, localArrayOfSimObjIDs[objID]);
				myThreadedEventSet->insertLocalMessage(outMgrCleanupMsg);
			}
		}
		else if(numberOfSimulationManagers==1) {
			//simulation complete only after the last worker performs output queue cleaning
			dynamic_cast<SingleTerminationManager*>(myTerminationManager)->simulationComplete();
		}
		else{
			myTerminationManager->setStatusPassive();
		}
	}
	stopwatch.stop();
	cout << "(" << getSimulationManagerID() << ") Threaded Simulation complete (" <<
			stopwatch.elapsed() << " secs)" << endl;
  cout << "Rollback Count: " << myThreadedEventSet->rollbackCounter << endl;
  //****************************End of Simulation Manager Routine********************************
  WorkerInformation::killWorkerThreads();
  //We are the simulationManager thread so that does not need to be joined with
  for (unsigned int threadIndex=1; threadIndex < numberOfWorkerThreads; threadIndex++) {
    debug << "SimulationManager(" << mySimulationManagerID << "): Stopping Worker Thread: " << threadIndex << endl;
    //Verify the thread is not suspended
    workerStatus[threadIndex]->resume();
    //Rejoin with the worker upon its exit
    pthread_join(*(workerStatus[threadIndex])->getThread(), NULL);
  }
  debug << "SimulationManager(" << mySimulationManagerID << "): All threads have been joined." << endl;
  //If we have to terminate the simulation because GVT reaches positive infinity
	//we can ignore the rest of the messages in the message queue
  if (myGVTManager->getGVT() >= simulateUntil) {
		LocalKernelMessage *toBeDeleted=NULL;
    while ((toBeDeleted = myThreadedEventSet->getNextObjectForMessageProc(SIMULATION_MANAGER_THREADID))!=NULL) {
      //If the same object has two or more remaining messages, we will need the lock back
      myThreadedEventSet->releaseObject(SIMULATION_MANAGER_THREADID,getObjectHandle(toBeDeleted->getObjectID()));
      delete toBeDeleted;
		}
	} else {
		ASSERT(myThreadedEventSet->getMessageCount()==0);
	}
}

void ThreadedTimeWarpSimulationManager::processOnlyMessages(){
	__sync_bool_compare_and_swap(&emptyAllQueues, true, false);
	//The compare and swap will fail for simulation manager 0 because emptyAllQueues is already off
	ASSERT(!emptyAllQueues);
	bool managerDidWork=true;
	//Wait here for all workers to finish emptying the message queue
	while (WorkerInformation::getStillBusyCount() > 0 || managerDidWork) {
		//Do the worker job until all messages are gone
		managerDidWork=executeObjects(SIMULATION_MANAGER_THREADID);
		pthread_yield();
	}
	//It should not possible for any new messages to have been created
	//because all the queues were just cleared
	ASSERT(myThreadedEventSet->getMessageCount()==0);
}

void ThreadedTimeWarpSimulationManager::processAllQueues(){
	bool emptyAllQueuesIsOn=__sync_bool_compare_and_swap(&emptyAllQueues, false, true);
	ASSERT(emptyAllQueuesIsOn);
}

const VTime &ThreadedTimeWarpSimulationManager::getEarliestOutputMessage() {
	const VTime *earliestTime = &getPositiveInfinity();
	if (getOutputMgrType()== LAZYMGR ||	getOutputMgrType() == ADAPTIVEMGR) {
		LazyOutputManager *lmgr = dynamic_cast<LazyOutputManager*>(myOutputManager);
		earliestTime = &lmgr->getLazyQMinTime();
	}
	return *earliestTime;
}

void ThreadedTimeWarpSimulationManager::workerThread( const unsigned int &threadID ) {
	ASSERT(myThreadedEventSet!=0);
	debug << "(" << mySimulationManagerID << "-" << threadID << ")" << " Thread is Starting" << endl;
	//The manager will unset this flag when this worker should terminate
	while(WorkerInformation::isWorkRemaining()) {
		//If no work was done and we had access to all of the queues
		//increment the idle count
		if (executeObjects(threadID)) {
			workerStatus[threadID]->resetIdleCount();
		}
		else
		{
			workerStatus[threadID]->incrementIdleCount();
			pthread_yield();
		}
	}
	debug << "(" << mySimulationManagerID << "-" << threadID << ")" << " Thread is Ending" << endl;
	pthread_exit(0);
}

/// This function is to be 100% threadsafe as it is called by any number of workerThreads
/// Thread safe means only one object is checked out per thread at any one time
/// and that object only interacts with local data or atomic calls to global data structures
bool ThreadedTimeWarpSimulationManager::executeObjects(const unsigned int &threadID) {
	bool IdidWork=true;
	LocalKernelMessage *nextMessage = NULL;
	SimulationObject *lockedObject = NULL;
	//Attempt to clear the message queue
	if ((nextMessage = myThreadedEventSet->getNextObjectForMessageProc(threadID)) != NULL)	{
		//The EventSet has already locked the object for us
		lockedObject = getObjectHandle(nextMessage->getObjectID());
		//verify this simulation object is a local object
		ASSERT(lockedObject->getObjectID()->getSimulationManagerID() == mySimulationManagerID);
		processLocalMessage(nextMessage);
		//Put object back into queue if there are still unprocessed events
		//If the reinsert fails then the that means the calendar queue has
		//passed this objects position, so we should immediately move to processing events
		while (!myThreadedEventSet->releaseObject(threadID, lockedObject)) {
			lockedObject->executeProcess();
		}
	}
	//If no messages exist and not in the middle of checking GVT try to empty the object queue
	else if (emptyAllQueues) {
		if ((lockedObject = myThreadedEventSet->getNextObjectForEventProc(threadID)) != NULL) {
			//verify this simulation object is a local object
			ASSERT(lockedObject->getObjectID()->getSimulationManagerID() == mySimulationManagerID);
			//The EventSet has already locked the object for us
			//It is possible between the time we got the object from the queue and the time
			//we got the lock that the last event for this object was processed somewhere else
			const Event *nextEvent;
			if ((nextEvent = peekEvent(lockedObject)) != NULL)	{
				if (outputCleanersSent)	{
					__sync_bool_compare_and_swap(&outputCleanersSent, true, false);
				}
				if(usingOptFossilCollection){
					myFossilCollManager->checkpoint(nextEvent->getReceiveTime(), *lockedObject->getObjectID());
					myFossilCollManager->fossilCollect(lockedObject, nextEvent->getReceiveTime());
				}
				//update the simulation time
				lockedObject->setSimulationTime( nextEvent->getReceiveTime() );
				//update the state manager for this object
				myStateManager->saveState( nextEvent->getReceiveTime(), lockedObject );
				//Execute the next Event for this object
				lockedObject->executeProcess();
				//This should be an atomic GVT manager
                if (mySimulationManagerID==0 && !checkGVT) {
                    pthread_spin_lock(&jobLock);
   					if (myGVTManager->checkGVTPeriod())	{
						//Turn off the object queue so nothing else can be removed
						bool emptyAllQueuesIsOff=__sync_bool_compare_and_swap(&emptyAllQueues, true, false);
						ASSERT(emptyAllQueuesIsOff);
						//Set the GVT flag so the Manager thread can calculate GVT
						bool checkGVTOn = __sync_bool_compare_and_swap(&checkGVT,false,true);
						ASSERT(checkGVTOn);
					}
                    pthread_spin_unlock(&jobLock);
				}
			}
			//Put object back into queue if there are still unprocessed events
			while (!myThreadedEventSet->releaseObject(threadID, lockedObject)) {
				//Execute the next Event for this object
				lockedObject->executeProcess();
			}
		} else {
			IdidWork=false;
		}
	}
	else
	{
		IdidWork=false;
	}
	return IdidWork;
}

//For this function the thread should only be dealing with the sender object,
//and inserting the event into the message queue
void
ThreadedTimeWarpSimulationManager::handleEventReceiver( SimulationObject *curObject, const Event *event ){
	if( contains( curObject->getName() ) ){
       LocalEventMessage *newEventMessage = new LocalEventMessage(event, LKM_EVENT, curObject);
       ASSERT(myThreadedEventSet!=NULL);
       myThreadedEventSet->insertLocalMessage(newEventMessage);
    }
    else {
       handleRemoteEvent( event );
    }
}

void
ThreadedTimeWarpSimulationManager::cancelEventsReceiver( SimulationObject *curObject,
		vector<const NegativeEvent *> &eventsToCancel ) {
	if( contains( curObject->getName() ) ){
		LocalNegativeEventMessage *newNegEventMessage =
				new LocalNegativeEventMessage(eventsToCancel, LKM_NEGATIVE_EVENT, curObject);
		ASSERT(myThreadedEventSet!=NULL);
		myThreadedEventSet->insertLocalMessage(newNegEventMessage);
	}
	else{
		cancelRemoteEvents( eventsToCancel );
	}
}

inline void
ThreadedTimeWarpSimulationManager::sendMessage( KernelMessage *msg, unsigned int destSimMgrId) {
	outgoingMessages->enqueue(msg);
}

inline void
ThreadedTimeWarpSimulationManager::sendPendingMessages() {
	KernelMessage *messageToBeSent = NULL;
	while ((messageToBeSent=outgoingMessages->dequeue()) != NULL) {
		myCommunicationManager->sendMessage(messageToBeSent, messageToBeSent->getReceiver());
	}
}

//If we made it this far, the receiver object this message is involved with is locked just for us
void ThreadedTimeWarpSimulationManager::processLocalMessage( LocalKernelMessage *receivedMessage)
{
	switch (receivedMessage->getMessageType()) {
		case LKM_EVENT:	{
			processLocalEventMessage(receivedMessage);
			break;
		}
		case LKM_NEGATIVE_EVENT: {
			processLocalNegativeEventMessage(receivedMessage);
			break;
		}
		case LKM_OUTPUT_CLEANUP: {
			processLocalOutputCleanUpMessage(receivedMessage);
			break;
		}
		case LKM_FOSSIL_COLLECT_STATE: {
			processFossilCollectStateMessage(receivedMessage);
			break;
		}
		case LKM_FOSSIL_COLLECT_OUTPUT:	{
			processFossilCollectOutputMessage(receivedMessage);
			break;
		}
		case LKM_FOSSIL_COLLECT_EVENTSET: {
			processFossilCollectEventSetMessage(receivedMessage);
			break;
		}
		default: {
			cerr << "ProcessLocalMessage found unknown local message type: " <<
					receivedMessage->getMessageType() << " for object " <<
					receivedMessage->getObjectID() << endl;
			cerr << "Aborting simulation ..." << endl;
			delete receivedMessage;
			abort();
		}
	}
}

void ThreadedTimeWarpSimulationManager::processLocalEventMessage(LocalKernelMessage *receivedMessage) {
	LocalEventMessage *localMessage = dynamic_cast< LocalEventMessage *>(receivedMessage);
	ASSERT(localMessage!=NULL);
	handleLocalEvent( localMessage->getEvent() );
	delete localMessage;
}

void ThreadedTimeWarpSimulationManager::processLocalNegativeEventMessage(LocalKernelMessage *receivedMessage) {
	LocalNegativeEventMessage *negativeMessage = dynamic_cast<LocalNegativeEventMessage *>(receivedMessage);
	ASSERT(negativeMessage!=NULL);
	//Get a handle to the object
	SimulationObject *lockedObject = getObjectHandle(negativeMessage->getObjectID());
	//Process the negative message
	cancelLocalEvents( negativeMessage->getNegativeEvents() );
	delete negativeMessage;
}


void ThreadedTimeWarpSimulationManager::processLocalOutputCleanUpMessage(LocalKernelMessage *receivedMessage) {
	// This is done when it is certain there are no more events to process and as
	// a result, no way to regenerate the same events that remain to be checked.
	if(outMgrType == LAZYMGR || outMgrType == ADAPTIVEMGR){
		LazyOutputManager *myLazyOutputManager = dynamic_cast<LazyOutputManager *>(myOutputManager);
		ASSERT(myLazyOutputManager != NULL);
		myLazyOutputManager->emptyLazyQueues(getPositiveInfinity());
	}
	//Increment the counter for every output manager that is cleaned
	__sync_fetch_and_sub(&outputMgrsCleaned,1);
	delete receivedMessage;
}

void ThreadedTimeWarpSimulationManager::fossilCollect(const VTime &gVT) {
	if(!usingOptFossilCollection){
		const VTime *fossilCollectTime = gVT.clone();
		//The first round message which determine the real minTime
		const VTime *minTime = getPositiveInfinity().clone();
		//Create a new round 1 fossil collection message
		LocalFossilCollectMessage *outFossilMsg =
				new LocalFossilCollectMessage(minTime, fossilCollectTime, LKM_FOSSIL_COLLECT_STATE, localArrayOfSimObjIDs[0]);
		//Sending this message will cause the simulation to go active again
		myThreadedEventSet->insertLocalMessage(outFossilMsg);
	}
}

void ThreadedTimeWarpSimulationManager::processFossilCollectStateMessage(LocalKernelMessage *receivedMessage) {
	//Cast the message to the correct type
	LocalFossilCollectMessage *localMessage =
		dynamic_cast<LocalFossilCollectMessage *>(receivedMessage);
	ASSERT(localMessage!=0);
	//Fossil collect phase 1 and record the mintime from the stateManager
	const VTime *minTime = fossilCollectState(
					getObjectHandle(localMessage->getObjectID()),
					*localMessage->getGVTTime() );
	//If the minTime is less than the one from the local message then
	//use it, else use the messageTime
	if (*minTime <= *(localMessage->getMinTime()))
	{}
	else
	{
		delete minTime;
		minTime = localMessage->getMinTime()->clone();
	}
	unsigned int nextObjectID = receivedMessage->getObjectID().getSimulationObjectID()+1;
	//If there are more objects remaining send this message to the next one
	//Else start a phase 2 message with objectID=0
	if (nextObjectID<getNumberOfSimulationObjects())
	{
		const VTime *gvtTime = localMessage->getGVTTime()->clone();
		LocalFossilCollectMessage *outMgrFossilMsg1 =
				new LocalFossilCollectMessage(minTime, gvtTime, LKM_FOSSIL_COLLECT_STATE, localArrayOfSimObjIDs[nextObjectID]);
		myThreadedEventSet->insertLocalMessage(outMgrFossilMsg1);
	}
	else
	{
		nextObjectID=0;
		LocalFossilCollectMessage *outMgrFossilMsg2 =
				new LocalFossilCollectMessage(minTime, 0, LKM_FOSSIL_COLLECT_OUTPUT, localArrayOfSimObjIDs[nextObjectID]);
		myThreadedEventSet->insertLocalMessage(outMgrFossilMsg2);
	}
	delete localMessage;
}

void ThreadedTimeWarpSimulationManager::processFossilCollectOutputMessage(LocalKernelMessage *receivedMessage)
{
	//Cast the message to the correct type
	LocalFossilCollectMessage *localMessage =
		dynamic_cast<LocalFossilCollectMessage *>(receivedMessage);
	ASSERT(localMessage!=0);
	myOutputManager->fossilCollect( getObjectHandle(localMessage->getObjectID()),
										*localMessage->getMinTime());
	//Perform phase 2 Fossil collect with the known minimum time from the first phase
	unsigned int nextObjectID = localMessage->getObjectID().getSimulationObjectID()+1;
	//If there are more objects pass this message to the next one
	if (nextObjectID<getNumberOfSimulationObjects())
	{
		const VTime *minTime = localMessage->getMinTime()->clone();
		LocalFossilCollectMessage *outMgrFossilMsg1 =
				new LocalFossilCollectMessage(minTime, NULL,
											  LKM_FOSSIL_COLLECT_OUTPUT,
											  localArrayOfSimObjIDs[nextObjectID]);
		myThreadedEventSet->insertLocalMessage(outMgrFossilMsg1);
	}
	else
	{
		//Start the next round of messages with object[0]
		const VTime *minTime = localMessage->getMinTime()->clone();
		LocalFossilCollectMessage *outMgrFossilMsg1 =
				new LocalFossilCollectMessage(minTime, NULL,
										      LKM_FOSSIL_COLLECT_EVENTSET,
										      localArrayOfSimObjIDs[0]);
		myThreadedEventSet->insertLocalMessage(outMgrFossilMsg1);
	}
	delete localMessage;
}

void ThreadedTimeWarpSimulationManager::processFossilCollectEventSetMessage(LocalKernelMessage *receivedMessage)
{
	//Cast the message to the correct type
	LocalFossilCollectMessage *localMessage =
		dynamic_cast<LocalFossilCollectMessage *>(receivedMessage);
	ASSERT(localMessage!=0);
	myEventSet->fossilCollect( getObjectHandle(localMessage->getObjectID()),
										*localMessage->getMinTime());
	//Perform phase 2 Fossil collect with the known minimum time from the first phase
	unsigned int nextObjectID = localMessage->getObjectID().getSimulationObjectID()+1;
	//If there are more objects pass this message to the next one
	if (nextObjectID<getNumberOfSimulationObjects())
	{
		const VTime *minTime = localMessage->getMinTime()->clone();
		LocalFossilCollectMessage *outMgrFossilMsg1 =
				new LocalFossilCollectMessage(minTime, NULL,
											  LKM_FOSSIL_COLLECT_EVENTSET,
											  localArrayOfSimObjIDs[nextObjectID]);
		myThreadedEventSet->insertLocalMessage(outMgrFossilMsg1);
	}
	delete localMessage;
}

void ThreadedTimeWarpSimulationManager::receiveKernelMessage(KernelMessage *msg){
  myTerminationManager->setStatusActive();
  ASSERT(msg != NULL);
  ASSERT(myThreadedEventSet!=0);
  if( dynamic_cast<EventMessage *>(msg) != NULL ){
    EventMessage *eventMsg = dynamic_cast<EventMessage *>(msg);
    const Event *event = eventMsg->getEvent();
    //Updated the white message cound
    myGVTManager->updateEventRecord( eventMsg->getGVTInfo(),mySimulationManagerID );
    SimulationObject *object = getObjectHandle(event->getReceiver());
    //Package up the event message and insert it into the receive queue to be processed
    LocalEventMessage *localMessage = new LocalEventMessage(eventMsg->getEvent(), LKM_EVENT, object);
    //Insert the message into the queue
    myThreadedEventSet->insertLocalMessage(localMessage);
  }
  else if( dynamic_cast<NegativeEventMessage *>(msg) != NULL ){
    NegativeEventMessage *negEventMsg = dynamic_cast<NegativeEventMessage *>(msg);
    //Note this is actually copying the data because the message is being destroyed
    ASSERT(!negEventMsg->getEvents().empty());
    //Updated the white message cound
    myGVTManager->updateEventRecord( negEventMsg->getGVTInfo(), mySimulationManagerID );
	//We only have to check the receiver of the first event because this group of events was set as a package
	//One receiver object per package
	SimulationObject *object = getObjectHandle(negEventMsg->getEvents()[0]->getReceiver());
	//Package up the event message and insert it into the receive queue to be processed
	LocalNegativeEventMessage *localMessage =
			new LocalNegativeEventMessage(negEventMsg->getEvents(), LKM_NEGATIVE_EVENT, object);
	//Insert the message into the queue
	myThreadedEventSet->insertLocalMessage(localMessage);
  }
  //All remaining kernel messages are the same as the original simulation manager
  else if( dynamic_cast<InitializationMessage *>(msg) != NULL ){
    InitializationMessage *initMsg = dynamic_cast<InitializationMessage *>(msg);
    registerSimulationObjectProxies( &initMsg->getObjectNames(),
				     initMsg->getReceiver(),
				     initMsg->getSender());
  }
  else if( dynamic_cast<StartMessage *>(msg) != NULL ){
    utils::debug << "SimulationManager(" << mySimulationManagerID
		   << "): Starting Simulation" << endl;
  }
  else if( msg->getDataType() == "AbortSimulationMessage" ){
    cerr << "TimeWarpSimulationManager is going to abort simulation"
	 << endl;
  }
  else {
    cerr << "TimeWarpSimulationManager::receiveKernelMessage() received"
	 << " unknown (" << msg->getDataType() << ") message type" << endl;
    cerr << "Aborting simulation ..." << endl;
    abort();
  }
  // we are done with this kernel message
  delete msg;
}

const VTime *ThreadedTimeWarpSimulationManager::fossilCollectState(SimulationObject *object,
	const VTime& fossilCollectTime) {
	const VTime *minTime = (myStateManager->fossilCollect(object, fossilCollectTime )).clone();
	// call fossil collect on the file queues
	if( fossilCollectTime != getPositiveInfinity() ){
                unsigned int objID = object->getObjectID()->getSimulationObjectID();		
		if(!inFileQueues[objID].empty()){
			vector<TimeWarpSimulationStream*>::iterator iter =
					inFileQueues[objID].begin();
			vector<TimeWarpSimulationStream*>::iterator iter_end =
					inFileQueues[objID].end();
			while(iter != iter_end){
				(*iter)->fossilCollect(*minTime);
				++iter;
			}
		}
		if(!outFileQueues[objID].empty()){
			vector<TimeWarpSimulationStream*>::iterator iter =
			outFileQueues[objID].begin();
			vector<TimeWarpSimulationStream*>::iterator iter_end =
			outFileQueues[objID].end();
			while(iter != iter_end){
				(*iter)->fossilCollect(*minTime);
				++iter;
			}
		}
	}
	return minTime;
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
	for (unsigned int threadIndex=1; threadIndex<numberOfWorkerThreads; threadIndex++)
	{
		//Allocate some memory for a new thread
		pthread = new pthread_t;
		//Define worker status for this thread number including the threadId and numerical threadNumber
		workerStatus[threadIndex] = new WorkerInformation(pthread);
		//Each workerThread gets a pointer to this simulation manager and the index to its status information
		threadArguments = new thread_args(this, threadIndex);
		//Create the new worker thread
		retVal = pthread_create( pthread, attributes, startWorkerThread, threadArguments);
		if (retVal != 0)
		{
			cout << "pthread_create failed with error code: " << retVal << endl;
			cout << "Aborting!!!" << endl;
			abort();
		}
	}
}

void *ThreadedTimeWarpSimulationManager::startWorkerThread(void *arguments)
{
	//Convert the arguments from void* back to thread_args
	thread_args* myArgs = static_cast<thread_args*>(arguments);
	//Start executing objects
	myArgs->simManager->workerThread(myArgs->threadIndex);
}
