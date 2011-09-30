#ifndef EVENT_FUNCTORS_H
#define EVENT_FUNCTORS_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <functional>
#include "Event.h"
#include "EventId.h"

/**
   Functor.  Compares two events to see if one has a receive time less than
   the other.
*/
struct receiveTimeLessThan : public std::binary_function<const Event *, 
	    const Event *, bool > {
  bool operator()( const Event *a, const Event *b ){
    return a->getReceiveTime() < b->getReceiveTime();
  }
};

/**
   Functor.  Compares two events to see if one has a receive time greater
   than or equal to the other.
*/
class receiveTimeGreaterThanEqual : 
  public std::unary_function<const Event *, bool > {
public:
  receiveTimeGreaterThanEqual( const VTime &initCompareTime ) : 
    compareTime( initCompareTime ){}
  bool operator()( const Event *a ){
    return a->getReceiveTime() >= compareTime;
  }
private:
  const VTime &compareTime;
};

/**
   Functor.  Compares two events to see if one has a send time greater
   than or equal to the other.
*/
class sendTimeGreaterThanEqual : 
  public std::unary_function<const Event *, bool > {
public:
  sendTimeGreaterThanEqual( const VTime &initCompareTime ) : 
    compareTime( initCompareTime ){}
  bool operator()( const Event *a ){
    return a->getSendTime() >= compareTime;
  }
private:
  const VTime &compareTime;
};

/**
   Functor.  Compares two events to see if one has a send time less
   than or equal to the other.
*/
class sendTimeLessThanEqual : 
  public std::unary_function<const Event *, bool > {
public:
  sendTimeLessThanEqual( const VTime &initCompareTime ) : 
    compareTime( initCompareTime ){}
  bool operator()( const Event *a ){
    return a->getSendTime() <= compareTime;
  }
private:
  const VTime &compareTime;
};

/**
   Functor.  Compares two events to see if one has a send time less
   than the other.
*/
class sendTimeLessThan : 
  public std::unary_function<const Event *, bool > {
public:
  sendTimeLessThan( const VTime &initCompareTime ) : 
    compareTime( initCompareTime ){}
  bool operator()( const Event *a ){
    return a->getSendTime() < compareTime;
  }
private:
  const VTime &compareTime;
};

/**
   Functor.  Compares two events to see if one has a receive time less than
   or equal to the other.
*/
class receiveTimeLessThanEqual : 
  public std::unary_function<const Event *, bool > {
public:
  receiveTimeLessThanEqual( const VTime &initCompareTime ) : 
    compareTime( initCompareTime ){}
  bool operator()( const Event *a ){
    return a->getReceiveTime() <= compareTime;
  }
private:
  const VTime &compareTime;
};

/**
   Functor.  Compares two events to see if one has a receive time less than
   or equal to the other. If the times are equal, it then compares the event
   IDs to see if one is lower than the other. If these previous two are equal,
   then compare the event object sender strings. The eventId and sender string
   will uniquely identify events. It does not matter what the string is, this
   sort is only used to keep events in order from one sort to the next.
*/
class receiveTimeLessThanEventIdLessThan :
  public std::binary_function<const Event *, const Event *, bool > {
public:
  bool operator()( const Event *a, const Event *b ){
    if(a->getReceiveTime() != b->getReceiveTime()){
      return a->getReceiveTime() < b->getReceiveTime();
    }
    else{
      if(a->getEventId() != b->getEventId()){
        return a->getEventId() < b->getEventId();
      }
      else{
        return a->getSender() < b->getSender();
      }
    }    
  }
};
#endif
