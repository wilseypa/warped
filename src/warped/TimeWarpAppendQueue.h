#ifndef TIME_WARP_APPEND_QUEUE_H
#define TIME_WARP_APPEND_QUEUE_H

#include <unordered_map>

#include "warped.h"
#include "TimeWarpEventSet.h"
#include "InputSetObject.h"
#include "SenderQueueContainer.h"


using std::equal_to;

class Event;
class SimulationObject;
class TimeWarpSimulationManager;

/** The TimeWarpAppendQueue concrete class.

    This is a concrete class implements append queues for TimeWarp
    Kernel. The insert operations are mostly and usually reduced to
    append operations instead of normal insert in this queue
    implementations.

*/
class TimeWarpAppendQueue : public TimeWarpEventSet {
public:

  /**@name Public Class Methods of TimeWarpAppendQueue. */
  //@{

  /// Default Constructor
  TimeWarpAppendQueue( TimeWarpSimulationManager *initSimulationManager );
   
  /// Virtual Destructor
  virtual ~TimeWarpAppendQueue();
   
  /** Insert an event into the event set.

      @param Event The event to be inserted.
  */
  bool insert( Event *event, SimulationObject *object );

  /** Cancel out positive message corresponding to the anti message.
       
      @param cancelEvent The antimessage.
      @param object The object who receives the antimessage.
  */
  void handleAntiMessage( Event *cancelEvent, SimulationObject *object );

  /** Remove an event from the event set.

      @param Event The event to be removed.
      @param findMode Option for how to find the event.
  */
  virtual void remove( Event *, findMode, SimulationObject * );

  /** Remove and return the next event in the event set.

      @return The removed event.
  */
  virtual Event *getEvent(SimulationObject*);

  /** Return a reference to the next event in the event set.

      @return A reference to the next event in the event set.
  */
  virtual Event *peekEvent(SimulationObject*);

  /** Get a reference to some event in the event set.

      @param Event The event to find.
      @param findMode Option for how to find the event.
      @return A reference to the found event.
  */
  virtual Event *find(const VTIME&, findMode, SimulationObject* );

  /** Fossil collect the event set upto a given time.

      @param VTIME Time upto which to fossil collect.
  */
  virtual void fossilCollect(const VTIME&, SimulationObject * );

  /** Overriden from Configurable */
  void configure( SimulationConfiguration &configure ){}


  //@} // End of Public Class Methods of TimeWarpAppendQueue.

protected:
  /**@name Protected Class Methods of TimeWarpAppendQueue. */
  //@{

  /// Mapping between simulation object names, object pointers & id
  typedef std::unordered_map<string,  SenderQueueContainer* >  typeSenderMap;

  //map of SimulationObjectID and individual sender queues
  typeSenderMap *senderQMap;
  list<Event*> *processedQ;
  list<Event*> *sortedUnProcessedQ;
  vector<ScheduleListContainer*>* scheduleList;

  bool isInThePast( const Event* event );

  //@} // End of Protected Class Methods of TimeWarpAppendQueue.

private:
  void initSenderQueueEntry( const string &senderId );


  Event *peekedEvent;
  bool makeHeapFlag;
  bool popHeapFlag;
  TimeWarpSimulationManager *mySimulationManager;
};
#endif 
