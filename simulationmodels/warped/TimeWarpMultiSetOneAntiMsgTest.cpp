#include "TimeWarpMultiSetOneAntiMsgTest.h"
#include "TimeWarpMultiSetOneAntiMsg.h"

void
TimeWarpMultiSetOneAntiMsgTest::setUp(){
  TimeWarpEventSetTest::setUp();
  toTest = new TimeWarpMultiSetOneAntiMsg( getSimManager() );
}

void
TimeWarpMultiSetOneAntiMsgTest::tearDown(){
  delete toTest;
  TimeWarpEventSetTest::tearDown();
}

void
TimeWarpMultiSetOneAntiMsgTest::testConstructor(){
  CPPUNIT_ASSERT( toTest != 0 );
}

void
TimeWarpMultiSetOneAntiMsgTest::testInsert(){
  TimeWarpEventSetTest::testInsert( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testGetEvent(){
  TimeWarpEventSetTest::testGetEvent( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testGetEventTime(){
  TimeWarpEventSetTest::testGetEventTime( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testPeekEvent(){
  TimeWarpEventSetTest::testPeekEvent( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testPeekEventTime(){
  TimeWarpEventSetTest::testPeekEventTime( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testHandleAntiMessage(){
  TimeWarpEventSetTest::testHandleAntiMessage( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testHandleOneAntiMessage(){
  TimeWarpEventSetTest::testHandleOneAntiMessage( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testRollback(){
  TimeWarpEventSetTest::testRollback( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testFossilCollect(){
  TimeWarpEventSetTest::testFossilCollect( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testFossilCollectInt(){
  TimeWarpEventSetTest::testFossilCollectInt( toTest );
}

void
TimeWarpMultiSetOneAntiMsgTest::testOfcPurge(){
  TimeWarpEventSetTest::testOfcPurge( toTest );
}
