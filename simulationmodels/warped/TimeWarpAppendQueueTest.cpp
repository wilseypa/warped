#include "TimeWarpAppendQueueTest.h"
#include "warped/TimeWarpAppendQueue.h"


void 
TimeWarpAppendQueueTest::setUp(){
  toTest = new TimeWarpAppendQueue( new TWAQTimeWarpSimulationManager() );
}

void 
TimeWarpAppendQueueTest::tearDown(){
  delete toTest;
}

void 
TimeWarpAppendQueueTest::testConstructor(){
  CPPUNIT_ASSERT( toTest != 0 );
}


void 
TimeWarpAppendQueueTest::testInsert(){
  TimeWarpEventSetTest::testInsert( toTest );
}

void 
TimeWarpAppendQueueTest::testGetEvent(){
  TimeWarpEventSetTest::testGetEvent( toTest );
}

void 
TimeWarpAppendQueueTest::testPeekEvent(){
  TimeWarpEventSetTest::testPeekEvent( toTest );
}

void 
TimeWarpAppendQueueTest::testHandleAntiMessage(){
  TimeWarpEventSetTest::testHandleAntiMessage( toTest );
}

