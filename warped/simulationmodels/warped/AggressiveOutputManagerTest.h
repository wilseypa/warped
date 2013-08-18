#ifndef AGGRESSIVE_OUTPUT_MANAGER_TEST_H
#define AGGRESSIVE_OUTPUT_MANAGER_TEST_H

#include "OutputManagerTest.h"

class AggressiveOutputManager;

class AggressiveOutputManagerTest : public OutputManagerTest {
  CPPUNIT_TEST_SUITE( AggressiveOutputManagerTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testInsert );
  CPPUNIT_TEST( testRollback );
  CPPUNIT_TEST( testFossilCollect );
  CPPUNIT_TEST( testFossilCollectInt );
  CPPUNIT_TEST( testOfcPurge );
  CPPUNIT_TEST_SUITE_END();

  class AOMSimMgr : public UnitTestSimulationManager {
  public:
    AOMSimMgr( SimulationObject *initObj ) :
      UnitTestSimulationManager( initObj ){}
    ~AOMSimMgr(){}

    void cancelEvents( const vector<const Event *> &eventsToCancel ){
      negativeEvents.insert( negativeEvents.begin(),
			     eventsToCancel.begin(),
			     eventsToCancel.end() );
			    
    }

    unsigned int negativeCount(){
      return negativeEvents.size();
    }

    void checkNegativeEvents(){
      CPPUNIT_ASSERT( negativeEvents.size() == getDefaultEvents().size() );
      vector<const Event *>::iterator i = find( negativeEvents.begin(),
						negativeEvents.end(),
						getEvent3() );
      CPPUNIT_ASSERT( i != negativeEvents.end() );
      const Event *event4 = *i;
      CPPUNIT_ASSERT( event4 != 0 );
      negativeEvents.erase( i );
      const Event *event1 = negativeEvents[0];
      CPPUNIT_ASSERT( event1 != 0 );
      const Event *event2 = negativeEvents[1];
      CPPUNIT_ASSERT( event2 != 0 );
      const Event *event3 = negativeEvents[2];
      CPPUNIT_ASSERT( event3 != 0 );
      checkDefaultEvents( event1, event2, event3, event4 );
    }
  private:
    vector<const Event *> negativeEvents;
  };

public:
  TimeWarpSimulationManager *
  allocateTimeWarpSimulationManager( SimulationObject *initObject ){
    return new AOMSimMgr( initObject );
  }

  AggressiveOutputManagerTest();

  void setUp();
  void teardown();

  void testConstructor();
  void testInsert();
  void testRollback();
  void testFossilCollect();
  void testFossilCollectInt();
  void testOfcPurge();

  unsigned int getNumElements( OutputManager *getSizeOf, const ObjectID &objectId );

protected:  

private:
  OutputManager *toTest;

};

#endif
