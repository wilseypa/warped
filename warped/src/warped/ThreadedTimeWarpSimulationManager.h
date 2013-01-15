#ifndef THREADEThreadedIMEWARPSIMULATIONMANAGER_H_
#define THREADEThreadedIMEWARPSIMULATIONMANAGER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationManager.h"
#include "LockedQueue.h"
#include "WorkerInformation.h"
#include "ThreadedOutputManager.h"
#include "ThreadedStateManager.h"
#include "ThreadedTimeWarpEventSet.h"
#include "ThreadedOptFossilCollManager.h"

class Application;
//class ThreadedSchedulingManager;
class SimulationObject;
class LocalKernelMessage;
class VTime;
class WorkerInformation;
class AtomicState;
class ThreadedOptFossilCollManager;

template<class element> class LockedQueue;
class ThreadedTimeWarpSimulationManager: public TimeWarpSimulationManager {
public:
	ThreadedTimeWarpSimulationManager(unsigned int numberOfWorkerThreads,
			const string syncMechanism, const string scheduleQScheme,
			unsigned int scheduleQCount, Application *initApplication);
	virtual ~ThreadedTimeWarpSimulationManager();
	/** Return a handle to the state manager.

	 @return A handle to the state manager.
	 */
	ThreadedStateManager *getStateManagerNew() {
		return myStateManager;
	}

	/* Return a handle to the Fossil Collection manager.

	 @return A handle to the Fossil Collection manager.
	 */
	ThreadedOptFossilCollManager *getOptFossilCollManagerNew() {
		return myrealFossilCollManager;
	}

	/* Return a handle to the event set factory.

	 @return A handle to the event set factory.
	 */
	ThreadedTimeWarpEventSet *getEventSetManagerNew() {
		return myEventSet;
	}

	/* Return a handle to the output manager.

	 @return A handle to the output manager.
	 */
	ThreadedOutputManager *getOutputManagerNew() {
		return myOutputManager;
	}

	int getNumberofThreads() {
		return numberOfWorkerThreads;
	}

	const string getSyncMechanism() {
		return syncMechanism;
	}

	const string getScheduleQScheme() {
		return scheduleQScheme;
	}

	unsigned int getScheduleQCount() {
		return scheduleQCount;
	}

	/** Create a map of simulation objects.

	 @return An STL hash-map of the simulation objects.
	 */
	typeSimMap *createMapOfObjects();

	/** Return a handle to the scheduling manager.

	 @return A handle to the scheduling manager.
	 */
	SchedulingManager *getSchedulingManager() {
		return mySchedulingManager;
	}

	/**
	 Returns true if the simulation is complete, false otherwise.
	 */
	bool simulationComplete(const VTime &simulateUntil);

	/** Registers a set of simulation objects with this simulation manager.
	 Some tasks this function is responsible for:
	 a. creating and storing the map of simulation object pointers
	 b. assigning unique ids to each simulation object
	 c. setting the simulation manager handle in each object
	 d. configuring the state by calling allocateState on each object */
	void registerSimulationObjects();

	void printObjectMaaping();

	bool checkTermination();

	void updateLVTArray(unsigned int threadId, unsigned int objId);
	void updateLVTArray(unsigned int threadId, VTime* timeToUpdate);

	inline void updateSendMinTime(unsigned int threadId, const VTime* sendTime);

	void decrementLVTFlag(unsigned int threadId);

	// for handling anti-message associated with  a Straggler
	void handleAntiMessageFromStraggler(const Event* stragglerEvent,
			int threadId);

	void getLVTFlagLock(unsigned int threadId);

	void releaseLVTFlagLock(unsigned int threadId);

	void resetComputeLVTStatus();

	void setComputeLVTStatus();

	bool updateLVTfromArray();

	const VTime* getLVT();

	void updateCurrentExec(unsigned int threadId, unsigned int objId);

	void updateCurrentExecFromArray();

	void resetCurrentExecArray();

	const VTime* getMinCurrentExecTime();

