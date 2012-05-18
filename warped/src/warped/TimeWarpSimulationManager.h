#ifndef TIMEWARP_SIMULATION_MANAGER_H
#define TIMEWARP_SIMULATION_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "StopWatch.h"
#include "warped.h"
#include "SimulationStream.h"
#include "CommunicatingEntity.h"
#include "TimeWarpConfigurationManager.h"
#include "SimulationManagerImplementationBase.h"
#include "EventId.h"
#include "NegativeEvent.h"
#include <map>
#include <fstream>
#include <time.h>

class Application;
class CommunicationManager;
class GVTManager;
class OptFossilCollManager;
class DVFSManager;
class OutputManager;
class SchedulingData;
class SchedulingManager;
class SimulationObjectProxy;
class StateManager;
class TerminationManager;
class TimeWarpSimulationStream;
class TimeWarpEventSet;
class TimeWarpSimulationObjectQueue;

/** The TimeWarpSimulationManager class.

 The TimeWarpSimulationManager class implements a simulation
 manager that performs a TimeWarp synchronized parallel
 discrete-event simulation. Since simulation managers need to
 communicate, the TimeWarpSimulationManager is also derived from
 the CommunicatingEntity abstract base class.

 */
class TimeWarpSimulationManager: public SimulationManagerImplementationBase,
		public CommunicatingEntity {
public:

	/**@type friend class declarations */
	//@{

	/** Builder class */
	friend class TimeWarpEventSetFactory;
	friend class EventId;

	//@} // End of friend class declarations

	/**@name Public Class Methods of TimeWarpSimulationManager. */
	//@{

	/** Constructor.

	 @param numProcessors Number of processors requested
	 @param initApplication The application that we're going to start up.

	 */
    TimeWarpSimulationManager(Application *initApplication);

	/// Destructor.
	virtual ~TimeWarpSimulationManager();

	/// initialize the simulation objects before starting the simulation.
	virtual void initialize(); //made it virtual to support the new Dynamic Threaded Simulation(Karthikeyan Muthalagu)

	/** Run the simulation.

	 @param simulateUntil Time upto which to simulate.
	 */
	virtual void simulate(const VTime &simulateUntil);

	/// Clean up after the simulation has finished.
	void finalize();

	/** Registers a set of simulation objects with this simulation manager.
	 Some tasks this function is responsible for:
	 a. creating and storing the map of simulation object pointers
	 b. assigning unique ids to each simulation object
	 c. setting the simulation manager handle in each object
	 d. configuring the state by calling allocateState on each object */
	void registerSimulationObjects();

	/** Construct array of local object names to send out to other Sim-Mgrs.

	 @return The array of local object names.
	 */
	virtual vector<string> *getSimulationObjectNames();

	/** Add these simulation object proxies to the total array of sim objects.

	 The proxies are used for simulation objects that are handled
	 by another simulation manager.

	 @param arrayOfObjectProxies The set of sim-object proxies.
	 @param sourceSimulationManagerID The owning sim-manager.
	 @param destSimulationManagerID The sim-manager who has the proxy.
	 */
	virtual void registerSimulationObjectProxies(
			const vector<string> *arrayOfObjectProxies,
			unsigned int sourceSimulationManagerID,
			unsigned int destSimulationManagerID);

	/** Remove the next event in the event set and return it.

	 @param object The simulation object whose event set we get event from.
	 @return A reference to the removed event.
	 */
	const Event *getEvent(SimulationObject *object);

	/** Return a reference to the next event in the event set.

	 @param object The simulation object whose event set we get event from.
	 @return A reference to the next event in the event set.
	 */
	const Event *peekEvent(SimulationObject *object);

	/** Receive an event.

	 @param event A pointer to the received event.
	 */
	virtual void handleEvent(const Event *event);
	virtual void handleEventReceiver(SimulationObject *currObject,
			const Event *event);

	/**
	 The output managers call this method to cancel events when they are
	 rolled back.
	 */
	virtual void cancelEvents(const vector<const Event *> &eventsToCancel);
	virtual void cancelEventsReceiver(SimulationObject *curObject,
			vector<const NegativeEvent *> &cancelObjectIt);
	/** Return a handle to the communication manager.

	 @return A handle to the communication manager.
	 */
	virtual CommunicationManager *getCommunicationManager() const {
		return myCommunicationManager;
	}

	/** Return a handle to the scheduling manager.

	 @return A handle to the scheduling manager.
	 */
	virtual SchedulingManager *getSchedulingManager() {
		return mySchedulingManager;
	}

	/** Return a handle to the simulation object queue.

	 @return A handle to the simulation object queue.
	 */
	virtual TimeWarpSimulationObjectQueue *getSimulationObjectQueue() {
		return mySimulationObjectQueue;
	}

	/** Return a handle to the gvt manager.

	 @return A handle to the gvt manager.
	 */
	virtual GVTManager *getGVTManager() {
		return myGVTManager;
	}

	/** Return a handle to the termination manager.

	 @return A handle to the termination manager.
	 */
	virtual TerminationManager *getTerminationManager() {
		return myTerminationManager;
	}

	/** Return a handle to the state manager.

	 @return A handle to the state manager.
	 */
	virtual StateManager *getStateManager() {
		return myStateManager;
	}

	/** Return a handle to the event set factory.

	 @return A handle to the event set factory.
	 */
	virtual TimeWarpEventSet *getEventSetManager() {
		return myEventSet;
	}

	/** Return a handle to the output manager.

	 @return A handle to the output manager.
	 */
	virtual OutputManager *getOutputManager() {
		return myOutputManager;
	}

	/** Return a handle to the Scheduling Data

	 @return A handle to SchedulingData in Decentralized EventSet.
	 */
	SchedulingData* getSchedulingData() {
		return mySchedulingData;
	}

	virtual OptFossilCollManager *getFossilCollManager() {
		return myFossilCollManager;
	}

	/** Returns a simulation object pointer.

	 @param object The string representation of the object.
	 @return A pointer to the object corresponding to the string rep.
	 */
	virtual SimulationObject *getObjectHandle(const string &object) const {
		typeSimMap::const_iterator it = globalArrayOfSimObjPtrs.find(object);
		if (it == globalArrayOfSimObjPtrs.end())
			return 0;
		return it->second;
	}

	virtual OBJECT_ID &getObjectId(const string &objectName) {
		return *getObjectHandle(objectName)->getObjectID();
	}

	/*
	 Returns true if this simulation manager contains this object, false if
	 it's non-local.
	 */
	virtual bool contains(const string &object) const;
	virtual bool contains(const ObjectID &objId) const;

	/** Returns a simulation object pointer.

	 @param objectID The object Id of the simulation object.
	 @return A pointer to the object with the given object Id.
	 */
	virtual SimulationObject *getObjectHandle(const OBJECT_ID &objectID) const {
		return globalArrayOfSimObjIDs[objectID.getSimulationManagerID()][objectID.getSimulationObjectID()];
	}

	/// Register a particular message type with the communication manager.
	virtual void registerWithCommunicationManager();

	/** Method the communication manager calls to deliver a kernel message.

	 @param msg The message to receive.
	 */
	virtual void receiveKernelMessage(KernelMessage *msg);

	/** Return the simulation manager's id.

	 @return The Id of the simulation manager.
	 */
	virtual unsigned int getSimulationManagerID() const {
		return mySimulationManagerID;
	}

	/** Get the current simulation time.
	 @return The current simulation time.
	 */
	virtual const VTime &getSimulationTime() const {
		std::cerr
				<< "Error SimulationManager wide simulationTime is no longer used."
				<< std::endl;
		abort();
	}

	virtual const VTime &getCommittedTime();

	virtual const VTime &getNextEventTime();

	/** Check if this simulation manager has work to do or not.

	 @return Idle status (true/false).
	 */
	virtual bool checkIdleStatus();

	/// check if message aggregation is enabled ...
	virtual bool checkMessageAggregationFlag();

	/// set the message aggregation flag
	virtual void setMessageAggregationFlag(bool flag);

	/// get a handle to a simulation input stream
	virtual SimulationStream *getIFStream(const string &filename,
			SimulationObject *object);

	/// get a handle to a simulation output stream
	virtual SimulationStream *getOFStream(const string &filename,
			SimulationObject *object, ios::openmode mode);

	/// get a handle to a simulation input-output stream
	virtual SimulationStream *getIOFStream(const string &filename,
			SimulationObject *object);

	void configure(SimulationConfiguration &configuration);

	const VTime &getPositiveInfinity() const;
	const VTime &getZero() const;

	void shutdown(const string &errorMessage);

	/// Set the output manager type flag. Can be AGGRMGR, LAZYMGR, ADAPTIVEMGR.
	void setOutputMgrType(OutputMgrType type) {
		outMgrType = type;
	}

	/// Get the output manager type flag.
	OutputMgrType getOutputMgrType() {
		return outMgrType;
	}

	/// Set to true if using the one anti-message optimization.
	void setOneAntiMsg(bool initOneAntiMsg) {
		usingOneAntiMsg = initOneAntiMsg;
	}

	/// Get whether the one anti-message optimization is being used.
	bool getOneAntiMsg() {
		return usingOneAntiMsg;
	}

	/// Set the output manager type flag. Can be DEFAULTSTATE, STATICSTATE, ADAPTIVESTATE
	void setStateMgrType(StateMgrType type) {
		stateMgrType = type;
	}

	/// Get the state manager type flag.
	StateMgrType getStateMgrType() {
		return stateMgrType;
	}

	/// Set whether optimistic fossil collection is being used.
	void setOptFossilColl(bool initOptFossilColl) {
		usingOptFossilCollection = initOptFossilColl;
	}

	/// Get whether optimistic fossil collection is being used.
	bool getOptFossilColl() {
		return usingOptFossilCollection;
	}

	/// Set true when recovering from a catastrophic rollback during
	/// optimimistic fossil collection.
	void setRecoveringFromCheckpoint(bool inRec) {
		inRecovery = inRec;
	}

	/// Return true when recovering from a catastrophic rollback during
	/// optimimistic fossil collection.
	bool getRecoveringFromCheckpoint() {
		return inRecovery;
	}

	// RK_NOTE: 6/3/2010: This used to be protected, but I changed it
	//                    to public so that the gvt managers can call it
	//                    immediately when the new gvt is set.

	/** call fossil collect on the state, output, input queue, and file queues.

	 @param fossilCollectTime time upto which fossil collect is performed.
	 */
	virtual void fossilCollect(const VTime &fossilCollectTime);

	/** call fossil collect on the file queues. This one passes in an integer
	 and should only be used with the optimistic fossil collection manager.

	 @param fossilCollectTime time upto which fossil collect is performed.
	 */
	virtual void fossilCollectFileQueues(SimulationObject *object,
			int fossilCollectTime);

	/// Returns the current coast forward to time for the given object.
	/// If the object is not coasting forward, NULL is returned.
	const VTime *getCoastForwardTime(const unsigned int &objectID) const;

	/// Used in optimistic fossil collection to checkpoint the file queues.
	void saveFileQueuesCheckpoint(std::ofstream* outFile,
			const ObjectID &objId, unsigned int saveTime);

	/// Used in optimistic fossil collection to restore the file queues.
	void restoreFileQueues(std::ifstream* outFile, const ObjectID &objId,
			unsigned int restoreTime);

	/// Returns the number of rollbacks
	unsigned int getRollbacks() { return numberOfRollbacks; }

  unsigned int getNumEventsExecuted();
  unsigned int getNumEventsRolledBack();

	//@} // End of Public Class Methods of TimeWarpSimulationManager.

protected:
	/**@name Protected Class Methods of TimeWarpSimulationManager. */
	//@{

	/** Create a map of simulation objects.

	 @return An STL hash-map of the simulation objects.
	 */
	typeSimMap *createMapOfObjects();

	/** initiate rollback recovery.

	 @param object handle to the simulation object
	 @param rollbackTime The time it needs to rollback to.
	 */
	virtual void rollback(SimulationObject *object, const VTime &rollbackTime);

	/// call coastforward if an infrequent state saving strategy is used
	void coastForward(const VTime &coastForwardFromTime,
			const VTime &rollbackToTime, SimulationObject *object);

	/** Display the global simulation object map.

	 @param out The output stream.
	 */
	void displayGlobalObjectMap(ostream &out);

	//void calculateSimulationTime();

	bool simulationComplete() {
		return simulationCompleteFlag;
	}

	void setSimulationTime(const VTime &newSimulationTime) {
		std::cerr
				<< "Error SimulationManager wide simulationTime is no longer used."
				<< std::endl;
		abort();
		//delete simulationTime;
		//simulationTime = newSimulationTime.clone();
	}

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

	///Actually sends the kernel messages to the destination simulation manager
	virtual void sendMessage(KernelMessage *msg, unsigned int destSimMgrId);

	/// This simulation manager's id.
	unsigned int mySimulationManagerID;

	/// Status of the simulation manager.
	bool simulationCompleteFlag;

	/// Time up to which coast forwarding should be done.
	vector<const VTime *> coastForwardTime;

	/// Flag to determine if message aggregation is enabled or not
	bool messageAggregation;

	/// the simulation time of this simulation manager
	//const VTime *simulationTime;

	/// Mapping between simulation object names, object pointers & ids.
	typeSimMap globalArrayOfSimObjPtrs;

	/// Mapping between simulation object ids to names.
	vector<vector<SimulationObject*> > globalArrayOfSimObjIDs;

	/// handle to the StateManager Factory
	StateManager *myStateManager;

	// handle to the GVTManager Factory
	GVTManager *myGVTManager;

	/// Handle to the optimistic fossil collection manager.
	OptFossilCollManager *myFossilCollManager;

	/// Handle to the communication manager Factory
	CommunicationManager *myCommunicationManager;

	/// Handle to the scheduler Factory
	SchedulingManager *mySchedulingManager;

	/// Handle to the scheduler Factory
	TimeWarpSimulationObjectQueue *mySimulationObjectQueue;

	/// Handle to the output manager Factory
	OutputManager *myOutputManager;

	///This is the handle to the set of events
	TimeWarpEventSet *myEventSet;

	///This is the handle to head of event sets in decentralized eventset
	SchedulingData *mySchedulingData;

	/// This is a handle to a termination manager
	TerminationManager *myTerminationManager;

	/// handle to the clock frequency manager
	DVFSManager *myDVFSManager;

	/// map of objects where each object can have several output file queues
	vector<vector<TimeWarpSimulationStream*> > outFileQueues;

	/// map of objects where each object can have several input file queues
	vector<vector<TimeWarpSimulationStream*> > inFileQueues;

	/// Used to specify the type of output manager used by the simulation manager.
	OutputMgrType outMgrType;

	/// Used to specify the type of state manager used by the simulation manager.
	StateMgrType stateMgrType;

	/// Used to determine if the one anti-message optimization is being used.
	bool usingOneAntiMsg;

	/// Used to determine if optimistic fossil collection is being used.
	bool usingOptFossilCollection;

	/// Used to determine when the optimistic fossil collection manager is
	/// recovery from a catastrophic rollback.
	bool inRecovery;

	/// Used to keep track of the number of rollbacks.
	unsigned int numberOfRollbacks;

	/**
	 Used to cancel local events.
	 */
    void cancelLocalEvents(const vector<const NegativeEvent *> &eventsToCancel);
	/**
	 Used to cancel remote events.
	 */
	void
			cancelRemoteEvents(
					const vector<const NegativeEvent *> &eventsToCancel);

	/**
	 Used to route local events.
	 */
	void handleLocalEvent(const Event *event);

	/**
	 Used to route remote events.
	 */
	void handleRemoteEvent(const Event *event);

	/**
	 Used to deal with negative events that we've been informed about.
	 */
	void handleNegativeEvents(const vector<const Event*> &negativeEvents);

	/**
	 Returns true if the simulation is complete, false otherwise.
	 */
	bool simulationComplete(const VTime &simulateUntil);

	/**
	 Talks to the physical layer to get messages.
	 */
	void getMessages();

	/**
	 Executes the local objects.  Returns true if there was an event to
	 execute, false otherwise.
	 */
	virtual bool executeObjects(const VTime &simulateUntil);

	Application *myApplication;

  StopWatch myStopwatch;

	//@} // End of Protected Class Methods of TimeWarpSimulationManager.

	//private:
	/**@name Private Class Attributes of TimeWarpSimulationManager. */
	//@{
	//@} // End of Private Class Attributes of TimeWarpSimulationManager.

};

#endif
