#include "DefaultTimeWarpEventSetTest.h"
#include "DefaultTimeWarpEventSet.h"


void 
DefaultTimeWarpEventSetTest::setUp(){
  TimeWarpEventSetTest::setUp();
  toTest = new DefaultTimeWarpEventSet( getSimManager(), false );
}

void 
DefaultTimeWarpEventSetTest::tearDown(){
  delete toTest;
  TimeWarpEventSetTest::tearDown();
}

void 
DefaultTimeWarpEventSetTest::testConstructor(){
  CPPUNIT_ASSERT( toTest != 0 );
}


void 
DefaultTimeWarpEventSetTest::testInsert(){
  TimeWarpEventSetTest::testInsert( toTest );
}

void
DefaultTimeWarpEventSetTest::testGetEvent(){
  TimeWarpEventSetTest::testGetEvent( toTest );
}

void
DefaultTimeWarpEventSetTest::testGetEventTime(){
  TimeWarpEventSetTest::testGetEventTime( toTest );
}

void
DefaultTimeWarpEventSetTest::testPeekEvent(){
  TimeWarpEventSetTest::testPeekEvent( toTest );
}

void
DefaultTimeWarpEventSetTest::testPeekEventTime(){
  TimeWarpEventSetTest::testPeekEventTime( toTest );
}

void
DefaultTimeWarpEventSetTest::testHandleAntiMessage(){
  TimeWarpEventSetTest::testHandleAntiMessage( toTest );
}

void
DefaultTimeWarpEventSetTest::testHandleOneAntiMessage(){
  // This test is only required for the one anti-message
  // event set implementations, do nothing here.
}

void
DefaultTimeWarpEventSetTest::testRollback(){
  TimeWarpEventSetTest::testRollback( toTest );
}

void
DefaultTimeWarpEventSetTest::testFossilCollect(){
  TimeWarpEventSetTest::testFossilCollect( toTest );
}

void
DefaultTimeWarpEventSetTest::testFossilCollectInt(){
  TimeWarpEventSetTest::testFossilCollectInt( toTest );
}

void
DefaultTimeWarpEventSetTest::testOfcPurge(){
  TimeWarpEventSetTest::testOfcPurge( toTest );
}
