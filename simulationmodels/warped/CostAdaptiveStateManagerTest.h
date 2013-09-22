#ifndef COST_ADAPTIVE_STATE_MANAGER_TEST_H
#define COST_ADAPTIVE_STATE_MANAGER_TEST_H

#include "WarpedUnitTest.h"
#include "warped/CostAdaptiveStateManager.h"

//#include <set>
//using std::multiset;

class CostAdaptiveStateManager;

class CostAdaptiveStateManagerTest : public WarpedUnitTest {
  CPPUNIT_TEST_SUITE( CostAdaptiveStateManagerTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSaveState );
  CPPUNIT_TEST( testCalculatePeriod );
  CPPUNIT_TEST_SUITE_END();

public:

  class CASMTestClass : public CostAdaptiveStateManager {
  public:
     CASMTestClass( TimeWarpSimulationManager *simMgr ) 
        :CostAdaptiveStateManager(simMgr) {}
     ~CASMTestClass() {}

     vector<long> getForwardExecutionLength() { return forwardExecutionLength; }
     
     void setPeriod( SimulationObject *object, long period ) {
       OBJECT_ID *currentObjectID = object->getObjectID();
       unsigned int simObjectID = currentObjectID->getSimulationObjectID();
       objectStatePeriod[simObjectID] = period;
     }  
  };

  CostAdaptiveStateManagerTest();

  void setUp();
  void tearDown();

  void testConstructor();
  void testSaveState();
  void testCalculatePeriod();

protected:

private:
   StateManager *toTest;  
};

#endif

