#include "DefaultTimeWarpEventSetOneAntiMsgTest.h"
#include "DefaultTimeWarpEventSet.h"

void
DefaultTimeWarpEventSetOneAntiMsgTest::setUp(){
  TimeWarpEventSetTest::setUp();
  toTest = new DefaultTimeWarpEventSet( getSimManager(), true );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::tearDown(){
  delete toTest;
  TimeWarpEventSetTest::tearDown();
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testConstructor(){
  CPPUNIT_ASSERT( toTest != 0 );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testInsert(){
  TimeWarpEventSetTest::testInsert( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testGetEvent(){
  TimeWarpEventSetTest::testGetEvent( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testGetEventTime(){
  TimeWarpEventSetTest::testGetEventTime( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testPeekEvent(){
  TimeWarpEventSetTest::testPeekEvent( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testPeekEventTime(){
  TimeWarpEventSetTest::testPeekEventTime( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testHandleAntiMessage(){
  TimeWarpEventSetTest::testHandleAntiMessage( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testHandleOneAntiMessage(){
  TimeWarpEventSetTest::testHandleOneAntiMessage( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testRollback(){
  TimeWarpEventSetTest::testRollback( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testFossilCollect(){
  TimeWarpEventSetTest::testFossilCollect( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testFossilCollectInt(){
  TimeWarpEventSetTest::testFossilCollectInt( toTest );
}

void
DefaultTimeWarpEventSetOneAntiMsgTest::testOfcPurge(){
  TimeWarpEventSetTest::testOfcPurge( toTest );
}
