#ifndef SOURCE_OBJECT_H
#define SOURCE_OBJECT_H


#include "warped.h"
#include "SimulationObject.h"

/** The class SourceObject.

    In a typical discrete-event simulation, very often there are certain
    simulation objects which do nothing else except create ({\bf source
    objects}) or consume events ({\bf sink objects}).  In the past, we did
    not make any distinction between ordinary simulation objects and
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

    Since a source object is ONLY a generator of events, the only way to
    schedule this object for execution is if the object informs the kernel
    to schedule it at an appropriate time.  For this reason, during the
    start of the simulation, an object of type {\tt SourceObject} {\bf
    must} be constructed with a {\tt VTime} argument to inform the kernel
    when it will start.  During simulation, the kernel will schedule the
    source object for execution and will call the object's {\tt
    executeProcess} method. However, for this to happen regularly, the
    object {\bf has} to tell the kernel when to schedule it next.  When
    the source object is finished generating events, it sets an internal
    flag which the kernel accesses by calling the {\tt iAmDone} method.
    If the {\tt iAmDone} method returns true, the source object is done
    and need not be scheduled for execution anymore.  In addition, in
    order to intelligently partition simulation objects in a distributed
    simulation, the kernel can call the source object's {\tt getFanouts}
    method to determine the set of simulation objects that receive events
    from this source object.

    Also note that the methods {\tt sendEvent}, {\tt getEvent}, and {\tt
    haveMoreEvents} (which are inherited from the {\tt SimulationObject}
    class) are redefined and are private to the {\tt SourceObject} class.
    An error will be flagged if any of these three methods is called on a
    source object.
 */
class SourceObject : public SimulationObject {
public:
  /**@name Public Class Methods of SourceObject. */
  //@{

  /** Constructor.

      @param starttime The time when the simulation object will start.
   */
  SourceObject(const VTime& starttime);

  /// Destructor
  virtual ~SourceObject();

  /** Tells whether simulation object has finished its simulation.

      The kernel calls this method to see if the simulation object
      has finished its simulation.

      @return A boolean that tells whether simulation is finished.
  */
  bool amIDone();

  /** Tell kernel that simulation has finished.

      This method is used to tell the kernel that this simulation object
      has finished its simulation and need not be scheduled anymore. The
      flag \Ref{sourceFinished} will be set when this method is called.  
  */
  void iAmFinished();

  /** Get set of simulation objects that receive events from this
      simulation object.

      @return Set of object id's that receive events from this object.  
  */
  const int* getFanouts();

  //@} // End of Public Class Methods of SourceObject.

private:
  /**@name Private Class Attributes of SourceObject. */
  //@{

  /// Time at which simulation object should start.
  const VTime& startTime;
  /// Tells whether this object has finished its simulation.
  bool sourceFinished;

  //@} // End of Private Class Attributes of SourceObject. */

  /**@name Private Class Methods of SourceObject. */
  //@{

  /** Redefined and hidden method (should not be called).

      This method is kept private and is redefined from the inherited
      SimulationObject::\Ref{SimulationObject::sendEvent}. This is done so
      that no-one can call this method using a SourceObject. If this
      method is called from within this class, an error will be flagged.  
  */
  void sendEvent(const Event* event);

  /** Redefined and hidden method (should not be called).
   */
  const Event* getEvent();

  /** Redefined and hidden method (should not be called).
   */
  bool haveMoreEvents();

  //@} // End of Private Class Methods of SourceObject.
};

#endif
