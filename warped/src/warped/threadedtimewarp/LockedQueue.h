#ifndef LOCKEDQUEUE_H_
#define LOCKEDQUEUE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include<pthread.h>
#include<deque>
using std::deque;

template<class element> class LockedQueue
{
private:
	deque<element> myQueue;
	pthread_spinlock_t spinlock;
public:
	LockedQueue()
	{
		pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE );
	}
	~LockedQueue(){};

	void enqueue(element e)
	{
		pthread_spin_lock(&spinlock);
		myQueue.push_back(e);
		pthread_spin_unlock(&spinlock);
	}

	element dequeue()
	{
		element returnVal = 0;
		if (!myQueue.empty())
		{
			pthread_spin_lock(&spinlock);
			if (!myQueue.empty())
			{
				returnVal = myQueue.front();
				myQueue.pop_front();
			}
			pthread_spin_unlock(&spinlock);
		}
		return returnVal;
	}

	element peekNext()
	{
		element returnVal = 0;
		pthread_spin_lock(&spinlock);
		if (!myQueue.empty())
		{
			returnVal = myQueue.front();
		}
		pthread_spin_unlock(&spinlock);
		return returnVal;
	}
};

#endif
