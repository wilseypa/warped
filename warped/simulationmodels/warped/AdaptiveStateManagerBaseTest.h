#ifndef ADAPTIVE_STATE_MANAGER_BASE_TEST_H
#define ADAPTIVE_STATE_MANAGER_BASE_TEST_H

#include "WarpedUnitTest.h"
#include "warped/AdaptiveStateManagerBase.h"

class AdaptiveStateManagerBase;

class AdaptiveStateManagerBaseTest : public WarpedUnitTest {
  CPPUNIT_TEST_SUITE( AdaptiveStateManagerBaseTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSaveState );
  CPPUNIT_TEST( testRestoreState );
  CPPUNIT_TEST( testFossilCollect );
  CPPUNIT_TEST_SUITE_END();

public:

  class ASMBTestClass : public AdaptiveStateManagerBase {
  public:
     ASMBTestClass( TimeWarpSimulationManager *simMgr ) 
        :AdaptiveStateManagerBase(simMgr) {}
     ~ASMBTestClass() {}

     void setPeriod( SimulationObject *object, long period ) { 
       OBJECT_ID *currentObjectID = object->getObjectID();
       unsigned int simObjectID = currentObjectID->getSimulationObjectID();
       objectStatePeriod[simObjectID] = period; 
     }
     multiset< SetObject<State> > *getStateQueue() { return myStateQueue; }
  };

  AdaptiveStateManagerBaseTest();

  void setUp();
  void tearDown();
  void defaultSaveState();

  void testConstructor();
  void testSaveState();
  void testRestoreState();
  void testFossilCollect();

protected:

private:
   StateManager *toTest;  
};

#endif

