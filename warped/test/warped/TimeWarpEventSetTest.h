#ifndef TIME_WARP_EVENT_SET_TEST_H
#define TIME_WARP_EVENT_SET_TEST_H

#include "WarpedUnitTest.h"

class TimeWarpEventSet;

class TimeWarpEventSetTest : public WarpedUnitTest {
public:

  void setUp();
  void tearDown();
  
  virtual void testInsert( ) = 0;
  virtual void testGetEvent() = 0;
  virtual void testGetEventTime() = 0;
  virtual void testPeekEvent() = 0;
  virtual void testPeekEventTime() = 0;
  virtual void testHandleAntiMessage() = 0;
  virtual void testHandleOneAntiMessage() = 0;
  virtual void testRollback() = 0;
  virtual void testFossilCollect() = 0;
  virtual void testFossilCollectInt() = 0;
  virtual void testOfcPurge() = 0;

protected:  
  void testInsert( TimeWarpEventSet *toTest );
  void testGetEvent( TimeWarpEventSet *toTest );
  void testGetEventTime( TimeWarpEventSet *toTest );
  void testPeekEvent( TimeWarpEventSet *toTest );
  void testPeekEventTime( TimeWarpEventSet *toTest );
  void testHandleAntiMessage( TimeWarpEventSet *toTest );
  void testHandleOneAntiMessage( TimeWarpEventSet *toTest );
  void testRollback( TimeWarpEventSet *toTest );
  void testFossilCollect( TimeWarpEventSet *toTest );
  void testFossilCollectInt( TimeWarpEventSet *toTest );
  void testOfcPurge( TimeWarpEventSet *toTest );

private:
  void testHandleAntiMessageInit( TimeWarpEventSet *toTest );

};

#endif
