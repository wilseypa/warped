#ifndef TIMEWARPSIMULATIONOBJECTLOCKEDQUEUE_H_
#define TIMEWARPSIMULATIONOBJECTLOCKEDQUEUE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationObjectQueue.h"
#include "VTime.h"
#include "Event.h"
#include "EventId.h"
#include "ObjectEventPair.h"
#include <vector>
#include <queue>
#include <pthread.h>
using std::vector;
using std::priority_queue;
class TimeWarpSimulationManager;
class SimulationObject;
class OrganizedTimeWarpEventSet;
class TimeWarpSimulationObjectLockedQueue : public TimeWarpSimulationObjectQueue
{
public:
	TimeWarpSimulationObjectLockedQueue(TimeWarpSimulationManager *mySimulationManager);
	~TimeWarpSimulationObjectLockedQueue();

	const ObjectEventPair *getNext();
	const VTime *peekNext();
	bool insert(SimulationObject *object, const Event *event);
	void display();

private:
	class receiveTimeEventPair {
	public:
		bool operator()( const ObjectEventPair *a, const ObjectEventPair *b ) const{
			if( a->getReceiveTime() != b->getReceiveTime()) {
				return a->getReceiveTime() > b->getReceiveTime();
			}
			else {
				if (a->getSender() != b->getSender()) {
					return a->getSender() > b->getSender();
				}
				else {
					if (a->getEventId() != b->getEventId()) {
						return a->getEventId() > b->getEventId();
					}
					else {
						return true; //a == b
					}
				}// End If Sender
			}// End If ReceiveTime
		}// End Operator
	};// End class

	pthread_spinlock_t queueLock;
	priority_queue< ObjectEventPair *, vector<ObjectEventPair *>, receiveTimeEventPair > myObjectQueue;
	const VTime *minimumQueueTime;

};

#endif /* TIMEWARPSIMULATIONOBJECTLOCKEDQUEUE_H_ */
