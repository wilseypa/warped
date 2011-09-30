#include "WarpedUnitTest.h"

Event *
WarpedUnitTest::getEvent1(){
  static Event *event1 = new UnitTestEvent( IntVTime::getIntVTimeZero(), getSimObj1ID() );
  return event1;
}

Event *
WarpedUnitTest::getEvent2(){
  static Event *event2 = new UnitTestEvent( IntVTime::getIntVTimeZero(), getSimObj1ID() );
  return event2;
}

Event *
WarpedUnitTest::getEvent3(){
  static Event *event3 = new UnitTestEvent( *new IntVTime( 1 ), getSimObj1ID() );
  return event3;
}

Event *
WarpedUnitTest::getEvent4(){
  static Event *event4 = new UnitTestEvent( IntVTime::getIntVTimeZero(), getSimObj1ID() );
  return event4;
}

vector<Event *> &
WarpedUnitTest::getDefaultEvents(){
  static vector<Event *> *retval = new vector<Event *>;
  if( retval->size() == 0 ){
    retval->push_back( getEvent1() );
    retval->push_back( getEvent2() );
    retval->push_back( getEvent3() );
    retval->push_back( getEvent4() );
  }

  return *retval;
}

void
WarpedUnitTest::doDefaultInsert( TimeWarpEventSet *toTest ){
  vector<Event *> &events = getDefaultEvents();
  
  bool inThePast = true;
  for( vector<Event *>::iterator currentEvent = events.begin();
       currentEvent < events.end();
       currentEvent++ ){
    inThePast = toTest->insert( *currentEvent );
    CPPUNIT_ASSERT( inThePast == false );
  }
}

void
WarpedUnitTest::checkDefaultEvents( const Event *event1,
					  const Event *event2,
					  const Event *event3,
					  const Event *event4 ){
  if( event1 != 0 ){
    CPPUNIT_ASSERT( event1->getReceiveTime() == IntVTime(0) );
  }
  if( event2 != 0 ){
    CPPUNIT_ASSERT( event2 != event1 );
    CPPUNIT_ASSERT( event2->getReceiveTime() == IntVTime(0) );
  }
  if( event3 != 0 ){
    CPPUNIT_ASSERT(  event3 != event2 && event3 != event1 );
    CPPUNIT_ASSERT( event3->getReceiveTime() == IntVTime(0) );
  }
  if( event4 != 0 ){
    CPPUNIT_ASSERT( event4 == getEvent3() );
  }
}

