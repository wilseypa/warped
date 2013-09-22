#ifndef ADAPTTESTEVENT_HH
#define ADAPTTESTEVENT_HH

#include <warped/DefaultEvent.h>

class AdaptTestEvent : public DefaultEvent {
public:
  /**
     Constructor called by application.
  */
  AdaptTestEvent( const VTime &initSendTime,
                  const VTime &initRecvTime,
                  SimulationObject *initSender,
                  SimulationObject *initReceiver ) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ){}
  
  ~AdaptTestEvent(){}

  unsigned int getEventSize() const { return sizeof(AdaptTestEvent); }
  
  static Serializable *deserialize( SerializedInstance *instance );

  void serialize( SerializedInstance * ) const;
  
  bool eventCompare(const Event* event);

  static const string &getAdaptTestEventDataType(){
    static string AdaptTestEventDataType = "AdaptTestEvent";
    return AdaptTestEventDataType;
  }
  
  const string &getDataType() const {
    return getAdaptTestEventDataType();
  }

private:
  /*
     Constructor called by deserializer.
  */
  AdaptTestEvent( const VTime &initSendTime,
                  const VTime &initRecvTime,
                  const ObjectID &initSender,
                  const ObjectID &initReceiver,
                  const unsigned int eventIdVal ) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver, eventIdVal ){}

};

#endif

