#ifndef SIMULATION_OBJECT_H
#define SIMULATION_OBJECT_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include <string>

using std::string;
using std::ios;

class SimulationManager;
class Event;
class State;
class SerializedInstance;
class SimulationStream;

/** The abstract base class SimulationObject. 

Simulation objects are the core of a discrete event simulation.  A
simulation object represents an entity that can exchange (send and
receive) events with other simulation objects.  As a result of these
events, changes are made to the simulation object's internal state (and
output may result).

This class provides a concise definition of the simulation object's 
interface as the application sees it.

*/
class SimulationObject {

  friend class SequentialSimulationManager;
  friend class TimeWarpSimulationManager;
  friend class DefaultEvent;
  friend class DTTimeWarpSimulationManager;
   
public:
  /**
     Returns a string name for this object.  Each object's name must be
     unique across the simulation.
  */
  virtual const string &getName() const = 0;

  /// Returns a pointer to the receiver simulation object or a proxy.
  SimulationObject *getObjectHandle( const string& objectToGet ) const;

  /**@name Public Class Methods of SimulationObject. */
  //@{

  /// Destructor.
  virtual ~SimulationObject();

  /** This method gets called on each object before the simulation begins.

  This gives each object a chance to perform any actions required for
  initialization (examples of what might occur during initialization
  might include opening files, or source type objects sending initial
  events).  At least one object in each simulation should make a call
  to {\tt sendEvent} during the initialization phase (If no events are
  sent during this phase, then no objects will have events to process,
  and the simulation will complete before it ever begins!). {\tt
  MINFINITY} will be returned when {\tt getSimulationTime} is called
  during initialization. A call to {\tt getState} returns the object's
  initial state.  A call to {\tt haveMoreEvents} will return {\tt
  false}, and a call to {\tt getEvent} will return {\tt NULL}.  A call
  to {\tt reportError} is OK during this phase, and will cause the
  simulation to halt.

  This is a pure virtual function and has to be overridden by the
  user.  
  */
  virtual void initialize() = 0;

  /** This method is called after the simulation has ended.  
      
  This allows the simulation objects to ``clean up'' after themselves,
  perform actions such as closing files, collecting statistics, and
  producing output.  {\tt PINFINITY} will be returned when {\tt
  getSimulationTime} is called during finalization. A call to {\tt
  getState} will return the object's final state; a call to {\tt
  haveMoreEvents} will return {\tt false}, a call to {\tt getEvent}
  will return {\tt NULL}, and a call to {\tt reportError} will cause
  an error to be logged.  Any events sent with {\tt sendEvent} during
  finalization will {\bf not} be delivered.

  This is a pure virtual function and has to be overridden by the user.
  */
  virtual void finalize() = 0;

  /** Call to application to execute its code for one simulation cycle.

  The kernel calls this method on an object when it has at least one event
  for the object to process.  During a call to this method, the object
  should process {\bf all} events available for it, or an error will be
  logged by the kernel (the simulation won't halt).  During process
  execution, a simulation object may not block for any reason.

  This is a pure virtual function and has to be overridden by the user.
  */
  virtual void executeProcess() = 0;

  /** Call application to allocate state.

  The kernel calls this method when it needs the simulation object to
  allocate a state on its behalf. This method will be called at least
  once before the first call to {\tt initialize}.  The application
  will know if a call to {\tt allocateState} needs to initialize the
  new state as the object's initial state when {\tt getSimulationTime}
  returns {\tt MINIFINITY}.  If a call to this method returns {\bf
  anything else}, then the state need not be initialized - the kernel
  is going to call {\tt State::copyState()} to initialize it with some
  other state, but the application need not worry about this.

  This is a pure virtual function and has to be overridden by the
  user.

  @return A pointer to the state that was allocated.  
  */
  virtual State* allocateState() = 0;
  
  /** Call application to deallocate state.

  The kernel calls this method to hand a state back to the application
  when it is done with it. At this point, the application may
  deallocate it, or do whatever it likes with it.  (It could just push
  it into a stack and give it back in the next {\tt allocateState}
  call, hint hint.)

  This is a pure virtual function and has to be overridden by the
  user.

  @param state A pointer to the state to the deallocated.  
  */
  virtual void deallocateState( const State *state ) = 0;

