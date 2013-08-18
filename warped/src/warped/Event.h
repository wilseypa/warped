#ifndef EVENT_H
#define EVENT_H


#include "warped.h"
#include <warped/Serializable.h>

class VTime;
class EventId;
//class OptFossilCollManager;

/** The abstract base class Event.  

Events represent the communication between simulation objects.  Every
simulation should have at least one event class defined. This class
provides an interface for such definitions.  
    
To send an Event, the sender needs to have a handle to the receiver (or
a kernel proxy, which is transparent to the sender), and call
"receiveEvent" on the handle.

@see SimulationObject#receiveEvent
*/
class Event : public Serializable {
public:
  /**@name Public Class Methods of Event */
  //@{

  friend class TimeWarpSimulationManager;
  friend class NegativeEvent;

  /// Destructor.  Overriders must define a destructor.
  virtual ~Event() = 0;
  
  /** Returns the source (sender) of an event.

  @return Sender id of event.
  
  */
  virtual const ObjectID &getSender() const = 0;

  /** Returns the send time of this event.

  @return sendTime send time of this event.

  */
  virtual const VTime &getSendTime() const = 0;

  /** Returns the destination (receiver) of an event.

  @return Receiver id of event.

  */
  virtual const ObjectID &getReceiver() const = 0;
  

  /** Get time at which the event must be received and scheduled for
      execution.

      @return Time at which the event must be scheduled.  

  */
  virtual const VTime &getReceiveTime() const = 0;
  
  /** Compare two events and determine if they are equal.

  This method is used to compare two events to see if they are
  identical or not. If there are differences, then false is
  returned else true is returned.

  This is a pure virtual function and has to be overridden.

  @param Event the event you are comparing this one with.
  @result boolean result of the comparison.
  */
  virtual bool eventCompare(const Event* event) = 0;

  /**
     Return the EventId associated with this event.  The tuple of {sender,
     eventId} should be able to uniquely identify a messages between two
     separate SimulationManagers.  This method/id is really only used in
     distributed simulation but currently the only place to put this
     interface is in Event.
  */
  virtual const EventId &getEventId() const = 0;

  /** Get the size of the event.

      This is a pure virtual function and has to be overridden.

      @return The size of the event.
  */
  virtual unsigned int getEventSize() const = 0;

  /** 
     A utility method that performs the serialization of the data available
     to this abstract class.  In general derived events will want to
     override Serializable#serialized and possible call this utility to do
     part of the work.
  */
  void serialize( SerializedInstance *addTo ) const;
   
  /// Overloaded operator <<
  friend std::ostream& operator<<(std::ostream& os, const Event& event);

  /// Overloaded operator ==
  friend bool operator==( const Event &eve1, const Event &eve2 );

  /// Overload operator new.
  void *operator new(size_t);

  /// Overload operator delete.
  void operator delete(void *);

  /**
     A static utility method that allows the comparision of two events
     based on this interface.  In general derived events will want to
     override eventCompare and possibly call this utility to do part of the
     work.
  */
  static bool compareEvents( const Event *, const Event * );

private:
  // No longer using these for optimistic fossil collection.
  //static OptFossilCollManager *myOptFosColMan;
  //static bool usingOptFossilCollMan;
};


#endif
