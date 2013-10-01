#ifndef TIME_WARP_MULIT_SET_TEST_H
#define TIME_WARP_MULTI_SET_TEST_H

#include "TimeWarpEventSetTest.h"
#include "TimeWarpSimulationManagerAdapter.h"

class TimeWarpMultiSet;

class TimeWarpMultiSetTest : public TimeWarpEventSetTest {
  CPPUNIT_TEST_SUITE( TimeWarpMultiSetTest );
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
  TimeWarpMultiSet *toTest;

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
