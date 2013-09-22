#include "SchedulingManagerTest.h"
#include "warped/SchedulingManager.h"

SchedulingManagerTest::SchedulingManagerTest(){}

void
SchedulingManagerTest::testConstructor( SchedulingManager *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  CPPUNIT_ASSERT( toTest->peekNextEvent() == 0 );
}

void
SchedulingManagerTest::testGetNextEvent( SchedulingManager *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( getSimManager()->getEventSetManager() );
  const Event *event1 = toTest->peekNextEvent();
  CPPUNIT_ASSERT( event1 != 0 );
  CPPUNIT_ASSERT( toTest->getLastEventScheduledTime() == IntVTime(0) );
  CPPUNIT_ASSERT( event1 == getSimManager()->getEventSetManager()->getEvent( getSimObj1() ) );
  const Event *event2 = toTest->peekNextEvent();
  CPPUNIT_ASSERT( event2 != 0 );
  CPPUNIT_ASSERT( toTest->getLastEventScheduledTime() == IntVTime(0) );
  CPPUNIT_ASSERT( event2 == getSimManager()->getEventSetManager()->getEvent( getSimObj1() ) );
  const Event *event3 = toTest->peekNextEvent();
  CPPUNIT_ASSERT( event3 != 0 );
  CPPUNIT_ASSERT( toTest->getLastEventScheduledTime() == IntVTime(0) );
  CPPUNIT_ASSERT( event3 == getSimManager()->getEventSetManager()->getEvent( getSimObj1() ) );
  const Event *event4 = toTest->peekNextEvent();
  CPPUNIT_ASSERT( event4 != 0 );
  CPPUNIT_ASSERT( toTest->getLastEventScheduledTime() == IntVTime(1) );
  CPPUNIT_ASSERT( event4 == getSimManager()->getEventSetManager()->getEvent( getSimObj1() ) );
  
  checkDefaultEvents( event1, event2, event3, event4 );
}

