#ifndef SENDER_QUEUE_CONTAINER_H
#define SENDER_QUEUE_CONTAINER_H


#include "warped.h"
#include "Event.h"
#include <list>
using std::list;

class SenderQueueContainer {
 public:
  SenderQueueContainer():senderQ(new list<Event*>), headEventPtr(0){};
  
  ~SenderQueueContainer() {
    senderQ->clear();
    delete senderQ;
  }
  
  list<Event*> *senderQ;
  Event* headEventPtr;
};

class ScheduleListContainer {
 public:
  ScheduleListContainer() : headEventPtr(0) {};

  ScheduleListContainer(Event** ptr) : headEventPtr(ptr) {};

  ~ScheduleListContainer() {};

  friend bool operator>(const ScheduleListContainer& lhs, const ScheduleListContainer& rhs) {
    if(lhs.headEventPtr != 0) {
      if(rhs.headEventPtr != 0) {
	return ((*(lhs.headEventPtr))->getReceiveTime() > 
		(*(rhs.headEventPtr))->getReceiveTime());
      }
      else {
	return false;
      }
    }
    else {
      if(rhs.headEventPtr != 0) {
	return true;
      }
    }
    return false;
  }

  friend bool GreaterThanWithReceiverID(const ScheduleListContainer& lhs, const ScheduleListContainer& rhs) {
    if(lhs.headEventPtr != 0) {
      if(rhs.headEventPtr != 0) {
	if((*(lhs.headEventPtr))->getReceiveTime() > 
	   (*(rhs.headEventPtr))->getReceiveTime()) {
	  return true;
	}
	else if((*(lhs.headEventPtr))->getReceiveTime() == 
		(*(rhs.headEventPtr))->getReceiveTime()) {
	  return ((*(lhs.headEventPtr))->getReceiver() > 
		  (*(rhs.headEventPtr))->getReceiver());
	}
	else {
	  return false;
	}
      }
      else {
	return false;
      }
    }
    else {
      if(rhs.headEventPtr != 0) {
	return true;
      }
    }
    return false;
  }

  Event** headEventPtr;
};

class GreaterThan_ScheduleListContainer {
 public:
  bool operator() (const ScheduleListContainer* lhs, const ScheduleListContainer* rhs) const {
    return (*lhs) > (*rhs);
  }
};

class GreaterThan_ScheduleListContainerWithReceiverID {
 public:
  bool operator() (const ScheduleListContainer* lhs, const ScheduleListContainer* rhs) const {
    return GreaterThanWithReceiverID(*lhs, *rhs);
  }
};

#endif
