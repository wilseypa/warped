#ifndef TIME_WARP_APPEND_QUEUE_TEST_H
#define TIME_WARP_APPEND_QUEUE_TEST_H

#include "TimeWarpEventSetTest.h"
#include "TimeWarpSimulationManagerAdapter.h"

class TimeWarpAppendQueue;

class TimeWarpAppendQueueTest : public TimeWarpEventSetTest {
  CPPUNIT_TEST_SUITE( TimeWarpAppendQueueTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testGetEvent );
  CPPUNIT_TEST( testHandleAntiMessage );
  CPPUNIT_TEST( testInsert );
  CPPUNIT_TEST( testPeekEvent );
  CPPUNIT_TEST_SUITE_END();

  class TWAQTimeWarpSimulationManager : public TimeWarpSimulationManagerAdapter {
  public:
    TWAQTimeWarpSimulationManager(){}
    ~TWAQTimeWarpSimulationManager(){}
  };

private:
  TimeWarpAppendQueue *toTest;


public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testInsert();
  void testGetEvent();
  void testPeekEvent();
  void testHandleAntiMessage();

};

#endif
