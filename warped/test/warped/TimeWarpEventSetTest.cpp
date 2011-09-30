#include "TimeWarpEventSetTest.h"
#include "warped/TimeWarpEventSet.h"
#include "warped/IntVTime.h"


void
TimeWarpEventSetTest::setUp(){ 
  WarpedUnitTest::setUp(); 
}

void 
TimeWarpEventSetTest::tearDown(){
  WarpedUnitTest::tearDown();
}

void 
TimeWarpEventSetTest::testInsert( TimeWarpEventSet *toTest ){
  doDefaultInsert( toTest );
}

void 
TimeWarpEventSetTest::testGetEvent( TimeWarpEventSet *toTest ){
  // No events have been inserted yet...
  CPPUNIT_ASSERT( toTest != 0 );
  const Event *nullEvent = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( nullEvent == 0 );

  doDefaultInsert( toTest );

  const Event *event1 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );

  const Event *event2 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event2 != 0 );

  const Event *event3 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event3 != 0 && event3 != event2 && event3 != event1 );

  const Event *event4 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event4 != 0 );

  checkDefaultEvents( event1, event2, event3, event4 );
}

void 
TimeWarpEventSetTest::testGetEventTime( TimeWarpEventSet *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );
  const Event *event1 = toTest->getEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( event1 != 0 );
  const Event *event2 = toTest->getEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( event2 != 0 );
  const Event *event3 = toTest->getEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( event3 != 0 );
  checkDefaultEvents( event1, event2, event3, 0 );
  
  const Event *nullEvent = toTest->getEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( nullEvent == 0 );
}

void 
TimeWarpEventSetTest::testPeekEvent( TimeWarpEventSet *toTest ){
  // No events have been inserted yet...
  CPPUNIT_ASSERT( toTest != 0 );
  const Event *nullEvent = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( nullEvent == 0 );

  doDefaultInsert( toTest );

  const Event *event1 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );
  CPPUNIT_ASSERT( event1 == toTest->getEvent( getSimObj1() ) );

  const Event *event2 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event2 != 0 );
  CPPUNIT_ASSERT( event2 == toTest->getEvent( getSimObj1() ) );

  const Event *event3 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event3 != 0 );
  CPPUNIT_ASSERT( event3 == toTest->getEvent( getSimObj1() ) );

  const Event *event4 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event4 != 0 );
  CPPUNIT_ASSERT( event4 == toTest->getEvent( getSimObj1() ) );
  
  checkDefaultEvents( event1, event2, event3, event4 );
}

void 
TimeWarpEventSetTest::testPeekEventTime( TimeWarpEventSet *toTest ){
  // No events have been inserted yet...
  CPPUNIT_ASSERT( toTest != 0 );
  const Event *nullEvent = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( nullEvent == 0 );

  doDefaultInsert( toTest );

  const Event *event1 = toTest->peekEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( event1 != 0 );
  CPPUNIT_ASSERT( event1 == toTest->getEvent( getSimObj1() ) );

  const Event *event2 = toTest->peekEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( event2 != 0 );
  CPPUNIT_ASSERT( event2 == toTest->getEvent( getSimObj1() ) );

  const Event *event3 = toTest->peekEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( event3 != 0 );
  CPPUNIT_ASSERT( event3 == toTest->getEvent( getSimObj1() ) );

  nullEvent = toTest->peekEvent( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( nullEvent == 0 );
  
  checkDefaultEvents( event1, event2, event3, 0 );
}

void
TimeWarpEventSetTest::testHandleAntiMessageInit( TimeWarpEventSet *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  const Event *nullEvent = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( nullEvent == 0 );

  doDefaultInsert( toTest );
  const Event *event1 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );
  CPPUNIT_ASSERT( event1->getReceiveTime() == IntVTime(0) );
}

