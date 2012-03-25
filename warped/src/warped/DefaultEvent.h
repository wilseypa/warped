#ifndef DEFAULT_EVENT_H
#define DEFAULT_EVENT_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped/Event.h"
#include "warped/EventId.h"
#include "warped/SimulationObject.h"

/** A default implementation of the Event interface. */

class DefaultEvent : public Event {
public:
  virtual ~DefaultEvent() = 0;
  
  inline const ObjectID &getSender() const { return *sender; }

  inline const VTime &getSendTime() const { ASSERT( sendTime != 0 ); return *sendTime; }

  inline const ObjectID &getReceiver() const { return *receiver; }
  inline void setReceiver( const SimulationObject *newReceiver ){ receiver = newReceiver->getObjectID(); }
  
  inline const VTime &getReceiveTime() const { ASSERT( receiveTime != 0 ); return *receiveTime; }

  inline const EventId &getEventId() const {
    return eventId;
  }

  void subtractEffectiveWork(SimulationObject* o) const { o->undoEffectiveWork(work); }
  void setWork(int w) { work = w; }

  //@} // End of Event Public Class Methods.

protected:
  /**
     Constructor to be called by applications directly.
  */
  inline DefaultEvent( const VTime &initSendTime,
		       const VTime &initRecvTime,
		       SimulationObject *initSender,
		       SimulationObject *initReceiver ) : sendTime( initSendTime.clone() ),
						      receiveTime( initRecvTime.clone() ),
						      sender( new ObjectID(*(initSender->getObjectID())) ), 
						      receiver( new ObjectID(*(initReceiver->getObjectID())) ),
                              eventId( initSender->getNextEventId() ),
                              work(0){ }
  /**
     Constructor to be called by deserializers only.
  */
  inline DefaultEvent( const VTime &initSendTime,
		       const VTime &initRecvTime,
		       const ObjectID &initSender,
		       const ObjectID &initReceiver,
		       const unsigned int eventIdVal ) : sendTime( initSendTime.clone() ),
							 receiveTime( initRecvTime.clone() ),
							 sender( new ObjectID(initSender) ), 
							 receiver( new ObjectID(initReceiver) ),
                             eventId( eventIdVal ),
                             work(0){}

  /**
     Constructor available for copy constructors.
  */
  inline DefaultEvent( const VTime &initSendTime,
                       const VTime &initRecvTime,
                       const ObjectID &initSender,
                       const ObjectID &initReceiver,
                       const EventId &eventIdVal ) : sendTime( initSendTime.clone() ),
                                                     receiveTime( initRecvTime.clone() ),
                                                     sender( new ObjectID(initSender) ),
                                                     receiver( new ObjectID(initReceiver) ),
                                                     eventId( eventIdVal ),
                                                     work(0){}
  
private:
  /**@name Private Class Attributes of Event */
  //@{

  /// Time at which event was sent.
  const VTime *sendTime;

  /// Time at which event must be received and scheduled.
  const VTime *receiveTime;

  /// Id of the sender of the event.
  const ObjectID *sender;

  /// Id of the receiver of the event.
  const ObjectID *receiver;
  
  const EventId eventId;

  // for effective work estimator
  // reiher and jefferson (1990)
  int work;

  //@} // End of Event Private Class Methods.
};


#endif
