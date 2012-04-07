#ifndef THREADEDTIMEWARPSIMULATIONMANAGER_H_
#define THREADEDTIMEWARPSIMULATIONMANAGER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped/TimeWarpSimulationManager.h"
#include <iostream>
using std::endl;

class Application;
class ThreadedSchedulingManager;
class SimulationObject;
class LocalKernelMessage;
class VTime;
class ThreadedTimeWarpEventSet;
class WorkerInformation;
template<class element> class LockedQueue;
class ThreadedTimeWarpSimulationManager : public TimeWarpSimulationManager
{
public:
	/**@name Public Class Methods of ThreadedTimeWarpSimulationManager. */
	//@{

	/** Constructor.

	@param numProcessors Number of processors requested
	@param initApplication The application that we're going to start up.

	*/
    ThreadedTimeWarpSimulationManager( unsigned int numberOfWorkerThreads,
									   Application *initApplication );
	/// Destructor.
	~ThreadedTimeWarpSimulationManager();

	//Main simulation function
	void simulate( const VTime& simulateUntil );

	void setEventSetType(ThreadedTimeWarpEventSet *eventSet)
	{
		myThreadedEventSet = eventSet;
	}

	const unsigned int getNumberOfWorkerThreads() const
	{
		return numberOfWorkerThreads;
	}

	const VTime *getEarliestThreadTime();
	//Unmarks the emptyAllQueues flag and then calls executeObjects until all messages are gone
	void processOnlyMessages();
	//Turns on emptyAllQueues again
	void processAllQueues();
	const VTime &getEarliestOutputMessage();

	//@} // End of Public Class Methods of ThreadedTimeWarpSimulationManager.

protected:
	/**@name Protected Class Methods of ThreadedTimeWarpSimulationManager. */
	//@{

	/// Uses pthread_create to create threads
	void createWorkerThreads();

	///Function to be executed by threads
	void workerThread( const unsigned int &threadNumber );

	/// Calls the scheduler to get the next job
	bool executeObjects(const unsigned int &threadNumber);

	///By this time this function is called the messageReveiver is locked to our working thread
	void processLocalMessage(LocalKernelMessage *receivedMessage);
	void processLocalEventMessage(LocalKernelMessage *receivedMessage);
	void processLocalNegativeEventMessage(LocalKernelMessage *receivedMessage);
	void processLocalOutputCleanUpMessage(LocalKernelMessage *receivedMessage);
	void processFossilCollectStateMessage(LocalKernelMessage *receivedMessage);
	void processFossilCollectOutputMessage(LocalKernelMessage *receivedMessage);
	void processFossilCollectEventSetMessage(LocalKernelMessage *receivedMessage);

	/** Method the communication manager calls to deliver a kernel message.
	@param msg The message to receive.
	*/
	void receiveKernelMessage(KernelMessage *msg);
	void handleEventReceiver(  SimulationObject *currObject, const Event *event );
	void cancelEventsReceiver( SimulationObject *currObject, vector<const NegativeEvent *> &eventsToCancel );

	///Used by the worker threads to send their kernel messagee to a queue
	void sendMessage( KernelMessage *msg, unsigned int destSimMgrId);
	///Called by the manager thread to send all kernel messages for the whole simulation manager
	void sendPendingMessages();

	//Used by all simulation managers to fossil collect the state, outputQueue, and eventSet
	void fossilCollect(const VTime& fossilCollectTime);
	///First round of fossil collection to clean the state and find the minimum time
	const VTime *fossilCollectState(SimulationObject *object, const VTime& fossilCollectTime);
	//@} // End of Protected Class Methods of ThreadedTimeWarpSimulationManager.
private:
	///The OrganizedEventSet has special threadSafe functions that are not in an ordinary event set
	ThreadedTimeWarpEventSet  *myThreadedEventSet;
	///Holds information each thread needs to operate
	WorkerInformation **workerStatus;
	///Specified in the ThreadControl scope of the configuration file
	unsigned int numberOfWorkerThreads;
	///We can only end the simulation after cleaning all objects and having no events follow
	unsigned int outputMgrsCleaned;
	bool outputCleanersSent;
	//This flag is set when it is time for the GVT to be recalculated
	bool checkGVT;
	//This flag is used to determine whether the objectQueue should be dequeued or whether just
	//messages should be processed
	bool emptyAllQueues;
	//Queue of pending messages to be sent out.
	LockedQueue<KernelMessage*> *outgoingMessages;
	//This is a lock used around the simulation manager for debugging
	pthread_spinlock_t jobLock;
	/// The function called by pthread_create must be static
	static void *startWorkerThread(void *arguments);

	/// Put all arguments in one object to be passed to StartThread as void*
	class thread_args
	{
	public:
		thread_args(ThreadedTimeWarpSimulationManager *simManager, int threadIndex)
			: simManager(simManager), threadIndex(threadIndex) {}
		ThreadedTimeWarpSimulationManager *simManager;
		unsigned int threadIndex;
	};
};

#endif /* THREADEDTIMEWARPSIMULATIONMANAGER_H_ */