void
TimeWarpEventSetTest::testHandleAntiMessage( TimeWarpEventSet *toTest ){
  testHandleAntiMessageInit( toTest );

  NegativeEvent *neg = new NegativeEvent(getEvent1()->getSendTime(), getEvent1()->getReceiveTime(),
                                         getEvent1()->getSender(), getEvent1()->getReceiver(),
                                         getEvent1()->getEventId());

  toTest->handleAntiMessage( getSimObj1(), neg );

  const Event *event1 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );
  CPPUNIT_ASSERT( event1 != getEvent1() );
  CPPUNIT_ASSERT( event1->getReceiveTime() == IntVTime(0) );
}

void
TimeWarpEventSetTest::testHandleOneAntiMessage( TimeWarpEventSet *toTest ){
  vector<Event*> events;
  for(int i = 1; i <= 5; i++){
    events.push_back(new UnitTestEvent( *(new IntVTime(i+1)), getSimObj1ID(), *(new IntVTime(i))));
    toTest->insert(events[i-1]);
  }

  NegativeEvent *neg = new NegativeEvent(events[2]->getSendTime(), events[2]->getReceiveTime(),
                                         events[2]->getSender(), events[2]->getReceiver(),
                                         events[2]->getEventId());

  toTest->handleAntiMessage( getSimObj1(), neg );

  const Event *event0 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event0 != 0 );
  CPPUNIT_ASSERT( event0 == events[0] );

  const Event *event1 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );
  CPPUNIT_ASSERT( event1 == events[1] );

  const Event *event2 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event2 == 0 );
}

void
TimeWarpEventSetTest::testRollback( TimeWarpEventSet *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );

  const Event *event1 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );

  const Event *event2 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event2 != 0 );

  const Event *event3 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event3 != 0 );

  const Event *event4 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event4 != 0 );

  checkDefaultEvents( event1, event2, event3, event4 );

  toTest->rollback( getSimObj1(), event1->getReceiveTime() );
  
  // OK, we rolled back to zero, now we should be able to repeat the sequence

  event1 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );
  CPPUNIT_ASSERT( event1->getReceiveTime() == IntVTime(0) );

  event2 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event2 != 0 && event2 != event1 );
  CPPUNIT_ASSERT( event2->getReceiveTime() == IntVTime(0) );

  event3 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event3 != 0 && event3 != event1 && event3 != event2 );
  CPPUNIT_ASSERT( event3->getReceiveTime() == IntVTime(0) );

  event4 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event4 != 0 && event4 != event1 && event4 != event2 && event4 != event3 );
  CPPUNIT_ASSERT( event4->getReceiveTime() == IntVTime(1) );
}

void
TimeWarpEventSetTest::testFossilCollect( TimeWarpEventSet *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );
  const Event *event1 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );
  const Event *event2 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event2 != 0 );
  const Event *event3 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event3 != 0 );
  const Event *event4 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event4 != 0 );

  checkDefaultEvents( event1, event2, event3, event4 );
  toTest->fossilCollect( getSimObj1(), IntVTime( 1 ) );
  toTest->rollback( getSimObj1(), IntVTime( 0 ) );
  event4 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( toTest != 0 );
  checkDefaultEvents( 0, 0, 0, event4 );  
}

void
TimeWarpEventSetTest::testFossilCollectInt( TimeWarpEventSet *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );
  const Event *event1 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );
  const Event *event2 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event2 != 0 );
  const Event *event3 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event3 != 0 );
  const Event *event4 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( event4 != 0 );

  checkDefaultEvents( event1, event2, event3, event4 );
  toTest->fossilCollect( getSimObj1(), 1 );
  toTest->rollback( getSimObj1(), IntVTime(0) );
  event4 = toTest->getEvent( getSimObj1() );
  CPPUNIT_ASSERT( toTest != 0 );
  checkDefaultEvents( 0, 0, 0, event4 );
}

void
TimeWarpEventSetTest::testOfcPurge( TimeWarpEventSet *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );

  const Event *event1 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 != 0 );

  toTest->ofcPurge();

  event1 = toTest->peekEvent( getSimObj1() );
  CPPUNIT_ASSERT( event1 == 0 );
}