  /** Serialize an event.

  The kernel calls this method to handle migration of events from one
  process to another. It is the responsibility of the simulation
  object that generated the event to serialize that event. If an
  application generates different types of events then it is up to the
  application writer to distinguish between the different events. A
  {\tt tag} field in the serialization instance may be used for this
  purpose.
      
  A good approach to do this would be to correspondingly overload the
  {\tt serializeEvent} method in the {\tt EventBase} class to actually
  perform the serialization of the event. During or after
  serialization the event (or any of its constituents) should not be
  deleted or altered by the process.

  @return A pointer to a serialized event.
  @param event A pointer to an event to be serialized.  
  */
  virtual SerializedInstance* serializeEvent( Event* event );

  /** Deserialize an event.

  The kernel calls this method to handle deserialization of serialized
  objects. It is the responsibility of the application process to
  instantiate the right type of event (a {\tt tag} field can be used
  for this). Having instantiated the right event, the process must
  copy the corresponding data from the serialized object to the
  event. A good mechanism to implement this functionality would be to
  correspondingly overload the {\tt deserializeEvent} method in the
  event class to actually copy the data from the serialized object to
  the event.  The event generated from deserialization should not be
  processed by the application.  Call to deserialization does not mean
  that the process is being scheduled for simulation.

  @return A pointer to a deserialized event.  
  @param instance A pointer to the instance to be deserialized.  
  */
  virtual Event* deserializeEvent( SerializedInstance* instance );

  /** Reclaim an event.

  {\bf Note: We ourselves are undecided about these features and this
  specification (or even whether to have this method)
  is not "nailed down"!}

  This method is called to inform the user application that the kernel
  has no further use for this event. This method is provided so that
  the user may reuse the memory allocated for the event in future. It
  is up to the user to recycle the memory allocated for the event. In
  the simple case, the event is deleted.
            
  Recycling events using a simple stack (for each type of event that
  the process generates) will reduce the memory allocation overheads
  and thereby reducing the overall simulation time.  This method may
  be called by the kernel to allow the process to reclaim the events
  that it created as a result of a call to {\tt deSerializeEvent}
  method.  The process may once again choose to recycle the memory
  allocated (a Last-In-First-Out structure is strongly recommended for
  this purpose) or may merely delete it.

  {\bf Note:} Depending on what kind of performance benefits can be
  obtained, a second definition of {\tt reclaimEvents} that accepts a
  list of events as an input parameter is also being considered.

  @param event Pointer to the reclaimed event.  
  */
  virtual void reclaimEvent( const Event* event ) = 0;

  /** Return the next event.

  This method is called by the application to get the next event at
  the current simulation time. This method will return {\tt NULL} if
  it is called after all events for the current simulation time have
  been received.  This method should be used in conjunction with {\tt
  haveMoreEvents()} to process all events during a call to {\tt
  executeProcess}.  Refer to the following example:

  \begin{verbatim}
  void executeProcess() {
  while( haveMoreEvents() == true ){
  MyEventType *currentEvent = dynamic_cast<MyEventType *>(getEvent());
  doIt( currentEvent );
  }
  }  
  \end{verbatim}

  @return A pointer to the next event at the current simulation time.  
  */
  const Event *getEvent();

  /** Receive an event.
      
  When one SimulationObject wants to send another SimulationObject an
  Svent, it should call SimulationManager::getObjectHandle and then
  SimulationObject::receiveEvent on the returned handle.

  @param A pointer to event to be delivered.
  */
  virtual void receiveEvent( const Event *newEvent );

  /** Tells whether this simulation object has more events to process at
      the current simulation time.

      This method should be called by the application to find out if there
      are more events to be processed at the current simulation time.  The
      first time this method is called within an {\tt executeProcess}
      call, it will return {\tt true}.  Each subsequent call will return
      {\tt true} if there is another event to process this simulation
      cycle, and {\tt false} if there is not.

      @return A boolean specifying whether sim-object has more events.  
  */
  bool haveMoreEvents();
  
