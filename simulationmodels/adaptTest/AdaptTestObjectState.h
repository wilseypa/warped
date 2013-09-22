#ifndef ADAPTTESTOBJECTSTATE_H
#define ADAPTTESTOBJECTSTATE_H

#include <State.h>
#include <string>

class AdaptTestObjectState : public State {
public :
  AdaptTestObjectState( bool initAdaptState ) :
    numEventsStarted( 0 ),
    numEventsReceived( 0 ),
    numEventsSent( 0 ),
    numStragglers( 0 ),
    adaptState( initAdaptState ){};

  ~AdaptTestObjectState(){};
  
  void copyState( const State *toCopy ){
    ASSERT( toCopy != 0 );
    const AdaptTestObjectState *AdaptTestState = dynamic_cast<const AdaptTestObjectState *>(toCopy);
    numEventsReceived = AdaptTestState->numEventsReceived;
    numEventsSent = AdaptTestState->numEventsSent;
    numEventsStarted = AdaptTestState->numEventsStarted;

    // This delay loop is used to create a suddenly long state save time.
    if(adaptState){
        if( numEventsReceived > 1000 ){
        double x = 35.39453234857;
        for(long double wait = -123456789.1234567890; wait < 4000000; wait = wait + 100){
          x = wait / x;
        }
      }
    }
  }

  unsigned int getStateSize() const { return sizeof(AdaptTestObjectState); }

  void eventStarted(){ numEventsStarted++; }
  void eventSent(){ numEventsSent++; }
  void eventReceived(){ numEventsReceived++; }
  void stragglerSent(){ numStragglers++; }

  int getNumStarted(){ return numEventsStarted; }
  int getNumReceived(){ return numEventsReceived; }
  int getNumSent(){ return numEventsSent; }
  int getNumStragglersSent(){ return numStragglers; }

  const string getSummaryString(){
    return "Received " + std::to_string(numEventsReceived) + ", Sent " +
      std::to_string(numEventsSent ) + ", and Started " + std::to_string(numEventsStarted);
  }

private:
  int numEventsStarted;
  int numEventsReceived;
  int numEventsSent;
  int numStragglers;
  bool adaptState;
};

#endif

