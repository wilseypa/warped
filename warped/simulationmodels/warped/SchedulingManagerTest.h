#ifndef SCHEDULING_MANAGER_TEST_H
#define SCHEDULING_MANAGER_TEST_H

#include "WarpedUnitTest.h"

class SchedulingManager;

class SchedulingManagerTest : public WarpedUnitTest {
public:
  SchedulingManagerTest();

  void setUp(){ WarpedUnitTest::setUp(); }
  void tearDown(){ WarpedUnitTest::tearDown(); }

  virtual void testConstructor() = 0;
  virtual void testGetNextEvent() = 0;

protected:  
  void testConstructor( SchedulingManager *toTest );
  void testGetNextEvent( SchedulingManager *toTest );
};

#endif
