#ifndef TIME_WARP_SIMULATION_OBJECT_QUEUE_H_
#define TIME_WARP_SIMULATION_OBJECT_QUEUE_H_


#include "warped.h"
#include "Configurable.h"

class SimulationObject;
class TimeWarpSimulationObjectQueueFactory;
class TimeWarpSimulationManager;
class Event;
class ObjectEventPair;

/** The TimeWarpSimulationObjectQueue abstract base class.

	All object queues must be derived from this base class

*/
class TimeWarpSimulationObjectQueue : virtual public Configurable {

	/**@type friend class declarations */
	//@{

	/// Factory class for TimeWarpSimulationObjectQueue
	friend class TimeWarpSimulationObjectQueueFactory;

	//@} // End of friend class declarations

public:
	/**@name Public Class Methods of TimeWarpSimulationObjectQueue. */
	//@{

	/// Virtual Destructor
	virtual ~TimeWarpSimulationObjectQueue();

	/** Insert an object into the object queue based on its lowest time stamped event.

	This is a pure virtual function that has to be overridden.

	@param SimulationObject The object to be inserted.

	*/

	virtual bool insert( SimulationObject *object, const Event *event ) = 0;

	/** Return a pointer the next object to be scheduled for execution.
	Removes that object from the queue.
	Compare the receive time with the top event time
	to determine if the object from getNext is valid
	since their is no remove

	This is a pure virtual function and has to overridden.

	@return Pointer to the object, or NULL if none found.
	*/
	virtual const ObjectEventPair *getNext() = 0;

	/** Return a pointer the next object to be scheduled for execution.
	Compare the receive time with the top event time
	to determine if the object from getNext is valid
	since their is no remove

	This is a pure virtual function and has to overridden.

	@return Pointer to minimum queue time, or NULL if none found.
	*/
	virtual const VTime *peekNext() = 0;


	virtual void display() = 0;


	virtual void configure(SimulationConfiguration &){};

	TimeWarpSimulationManager *getSimulationManager()
	{
		return mySimulationManager;
	}
	//@} // End of Public Class Methods of TimeWarpSimulationObjectQueue.

protected:
	/**@name Protected Class Methods of TimeWarpSimulationObjectQueue. */
	//@{
	/// Default Constructor
	TimeWarpSimulationObjectQueue(TimeWarpSimulationManager *initSimulationManager);

	TimeWarpSimulationManager *mySimulationManager;
	//@} // End of Protected Class Methods of TimeWarpSimulationObjectQueue.
};

#endif /* TIME_WARP_OBJECT_QUEUE_H_ */
