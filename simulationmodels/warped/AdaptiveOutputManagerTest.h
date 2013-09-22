#ifndef ADAPTIVE_OUTPUT_MANAGER_TEST_H
#define ADAPTIVE_OUTPUT_MANAGER_TEST_H

#include "OutputManagerTest.h"
#include "warped/AdaptiveOutputManager.h"

class DynamicOutputManager;

class DynamicOutputManagerTest : public OutputManagerTest {
  CPPUNIT_TEST_SUITE( DynamicOutputManagerTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testInsert );
  CPPUNIT_TEST( testRollback );
  CPPUNIT_TEST( testFossilCollect );
  CPPUNIT_TEST( testFossilCollectInt );
  CPPUNIT_TEST( testOfcPurge );
  CPPUNIT_TEST( testDynamicCancel );
  CPPUNIT_TEST( testDetermineCancellationMode );
  CPPUNIT_TEST_SUITE_END();

  class ADOMSimMgr : public UnitTestSimulationManager {
  public:
    ADOMSimMgr( SimulationObject *initObj ) :
      UnitTestSimulationManager( initObj ){}
    ~ADOMSimMgr(){}

    void cancelEvents( const vector<const Event *> &eventsToCancel ){
      negativeEvents.insert( negativeEvents.begin(),
                             eventsToCancel.begin(),
                             eventsToCancel.end() );

    }

    void clearNegativeEvents() { negativeEvents.clear(); }

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

  class DynamicOutputManagerTestClass : public DynamicOutputManager {
  public:
    DynamicOutputManagerTestClass( TimeWarpSimulationManager *simMgr )
      : DynamicOutputManager( simMgr ) {}
    ~DynamicOutputManagerTestClass() {}

    vector<const Event*> getLazyCancelEvents(int id) { return *(lazyQueues[id]); }
    void setLazyCancelEvents( int id, vector<const Event*> *lazyEvents ) { lazyQueues[id] = lazyEvents; }

    void setComparisonResults(int id, vector<int> *results) { comparisonResults[id] = results; }
    void determineCancelMode(int id) { determineCancellationMode(id); }

    OutputEvents &getOutputEventsFor( const ObjectID &objectId ) {
      return OutputManagerImplementationBase::getOutputEventsFor(objectId);
    }

  };

public:
  TimeWarpSimulationManager *
  allocateTimeWarpSimulationManager( SimulationObject *initObject ){
    return new ADOMSimMgr( initObject );
  }

  DynamicOutputManagerTest();

  void setUp();
  void teardown();

  void testConstructor();
  void testInsert();
  void testRollback();
  void testFossilCollect();
  void testFossilCollectInt();
  void testOfcPurge();
  void testDynamicCancel();
  void testDetermineCancellationMode();

  unsigned int getNumElements( OutputManager *getSizeOf, const ObjectID &objectId );
  unsigned int getNumLazyCancelEvents( int id, OutputManager *getSizeOf, const ObjectID &objectId );

protected:

private:
  OutputManager *toTest;

};

#endif

