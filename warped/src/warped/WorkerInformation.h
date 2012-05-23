#ifndef WORKERINFORMATION_H_
#define WORKERINFORMATION_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include <pthread.h>
#include "VTime.h"
#include <iostream>
using namespace std;

static const int MAX_NUMBER_IDLE_LOOPS = 50;
//static int threadCounter = 0;
///Each worker thread gets assigned an object of this type to keep track of its status
class WorkerInformation {
public:
	WorkerInformation(pthread_t *thread) :
		mypthread(thread), idleCount(0), suspended(false) {
		//Set the busy flag with the function so the stillBusyCount goes up
		pthread_mutex_init(&suspendMutex, NULL);
		pthread_cond_init(&suspendCondition, NULL);
		pthread_rwlock_init(&threadTimeLock, NULL);
		//Assume each thread starts as busy, dont count the manager thread
		if (thread != NULL) {
			__sync_fetch_and_add(&globalStillBusyCount, 1);
		}
	}
	~WorkerInformation() {
		delete mypthread;
	}

	//The worker checks this to know whether the sim manager expects more work to be assigned
	//or whether this thread should exit for good
	static bool isWorkRemaining() {
		return workRemaining;
	}

	//Called by the simulation manager to kill the workers nicely
	static void killWorkerThreads() {
		ASSERT(__sync_bool_compare_and_swap(&workRemaining, true, false));
	}

	//If the worker does no work and its been idle for some time this flag is unset
	inline void resetIdleCount() {
		idleCount = 0;
	}

	//called by the workerThread everytime it does no work
	//If it is idle for long enough the thread will suspend itself
	//The manager thread should never call this function
	//Otherwise their is no-one left to call resume
	void incrementIdleCount() {
		idleCount++;
		//Check if we should suspend ourselves (been idle for to long)
		if (idleCount > MAX_NUMBER_IDLE_LOOPS) {
			//This thread is no longer active so reset the idle count.
			resetIdleCount();
			//Get a mutex lock to suspend safely
			pthread_mutex_lock(&suspendMutex);
			//Atomicallly mark the flag so the simulation manager does not need a lock
			//inorder to check whether this thread is suspended or not
			__sync_bool_compare_and_swap(&suspended, false, true);
			//decrement the global counter (when it hits 0, everyone is suspended)
			__sync_fetch_and_sub(&globalStillBusyCount, 1);
			//*******************************************************
			utils::debug << "Worker is sleeping!!!" << endl;
			//Suspend here until the suspendCondition is met, (also auto-releases the mutex lock we just got)
			pthread_cond_wait(&suspendCondition, &suspendMutex);
			utils::debug << "Worker is back!!!" << endl;
			//*******************************************************
			//Upon the condition being met, the suspendMutex lock is also reset for us to undo
			pthread_mutex_unlock(&suspendMutex);
			//Their is now one more busy thread
			__sync_fetch_and_add(&globalStillBusyCount, 1);
		}
	}

	//Called by the simulation manager to check whether any workers are still busy
	static unsigned int getStillBusyCount() {
		return globalStillBusyCount;
	}

	//Called by the simulation manager to resume a suspended thread because their is now
	//enough work to keep it busy
	void resume() {
		//If This worker is suspended wake them up
		if (suspended) {
			//Get a mutex lock to resume safely
			pthread_mutex_lock(&suspendMutex);
			//We can use cond_signal because we are only waking up a single thread
			//otherwise cond_broadcast would be used
			pthread_cond_signal(&suspendCondition);
			//No atomic move needed here, because the thread is already suspended
			//and we are the simulationManager
			suspended = false;
			//release the mutex lock so the thread can continue to resume
			pthread_mutex_unlock(&suspendMutex);
		}
	}

	inline const pthread_t *getThread() const {
		return mypthread;
	}

	///Everytime an object is released back into the pool
	///grab its simulation time
	void setThreadTime(const SimulationObject *object) {
		pthread_rwlock_wrlock(&threadTimeLock);
		delete threadTime;
		threadTime = object->getSimulationTime().clone();
		pthread_rwlock_unlock(&threadTimeLock);
	}

	///Whenever GVT needs to be calculated, use this function
	///to compare your current Lowest Time Stamp with this threads time
	const VTime *getLowerTime(const VTime *lowestSoFar) {
		if (!suspended) {
			pthread_rwlock_rdlock(&threadTimeLock);
			if (*threadTime < *lowestSoFar) {
				delete lowestSoFar;
				lowestSoFar = threadTime->clone();
			}
			pthread_rwlock_unlock(&threadTimeLock);
		}
		return lowestSoFar;
	}

private:
	///This threads Id;
	const pthread_t *mypthread;
	pthread_cond_t suspendCondition;
	pthread_mutex_t suspendMutex;
	///Single value to check whether this thread is suspended
	bool suspended;
	///A counter for the number of loops a worker can be idle before
	///suspending itself
	int idleCount;
	///the simulationTime of the last object that was executed
	//by this thread
	const VTime *threadTime;
	///rw lock for updating threadTime
	pthread_rwlock_t threadTimeLock;
	///This is used so the simulation manager can monitor when this is zero
	/// i.e no threads have there stillBusyFlagSet
	static int globalStillBusyCount;
	///This flag is for the simulation manager to tell the threads to end
	static bool workRemaining;
};

#endif /* WORKERINFORMATION_H_ */
