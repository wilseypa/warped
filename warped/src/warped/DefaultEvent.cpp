
#include "DefaultEvent.h"
#include "SerializedInstance.h"
#include "EventId.h"

DefaultEvent::~DefaultEvent(){
  delete sendTime;
  delete receiveTime;
  delete sender;
  delete receiver;
}

