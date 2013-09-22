#ifndef DEFAULT_TIME_WARP_EVENT_ONE_TEST_H
#define DEFAULT_TIME_WARP_EVENT_ONE_TEST_H

#include "TimeWarpEventSetTest.h"
#include "TimeWarpSimulationManagerAdapter.h"

class DefaultTimeWarpEventSet;

class DefaultTimeWarpEventSetOneAntiMsgTest : public TimeWarpEventSetTest {
  CPPUNIT_TEST_SUITE( DefaultTimeWarpEventSetOneAntiMsgTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testHandleOneAntiMessage );
  CPPUNIT_TEST_SUITE_END();

private:
  DefaultTimeWarpEventSet *toTest;

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testInsert();
  void testGetEvent();
  void testGetEventTime();
  void testPeekEvent();
  void testPeekEventTime();
  void testHandleAntiMessage();
  void testHandleOneAntiMessage();
  void testRollback();
  void testFossilCollect();
  void testFossilCollectInt();
  void testOfcPurge();

};

#endif
