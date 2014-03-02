#ifndef SINK_OBJECT_H
#define SINK_OBJECT_H


#include "SimulationObject.h"           // for SimulationObject
#include "warped.h"

class Event;
class VTime;

/** The class SinkObject.

    In a typical discrete-event simulation, very often there are certain
    simulation objects which do nothing else except create ({\bf source
    objects}) or consume events ({\bf sink objects}).  In the past, we
    did not make any distinction between ordinary simulation objects and
    source/sink simulation objects.  However, significant performance
    gains (in the case of distributed simulation where intelligent
    partitioning plays an important role in performance) can be achieved
    by discriminating between ordinary simulation objects and source/sink
    objects.  Since source/sink objects do nothing else other than create
    and consume events, they form a special class of simulation objects
    that can be scheduled differently from conventional simulation
    objects.  For this reason, we introduce two new object classes ({\tt
    SourceObject} and {\tt SinkObject}) derived from the {\tt
    SimulationObject} class.  If the user application needs to define
    source and sink objects, then they can derive from the {\tt
    SourceObject} and {\tt SinkObject} classes.

    Similar to the \Ref{SourceObject}, a sink object must be constructed
    with a {\tt VTime} argument to inform the kernel when it will start.
    During simulation, the kernel will schedule the sink object for
    execution and will call the object's {\tt executeProcess} method.
    However, for this to happen regularly, the object {\bf has} to tell
    the kernel when to schedule it next.  When the sink object is finished
    consuming events, it sets an internal flag which the kernel accesses
    by calling the {\bf iAmDone} method.  If the {\tt iAmDone} method
    returns true, the sink object is done and need not be scheduled for
    execution anymore.  In addition, in order to intelligently partition
    simulation objects in a distributed simulation, the kernel can call
    the sink object's {\tt getFanins} method to determine the set of
    simulation objects that send events to this sink object.
*/
class SinkObject : public SimulationObject {
public:
    /**@name Public Class Methods of SinkObject. */
    //@{

    /** Constructor.

        @param starttime Time when simulation object will start.
     */
    SinkObject(const VTime& starttime);

    /// Destructor
    virtual ~SinkObject();

    /** Tells whether simulation object has finished its simulation.

        The kernel calls this method to see if the simulation object
        has finished its simulation.

        @return A boolean that tells whether simulation is finished.
    */
    bool amIDone();

    /** Tell kernel that simulation has finished.

        This method is used to tell the kernel that this simulation object
        has finished its simulation and need not be scheduled anymore.
    */
    void iAmFinished();

    /** Get set of simulation objects that call this simulation object.

        @return Set of object id's that call this object.
    */
    const int* getFanins();

    //@} // End of Public Class Methods of SinkObject.

private:
    /**@name Private Class Attributes of SinkObject. */
    //@{

    /// Time at which this simulation object should start.
    const VTime& startTime;

    /// Tells whether simulation has finished for this object.
    bool sinkFinished;

    //@} // End of Private Class Attributes of SinkObject.

    /**@name Private Class Methods of SinkObject. */
    //@{
    /** Redefined and hidden method (should not be called).

        Since SinkObjects only consume events, this method cannot be called.
        If this method is called from within SinkObject, an error will be
        flagged.
    */
    void sendEvent(const Event* event);
    //@} // End of Private Class Methods of SinkObject.
};

#endif