	// Checks if this is the simulation manager that initiated the recovery process
	bool getInitiatedRecovery() {
		return initiatedRecovery;
	}

	// Sets the flag to indicate that this is the manager that started recovery
	void setInitiatedRecovery(bool initRec) {
		initiatedRecovery = initRec;
	}

	// Releases all the locks that the workers have on the objects  (Called during a catastrophic rollback)
	void releaseObjectLocksRecovery();

	// Empty the message buffer
	void clearMessageBuffer();

protected:
	/**@name Protected Class Methods of ThreadedTimeWarpSimulationManager. */
	//@{
	//Main simulation function
	void simulate(const VTime& simulateUntil);

	/// Uses pthread_create to create threads
	void createWorkerThreads();

	/// The function called by pthread_create must be static
	static void *startWorkerThread(void *arguments);

	///Function to be executed by threads
	void workerThread(const unsigned int &threadId);

	/// Calls the scheduler to get the next job
	bool executeObjects(const unsigned int &threadId);

	///Used by the worker threads to send their kernel messagee to a messageBuffer
	void sendMessage(KernelMessage *msg, unsigned int destSimMgrId);

	///Called by the manager thread to clear the messageBuffer
	void sendPendingMessages();

	///getEvent for ThreadedMultiset
	const Event *getEvent(SimulationObject *object);

	///Peekevent from ThreadedMultiset
	const Event *peekEvent(SimulationObject *object);
	/** Receive an event.

	 @param event A pointer to the received event.
	 */
public:
	void handleEvent(const Event *event);

	/** call fossil collect on the file queues. This one passes in an integer
	 and should only be used with the optimistic fossil collection manager.

	 @param fossilCollectTime time upto which fossil collect is performed.
	 */
	virtual void fossilCollectFileQueues(SimulationObject *object,
			int fossilCollectTime);

	/// Used in optimistic fossil collection to checkpoint the file queues.
	void saveFileQueuesCheckpoint(std::ofstream* outFile,
			const ObjectID &objId, unsigned int saveTime);

	void restoreFileQueues(ifstream* inFile, const ObjectID &objId,
			unsigned int restoreTime);

	void handleEventReceiver(SimulationObject *currObject, const Event *event,
			int threadID);

	/// Return true when recovering from a catastrophic rollback during
	/// optimimistic fossil collection.
	bool getRecoveringFromCheckpoint() {
		return inRecovery;
	}

	/// Set true when recovering from a catastrophic rollback during
	/// optimimistic fossil collection.
	void setRecoveringFromCheckpoint(bool inRec) {
		inRecovery = inRec;
	}

	bool initiateLocalGVT();

	bool setGVTTokenPending();
	bool resetGVTTokenPending();

	/** call fossil collect on the state, output, input queue, and file queues.

	 @param fossilCollectTime time upto which fossil collect is performed.
	 */
	virtual void fossilCollect(const VTime &fossilCollectTime);

	bool isRollbackJustCompleted(int objId);
	void setRollbackCompletedStatus(int objId);
	void resetRollbackCompletedStatus(int objId);
	void setCheckpointing(bool chkpt) {
		checkpointing = chkpt;
	}

protected:

	void cancelEventsReceiver(SimulationObject *curObject,
			vector<const NegativeEvent *> &cancelObjectIt, int threadID);
	/**
	 Used to route local events.
	 */
	void handleLocalEvent(const Event *event, int threadID);

	/**
	 Used to route remote events.
	 */
	void handleRemoteEvent(const Event *event, int threadID);

	/**
	 Used to cancel local events.
	 */
	void cancelLocalEvents(const vector<const NegativeEvent *> &eventsToCancel,
			int threadID);

	/**
	 Used to cancel remote events.
	 */
	void
	cancelRemoteEvents(const vector<const NegativeEvent *> &eventsToCancel,
			int threadID);
	/**
	 Used to deal with negative events that we've been informed about.
	 */
	void handleNegativeEvents(const vector<const Event*> &negativeEvents,
			int threadID);

	/**
	 The output managers call this method to cancel events when they are
	 rolled back.
	 */
	void
	cancelEvents(const vector<const Event *> &eventsToCancel);

