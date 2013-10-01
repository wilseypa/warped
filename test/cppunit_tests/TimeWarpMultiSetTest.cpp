#include "TimeWarpMultiSetTest.h"
#include "TimeWarpMultiSet.h"

void 
TimeWarpMultiSetTest::setUp(){
  TimeWarpEventSetTest::setUp();
  toTest = new TimeWarpMultiSet( getSimManager() );
}

void 
TimeWarpMultiSetTest::tearDown(){
  delete toTest;
  TimeWarpEventSetTest::tearDown();
}

void 
TimeWarpMultiSetTest::testConstructor(){
  CPPUNIT_ASSERT( toTest != 0 );
}

void 
TimeWarpMultiSetTest::testInsert(){
  TimeWarpEventSetTest::testInsert( toTest );
}

void
TimeWarpMultiSetTest::testGetEvent(){
  TimeWarpEventSetTest::testGetEvent( toTest );
}

void
TimeWarpMultiSetTest::testGetEventTime(){
  TimeWarpEventSetTest::testGetEventTime( toTest );
}

void
TimeWarpMultiSetTest::testPeekEvent(){
  TimeWarpEventSetTest::testPeekEvent( toTest );
}

void
TimeWarpMultiSetTest::testPeekEventTime(){
  TimeWarpEventSetTest::testPeekEventTime( toTest );
}

void
TimeWarpMultiSetTest::testHandleAntiMessage(){
  TimeWarpEventSetTest::testHandleAntiMessage( toTest );
}

void
TimeWarpMultiSetTest::testHandleOneAntiMessage(){
  // This test is only required for the one anti-message
  // event set implementations, do nothing here.
}

void
TimeWarpMultiSetTest::testRollback(){
  TimeWarpEventSetTest::testRollback( toTest );
}

void
TimeWarpMultiSetTest::testFossilCollect(){
  TimeWarpEventSetTest::testFossilCollect( toTest );
}

void
TimeWarpMultiSetTest::testFossilCollectInt(){
  TimeWarpEventSetTest::testFossilCollectInt( toTest );
}

void
TimeWarpMultiSetTest::testOfcPurge(){
  TimeWarpEventSetTest::testOfcPurge( toTest );
}
