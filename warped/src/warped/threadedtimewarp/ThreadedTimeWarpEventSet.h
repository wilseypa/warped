#ifndef ThreadedEVENTSET_H_
#define ThreadedEVENTSET_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "TimeWarpEventSet.h"
#include "Event.h"
#include "NegativeEvent.h"
#include "EventFunctors.h"
#include <pthread.h>
#include <deque>
#include <queue>
#include "LocalKernelMessage.h"
#include "LocalNegativeEventMessage.h"
using std::deque;
using std::priority_queue;

template<class element> class LockedQueue;
class SimulationObject;
class TimeWarpEventSetFactory;
class SimulationConfiguration;
class ThreadedTimeWarpSimulationManager;
class DefaultTimeWarpEventContainer;
class TimeWarpSimulationObjectQueue;
class AtomicSimulationObjectState;

/** The default implementation of TimeWarpEventSet.  This implementation
    works in the following manner.  Events are kept in two groups,
    unprocessed and processed.  Processed events are not explicitly sorted
    (although they maybe be by virtue of the order of their insertion),
    unprocessed events are sorted on demand.  That is, they are inserted in
    arbitrary order but when events are requested they are sorted if
    needed.
*/
class ThreadedTimeWarpEventSet : public TimeWarpEventSet {


public:
  ThreadedTimeWarpEventSet( TimeWarpSimulationManager *initSimManager );

  ~ThreadedTimeWarpEventSet();
  /// This insert may fail if this event occurs before the current time in the object queue
  bool insert( const Event *event );
  ///If the inset(event) fails, iterate on insertObject until the insert succeeds
  ///Each iteration you should execute one event from this object
  bool insertObject( SimulationObject *object );
  bool releaseObject(const unsigned int &threadNumber, SimulationObject *object);
  bool lockObject(const unsigned int &threadNumber, const OBJECT_ID &objectIDToBeLocked);

  bool handleAntiMessage( SimulationObject *eventFor, const NegativeEvent *cancelEvent );

  const Event *getEvent(SimulationObject *);

  const Event *getEvent(SimulationObject *, const VTime&);

  /**
	Grabs and locks the SimulationObject with either a pending receive
	or if it is the next in the calendar queue
  */
  LocalKernelMessage *getNextObjectForMessageProc(const unsigned int &threadID);
  SimulationObject *getNextObjectForEventProc(const unsigned int &threadID);
  ///If local messages are used this function call grabs the next one
  LocalKernelMessage *getLocalMessage(SimulationObject *);
  ///Inserts a local message into the receivers message queue
  void insertLocalMessage(LocalKernelMessage *message);

  const Event *peekEvent(SimulationObject *);
  const Event *peekEvent(SimulationObject *, const VTime&);
  const VTime *getEarliestVTimeFromObjectQueue();

  void fossilCollect( SimulationObject *, const VTime & );

  void fossilCollect( SimulationObject *, int );

  void fossilCollect( const Event * );

  void configure( SimulationConfiguration & ){}

  void rollback( SimulationObject *, const VTime & );

  bool inThePast( const Event * );

  void ofcPurge();

  void debugDump( const string &objectName, ostream &os );

  const AtomicSimulationObjectState *getSimulationObjectState(const SimulationObject *const object) const;
  const VTime &getEarliestVTime();

  const int getMessageCount()
  {
	  return numberOfMessages;
  }
  unsigned int rollbackCounter;
private:
	ThreadedTimeWarpSimulationManager *mySimulationManager;
	TimeWarpSimulationObjectQueue *simulationObjectQueue;
	LockedQueue<LocalKernelMessage*> *receivedMessagesQueue;
	AtomicSimulationObjectState** simulationObjectStates;
	DefaultTimeWarpEventContainer** events;
	unsigned int numberOfWorkerThreads;
	int numberOfMessages;
};

#endif /* ThreadedEVENTSET_H_ */