	void rollback(SimulationObject *object, const VTime &rollbackTime,
			int threadID);

	/// call coastforward if an infrequent state saving strategy is used
	void
			coastForward(const VTime &coastForwardFromTime,
					const VTime &rollbackToTime, SimulationObject *object,
					int threadID);

	/*@param msg The message to receive.
	 */
	virtual void receiveKernelMessage(KernelMessage *msg);

	/// initialize the simulation objects before starting the simulation.
	void initialize();

	void configure(SimulationConfiguration &configuration);

	void getGVTTimePeriodLock(int threadId);
	void releaseGVTTimePeriodLock(int threadId);
	/// Register a particular message type with the communication manager.
	virtual void registerWithCommunicationManager();

	/// Returns the current coast forward to time for the given object.
	/// If the object is not coasting forward, NULL is returned.
	const VTime *getCoastForwardTime(const unsigned int &objectID) const;

	void setCoastForwardTime(const unsigned int &objectID,
			const VTime &newCoastForwardTime) {
		ASSERT( coastForwardTime[objectID] == 0);
		coastForwardTime[objectID] = newCoastForwardTime.clone();
	}

	void clearCoastForwardTime(const unsigned int &objectID) {
		ASSERT( coastForwardTime[objectID] != 0);
		delete coastForwardTime[objectID];
		coastForwardTime[objectID] = 0;
	}

private:
	//Handle to the new OptFossilCollManager
	ThreadedOptFossilCollManager *myrealFossilCollManager;
	OptFossilCollManager *myFossilCollManager;

	///This is the handle to the set of events
	ThreadedTimeWarpEventSet *myEventSet;

	//handle for InputEventSet
	ThreadedOutputManager* myOutputManager;

	/// handle to the StateManager Factory
	ThreadedStateManager* myStateManager;

	/// Handle to the scheduler Factory
	SchedulingManager *mySchedulingManager;

	LockedQueue<KernelMessage*> *messageBuffer;

	///Specified in the ThreadControl scope of the configuration file
	unsigned int numberOfWorkerThreads;

	//Specfied in the ThreadControl scope the configuraion file
	const string syncMechanism;

	//Specfiy the Scheduler scope of the configuraion file
	const string scheduleQScheme;

	//Specified in the Scheduler scope of the configuration file
	unsigned int scheduleQCount;

public:
	///Holds information each thread needs to operate
	WorkerInformation **workerStatus;
private:
	/// Time up to which coast forwarding should be done.
	vector<const VTime *> coastForwardTime;

	/// Used to determine when the optimistic fossil collection manager is
	/// recovery from a catastrophic rollback.
	bool inRecovery;

	/// Put all arguments in one object to be passed to StartThread as void*
	class thread_args {
	public:
		thread_args(ThreadedTimeWarpSimulationManager *simManager,
				int threadIndex) :
			simManager(simManager), threadIndex(threadIndex) {
		}
		ThreadedTimeWarpSimulationManager *simManager;
		unsigned int threadIndex;
	};

	unsigned int idThread;

	//This flag is set when it is time for the GVT to be recalculated
	bool checkGVT;

	AtomicState* GVTTimePeriodLock;

	unsigned int masterID;

	//Flag for initiating the LVT Calculation between threads
	unsigned int LVTFlag;

	AtomicState* LVTFlagLock;

	const VTime** LVTArray;

	const VTime** sendMinTimeArray;

	//unsigned int** logTwice;

	bool** computeLVTStatus;

	const VTime* LVT;

	bool GVTTokenPending;

	bool* rollbackCompleted;

	// Checks if this simulation manager initiated the recovery process
	bool initiatedRecovery;

	int numCatastrophicRollbacks;

	bool checkpointing;

	int lvtCount;

	unsigned int numberOfNegativeEventMessage;

	unsigned int numberOfRemoteAntimessages;

	unsigned int numberOfLocalAntimessages;
};

#endif /* ThreadedTIMEWARPSIMULATIONMANAGER_H_ */
