#ifndef TIME_WARP_MULIT_SET_ONE_ANTI_MSG_TEST_H
#define TIME_WARP_MULTI_SET_ONE_ANTI_MSG_TEST_H

#include "TimeWarpEventSetTest.h"
#include "TimeWarpSimulationManagerAdapter.h"

class TimeWarpMultiSetOneAntiMsg;

class TimeWarpMultiSetOneAntiMsgTest : public TimeWarpEventSetTest {
  CPPUNIT_TEST_SUITE( TimeWarpMultiSetOneAntiMsgTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testHandleOneAntiMessage );
  CPPUNIT_TEST_SUITE_END();

private:
  TimeWarpMultiSetOneAntiMsg *toTest;

public:
  // Test interfaces
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
