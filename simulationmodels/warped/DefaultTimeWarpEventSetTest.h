#ifndef DEFAULT_TIME_WARP_EVENT_TEST_H
#define DEFAULT_TIME_WARP_EVENT_TEST_H

#include "TimeWarpEventSetTest.h"
#include "TimeWarpSimulationManagerAdapter.h"

class DefaultTimeWarpEventSet;

class DefaultTimeWarpEventSetTest : public TimeWarpEventSetTest {
  CPPUNIT_TEST_SUITE( DefaultTimeWarpEventSetTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testInsert );
  CPPUNIT_TEST( testGetEvent );
  CPPUNIT_TEST( testGetEventTime );
  CPPUNIT_TEST( testPeekEvent );
  CPPUNIT_TEST( testPeekEventTime );
  CPPUNIT_TEST( testHandleAntiMessage );
  CPPUNIT_TEST( testHandleOneAntiMessage );
  CPPUNIT_TEST( testRollback );
  CPPUNIT_TEST( testFossilCollect );
  CPPUNIT_TEST( testFossilCollectInt );
  CPPUNIT_TEST( testOfcPurge );
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
