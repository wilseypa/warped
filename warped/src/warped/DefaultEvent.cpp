// See copyright notice in file Copyright in the root directory of this archive.

#include "DefaultEvent.h"
#include "SerializedInstance.h"
#include "EventId.h"

DefaultEvent::~DefaultEvent(){
  delete sendTime;
  delete receiveTime;
  delete sender;
  delete receiver;
}

