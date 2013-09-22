#include "AggressiveOutputManagerTest.h"
#include "warped/AggressiveOutputManager.h"

AggressiveOutputManagerTest::AggressiveOutputManagerTest()
 : toTest(0){}

void 
AggressiveOutputManagerTest::setUp(){
  OutputManagerTest::setUp();
  toTest = new AggressiveOutputManager( getSimManager() );
}

void
AggressiveOutputManagerTest::teardown(){
  delete toTest;
  toTest = 0;
  OutputManagerTest::tearDown();
}

void
AggressiveOutputManagerTest::testConstructor(){
  OutputManagerTest::testConstructor( toTest );
}

void
AggressiveOutputManagerTest::testInsert(){
  OutputManagerTest::testInsert( toTest );
}

void
AggressiveOutputManagerTest::testRollback(){
  CPPUNIT_ASSERT( toTest != 0 );
  AOMSimMgr *simMgr = dynamic_cast<AOMSimMgr *>(getSimManager());
  CPPUNIT_ASSERT( simMgr != 0 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );

  doDefaultInsert( toTest );
  toTest->rollback( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  
  toTest->rollback( getSimObj1(), IntVTime(0) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 4 );
  simMgr->checkNegativeEvents();
}

void
AggressiveOutputManagerTest::testFossilCollect(){
  OutputManagerTest::testFossilCollect( toTest );
}

void
AggressiveOutputManagerTest::testFossilCollectInt(){
  OutputManagerTest::testFossilCollectInt( toTest );
}

void
AggressiveOutputManagerTest::testOfcPurge(){
  OutputManagerTest::testOfcPurge( toTest );
}

unsigned int
AggressiveOutputManagerTest::getNumElements( OutputManager *getSizeOf,
					     const ObjectID &objectId ){
  AggressiveOutputManager *toCheck = dynamic_cast<AggressiveOutputManager *>( getSizeOf );
  CPPUNIT_ASSERT( toCheck != 0 );
  OutputEvents &oe = toCheck->getOutputEventsFor( objectId );
  vector<const Event *> *events = oe.getEventsSentAtOrAfter( IntVTime::getIntVTimeZero() );
  unsigned int retval = events->size();
  delete events;
  return retval;
}
