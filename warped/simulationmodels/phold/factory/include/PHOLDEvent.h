#ifndef PHOLD_EVENT_H
#define PHOLD_EVENT_H

#include "warped.h"
#include <warped/DefaultEvent.h>

class PHOLDEvent: public DefaultEvent {
public:

  // How many processes have touched this event
  int numberOfHops;
  int eventNumber;

  PHOLDEvent(const VTime& sendTime,
             const VTime& recvTime,
             SimulationObject *sender,
             SimulationObject *receiver)
   :DefaultEvent(sendTime, recvTime, sender, receiver){
    numberOfHops = 0;
    eventNumber = 0;
  }

  unsigned int getEventSize() const { return sizeof(PHOLDEvent); }

  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize(SerializedInstance* instance);
  
  bool eventCompare(const Event* event);

  static const string &getPHOLDEventDataType(){
    static string PHOLDEventDataType = "PHOLDEvent";
    return PHOLDEventDataType;
  }

  const string &getDataType() const {
    return getPHOLDEventDataType();
  }

  friend ostream& operator<< (ostream& os, const PHOLDEvent& pe);

private:

  //Constructor to be used by the deserializer
  PHOLDEvent(const VTime& sendTime,
             const VTime& recvTime,
             const ObjectID &sender,
             const ObjectID &receiver,
             const unsigned int IDVal,
             const unsigned int numHops,
             const unsigned int evtNum)
   :DefaultEvent(sendTime, recvTime, sender, receiver, IDVal){
    numberOfHops = numHops;
    eventNumber = evtNum;
  }
};


#endif