  /** Get the current local virtual time for this simulation object.

  During simulation execution (i.e. when the {\tt executeProcess} method is
  being executed), a call to {\tt getSimulationTime} will return the
  current local virtual time.  If called during {\tt initialize()}, {\tt
  MINFINITY} is returned.  A call during {\tt finalize()} will return {\tt
  PINFINITY}.


  @return The current local virtual time for this simulation object.  
  */
  const VTime &getSimulationTime() const;
   
  /** Return the state corresponding to the current simulation time.

  During simulation, a call to this method will return a valid state
  corresponding to the current simulation time.  A call to this method
  during object construction will return {\tt NULL}.  A call to this
  method during {\tt initialize} will return the object's initial
  state.  A call to this method during {\tt finalize} will return the
  object's final state.  During simulation, the application is allowed
  to modify the state returned by {\tt getState}. In fact, this is the
  only way that the object should modify any dynamic state that it
  has.  However, it should NOT deallocate the state.

  @return A pointer to the state at the current simulation time.  
  */
  virtual State *getState(){ return myState; }

  /** Report an error.

  This call can be used during the simulation to report than an error
  has occurred. If the {\tt Severity} is {\tt WARNING}, the message
  will simply be logged by the kernel with some kernel-dependent debug
  data.  If the {\tt Severity} is {\tt ABORT}, the simulation will
  abort and terminate immediately, with a call to {\tt finalize()}.

  @param msg The error message.
  @param level The level of severity of the error.  
  */
  virtual void reportError( const string& msg, const SEVERITY level );


  /// Returns the name of the simulation object
  SimulationObject *getObjectHandle( const OBJECT_ID *objectID ) const;

  /// sets the pointer to the simulation manager
  virtual void setSimulationManager( SimulationManager *simMgr );

  /// gets the pointer to the simulation manager
  virtual SimulationManager *getSimulationManager() const {
    return mySimulationManager;
  }
   
  /// set my object id
  virtual void setObjectID(OBJECT_ID *id){ myObjectID = id; }

  /// return my object id
  virtual OBJECT_ID* getObjectID() const { return myObjectID; }
  
  /// get a handle to a simulation input stream
  SimulationStream *getIFStream(const string &filename);
   
  /// get a handle to a simulation output stream
  SimulationStream *getOFStream(const string &filename,
				ios::openmode mode=ios::out);
   
  /// get a handle to a simulation input-output stream
  SimulationStream *getIOFStream(const string &filename);

  // functions for effective work estimation
  // based on reiher and jefferson (1990)
  void addEffectiveWork(double work) {
      myEffectiveWork += work;
      myTotalWork += work;
  }
  void undoEffectiveWork(double work) { myEffectiveWork -= work; }
  double getEffectiveWork() { return myEffectiveWork; }
  double getTotalWork() { return myTotalWork; }
  void resetEffectiveWork() {
      myEffectiveWork = 0;
      myTotalWork = 0;
  }
   
  //@} // End of Public Class Methods of SimulationObject.
  void setSimulationTime( const VTime &newSimulationTime );
protected:
  /**@name Protected Class Methods of SimulationObject. */
  //@{

  /// Default Constructor.
  SimulationObject();

  //@} // End of Protected Class Methods of SimulationObject.

  /**@name Protected Class Attributes of SimulationObject. */
  //@{

  /// This is my handle to my simulation manager
  SimulationManager *mySimulationManager;


  //@} // End of Protected Class Attributes of SimulationObject.


private:
  /**@name Private Class Attributes of SimulationObject. */
  //@{

  virtual void setInitialState(State* state) { myState = state; }
  unsigned int getNextEventId(){ return eventIdVal++; }

  /// this is my object id
  OBJECT_ID  *myObjectID;

  /// This is the current state of the object
  State* myState;
  
  /// the local virtual time of this object
  VTime *localVirtualTime;

  /// The eventId sequence numbers.
  unsigned int eventIdVal;

  // functions for effective work estimation
  // based on reiher and jefferson (1990)
  double myEffectiveWork;
  double myTotalWork;
   
   //@} // End of Private Class Attributes of SimulationObject.
};

#endif
