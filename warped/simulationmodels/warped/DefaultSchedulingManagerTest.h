#ifndef DEFAULT_SCHEDULING_MANAGER_TEST_H
#define DEFAULT_SCHEDULING_MANAGER_TEST_H

#include "SchedulingManagerTest.h"

class DefaultSchedulingManager;

class DefaultSchedulingManagerTest : public SchedulingManagerTest {
  CPPUNIT_TEST_SUITE( DefaultSchedulingManagerTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testGetNextEvent );
  CPPUNIT_TEST_SUITE_END();

public:
  DefaultSchedulingManagerTest();

  void setUp();
  void teardown();

  void testConstructor();
  void testGetNextEvent();

protected:  

private:
  SchedulingManager *toTest;

};

#endif
