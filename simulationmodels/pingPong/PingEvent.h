#ifndef PINGEVENT_HH
#define PINGEVENT_HH

#include <warped/DefaultEvent.h>

class PingEvent : public DefaultEvent {
public:
  /**
     Constructor called by application.
  */
  PingEvent( const VTime &initSendTime,
	     const VTime &initRecvTime,
	     SimulationObject *initSender,
	     SimulationObject *initReceiver,
	     const string &owner ) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
    myOwner( owner ){}

  
  ~PingEvent(){}

  unsigned int getEventSize() const { return sizeof(PingEvent); } 
  
  static Serializable *deserialize( SerializedInstance *instance );

  void serialize( SerializedInstance * ) const;
  
  bool eventCompare(const Event* event);

  static const string &getPingEventDataType(){
    static string pingEventDataType = "PingEvent";
    return pingEventDataType;
  }
  
  const string &getDataType() const {
    return getPingEventDataType();
  }

  const string &getOwner() const { return myOwner; }

private:
  /*
     Constructor called by deserializer.
  */
  PingEvent( const VTime &initSendTime,
	     const VTime &initRecvTime,
	     const ObjectID &initSender,
	     const ObjectID &initReceiver,
	     const unsigned int eventIdVal,
	     const string &owner ) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver, eventIdVal ),
    myOwner( owner ){}


  const string myOwner;
};

#endif
