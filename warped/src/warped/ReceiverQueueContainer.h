#ifndef RECEIVER_QUEUE_CONTAINER_H
#define RECEIVER_QUEUE_CONTAINER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "SenderQueueContainer.h"

class TimeWarpEventSet;

class ReceiverQueueContainer {
 public:
  ReceiverQueueContainer():eventSet(0), headEventPtr(0){};
  
  ~ReceiverQueueContainer() {}
  
  TimeWarpEventSet* eventSet;
  Event* headEventPtr;
};

#endif
