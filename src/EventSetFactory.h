#ifndef EVENT_SET_FACTORY_H
#define EVENT_SET_FACTORY_H


#include "Configurer.h"                 // for Configurer
#include "EventSet.h"
#include "warped.h"

class Configurable;
class Event;
class SimulationConfiguration;

/** The EventSetFactory class.

This is the class interface for the various event set
implementations that are available in the Simulation kernel. The
specific event set implementation is hidden from the other
components of the simulation kernel by the aggregation of the
event set implemetations in this class (factory pattern).  All
event set implementations must be derived from the EventSet
abstract base class.

*/
class EventSetFactory : virtual public Configurer {

public:

    /**@name Public Class Methods of EventSetFactory. */
    //@{

    /// Default Constructor
    EventSetFactory();

    /// Destructor
    virtual ~EventSetFactory();

    /** Set the configured event set implementation.

    Function to attach the particular event set implementation in
    the kernel. Will be called by the configuration step to set the
    appropriate event set implementation.

    @param configurationMap Map of configuration choices.
    */
    Configurable* allocate(SimulationConfiguration& configuration,
                           Configurable* parent) const;

    static const EventSetFactory* instance();

    //@} // End of Public Class Methods of EventSetFactory.

protected:

    /**@name Private Class Attributes of EventSetFactory. */
    //@{

    //@} // End of Private Class Attributes of EventSetFactory.

};

#endif
