#include "DefaultSchedulingManagerTest.h"
#include "warped/DefaultSchedulingManager.h"

DefaultSchedulingManagerTest::DefaultSchedulingManagerTest()
 : toTest(0){}

void 
DefaultSchedulingManagerTest::setUp(){
  SchedulingManagerTest::setUp();
  toTest = new DefaultSchedulingManager( getSimManager() );
}

void
DefaultSchedulingManagerTest::teardown(){
  delete toTest;
  toTest = 0;
  SchedulingManagerTest::tearDown();
}

void
DefaultSchedulingManagerTest::testConstructor(){
  SchedulingManagerTest::testConstructor( toTest );
}

void
DefaultSchedulingManagerTest::testGetNextEvent(){
  SchedulingManagerTest::testGetNextEvent( toTest );
}





