#ifndef EVENT_SET_H
#define EVENT_SET_H


#include "warped.h"
#include "Configurable.h"

class Event;

/** The EventSet abstract base class.

    This is the abstract base class for all event set implementations in
    the kernel. Different event set implementations must be derived from
    this abstract class.

*/
class EventSet : virtual public Configurable {
public:
    /**@name Public Class Methods of EventSet. */
    //@{

    /// Virtual Destructor
    virtual ~EventSet() {};

    /** Insert an event into the event set.

        This is a pure virtual function and must be overridden.

        @param Event A pointer to the event to be inserted.
    */
    virtual void insert(const Event*) = 0;

    /** Returns the event to be processed.

        This is a pure virtual function and must be overridden.

        @return Return a pointer to the event.
    */
    virtual const Event* getEvent() = 0;

    /** Do I have an event to execute ?

        This is a pure virtual function and must be overridden.

        @return Return a ptr to an event (if any) to execute.
    */
    virtual const Event* peekEvent() = 0;

    /** Delete any unwanted (processed) elements.

        This is a pure virtual function and must be overridden.
    */
    virtual void cleanUp() = 0;

    //@} // End of Public Class Methods of EventSet.

protected:
    /**@name Protected Class Methods of EventSet. */
    //@{

    /// Default Constructor
    EventSet() {};

    //@} // End of Protected Class Methods of EventSet.
};

#endif
