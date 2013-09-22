#include "AdaptiveOutputManagerTest.h"

DynamicOutputManagerTest::DynamicOutputManagerTest()
 : toTest(0){}

void
DynamicOutputManagerTest::setUp(){
  OutputManagerTest::setUp();
  toTest = new DynamicOutputManagerTestClass( getSimManager() );
}

void
DynamicOutputManagerTest::teardown(){
  delete toTest;
  toTest = 0;
  OutputManagerTest::tearDown();
}

void
DynamicOutputManagerTest::testConstructor(){
  OutputManagerTest::testConstructor( toTest );
}

void
DynamicOutputManagerTest::testInsert(){
  OutputManagerTest::testInsert( toTest );
}

void
DynamicOutputManagerTest::testRollback(){
  CPPUNIT_ASSERT( toTest != 0 );
  ADOMSimMgr *simMgr = dynamic_cast<ADOMSimMgr *>(getSimManager());
  CPPUNIT_ASSERT( simMgr != 0 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );

  // No events are cancelled because none have send time greater than rollback time.
  doDefaultInsert( toTest );
  toTest->rollback( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 4 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 0 );

  // Current cancel mode is aggressive so 4 events should be cancelled.
  toTest->rollback( getSimObj1(), IntVTime(0) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 4 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 0 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 4 );

  // Current cancel mode is lazy so no events should be cancelled.
  simMgr->clearNegativeEvents();
  doDefaultInsert( toTest );
  DynamicOutputManagerTestClass *toCheck = dynamic_cast<DynamicOutputManagerTestClass *>(toTest);
  toCheck->setCancelMode( LAZY, 0 );
  toTest->rollback( getSimObj1(), IntVTime(0) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 0 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 8 );
}

void
DynamicOutputManagerTest::testFossilCollect(){
  OutputManagerTest::testFossilCollect( toTest );
}

void
DynamicOutputManagerTest::testFossilCollectInt(){
  OutputManagerTest::testFossilCollectInt( toTest );
}

void
DynamicOutputManagerTest::testOfcPurge(){
  OutputManagerTest::testOfcPurge( toTest );
}

void
DynamicOutputManagerTest::testDynamicCancel(){
  DynamicOutputManager *toCheck = dynamic_cast<DynamicOutputManager *>( toTest );
  CPPUNIT_ASSERT( toTest != 0 );
  ADOMSimMgr *simMgr = dynamic_cast<ADOMSimMgr *>(getSimManager());
  CPPUNIT_ASSERT( simMgr != 0 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 0);

  // Insert events into the output manager
  for(int a = 0; a < 20; a++){
     toTest->insert(new UnitTestEvent( *new IntVTime( a + 1 ), getSimObj1ID(), *new IntVTime( a ) ));
  }

  // Case 1: currently aggressive, remains aggressive.
  toTest->rollback( getSimObj1(), IntVTime(2) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 18 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 2 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 18 );

  // Messages should never be suppressed when in aggressive mode.
  for(int b = 2; b < 20; b = b + 3){
     CPPUNIT_ASSERT( !toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(b + 1), getSimObj1ID(), *new IntVTime(b))) );
  }
  CPPUNIT_ASSERT( !toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(21), getSimObj1ID(), *new IntVTime(20))) );

  CPPUNIT_ASSERT( toCheck->getCancelMode(0) == AGGRESSIVE );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 18 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 9);
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 0 );

  // Case 2: currently aggressive, changes to lazy.
  // Clear the cancelled event count and insert another 20 events.
  simMgr->clearNegativeEvents();
  for(int c = 21; c < 40; c++){
     toTest->insert(new UnitTestEvent( *new IntVTime( c + 1 ), getSimObj1ID(), *new IntVTime( c ) ));
  }
  toTest->rollback( getSimObj1(), IntVTime(22) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 18 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 10 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 18 );

  for(int d = 25; d < 40; d++){
     toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(d + 1), getSimObj1ID(), *new IntVTime(d)));
  }
  CPPUNIT_ASSERT( !toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(41), getSimObj1ID(), *new IntVTime(40))) );

  CPPUNIT_ASSERT( toCheck->getCancelMode(0) == LAZY );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 18 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 26 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 0 );

  // Case 3: currently lazy, remains lazy.
  // Clear the cancelled event count and insert another 20 events.
  simMgr->clearNegativeEvents();
  for(int e = 41; e < 60; e++){
     toTest->insert(new UnitTestEvent( *new IntVTime( e + 1 ), getSimObj1ID(), *new IntVTime( e ) ));
  }
  toTest->rollback( getSimObj1(), IntVTime(42) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 27 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 18 );

  for(int d = 44; d < 60; d++){
     CPPUNIT_ASSERT( toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(d + 1), getSimObj1ID(), *new IntVTime(d))) );
  }
  CPPUNIT_ASSERT( !toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(61), getSimObj1ID(), *new IntVTime(60))) );

  CPPUNIT_ASSERT( toCheck->getCancelMode(0) == LAZY );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 2 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 44 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 0 );

  // Case 4: currently lazy, changes to aggressive.
  // Clear the output manager and reinsert 20 events.
  simMgr->clearNegativeEvents();
  for(int f = 61; f < 80; f++){
     toTest->insert(new UnitTestEvent( *new IntVTime( f + 1 ), getSimObj1ID(), *new IntVTime( f ) ));
  }
  toTest->rollback( getSimObj1(), IntVTime(62) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 45 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 18 );

  for(int g = 62; g < 80; g = g + 6){
     toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(g + 1), getSimObj1ID(), *new IntVTime(g)));
  }
  CPPUNIT_ASSERT( !toCheck->checkDynamicCancel(new UnitTestEvent( *new IntVTime(81), getSimObj1ID(), *new IntVTime(80))) );

  CPPUNIT_ASSERT( toCheck->getCancelMode(0) == AGGRESSIVE );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 15 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 49 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toTest, *(getSimObj1()->getObjectID()) ) == 0 );
}

void
DynamicOutputManagerTest::testDetermineCancellationMode(){
   vector<const Event*> events;
   for(int i = 0; i < 10; i++){
      events.push_back(new UnitTestEvent( *new IntVTime( i + 1 ), getSimObj1ID(), *new IntVTime( i ) ));
   }

   DynamicOutputManagerTestClass *toCheck = dynamic_cast<DynamicOutputManagerTestClass *>( toTest );
   ADOMSimMgr *simMgr = dynamic_cast<ADOMSimMgr *>(getSimManager());
   CPPUNIT_ASSERT( simMgr != 0 );
 
   // Case 1: currently aggressive, remains aggressive.
   vector<int> res(FILTER_DEPTH,0);
   int index = 0;
   while( index < FILTER_DEPTH * 0.9 ){
      res[index] = 0;
      index++;
   }
   while( index < FILTER_DEPTH ){
      res[index] = 1;
      index++;
   }
   
   toCheck->setComparisonResults( 0, &res );
   toCheck->setLazyCancelEvents( 0, &events );
   toCheck->setCancelMode( AGGRESSIVE, 0 );
   toCheck->determineCancelMode(0);
   CPPUNIT_ASSERT( toCheck->getCancelMode(0) == AGGRESSIVE );

   // Case 2: currently aggressive, changes to lazy.
   index = 0;
   while( index < FILTER_DEPTH * 0.2 ){
      res[index] = 0;
      index++;
   }
   while( index < FILTER_DEPTH ){
      res[index] = 1;
      index++;
   }
   
   toCheck->setComparisonResults( 0, &res );
   toCheck->setLazyCancelEvents( 0, &events );
   toCheck->setCancelMode( AGGRESSIVE, 0 );
   toCheck->determineCancelMode(0);
   CPPUNIT_ASSERT( toCheck->getCancelMode(0) == LAZY );
   CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toCheck, getSimObj1ID() ) == 0 );

   // Case 3: currently lazy, remains lazy
   index = 0;
   while( index < FILTER_DEPTH * 0.2 ){
      res[index] = 0;
      index++;
   }
   while( index < FILTER_DEPTH ){
      res[index] = 1;
      index++;
   }

   for(int i = 0; i < 10; i++){
      events.push_back(new UnitTestEvent( *new IntVTime( i + 1 ), getSimObj1ID(), *new IntVTime( i ) ));
   }
   
   toCheck->setComparisonResults( 0, &res );
   toCheck->setLazyCancelEvents( 0, &events );
   toCheck->setCancelMode( LAZY, 0 );
   toCheck->determineCancelMode(0);
   CPPUNIT_ASSERT( toCheck->getCancelMode(0) == LAZY );
   CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toCheck, getSimObj1ID() ) == 10 );

   // Case 4: currently lazy, changes to aggressive
   index = 0;
   while( index < FILTER_DEPTH * 0.9 ){
      res[index] = 0;
      index++;
   }
   while( index < FILTER_DEPTH ){
      res[index] = 1;
      index++;
   }
   
   toCheck->setComparisonResults( 0, &res );
   toCheck->setLazyCancelEvents( 0, &events );
   toCheck->setCancelMode( LAZY, 0 );
   toCheck->determineCancelMode(0);
   CPPUNIT_ASSERT( toCheck->getCancelMode(0) == AGGRESSIVE );
   CPPUNIT_ASSERT( getNumLazyCancelEvents( 0, toCheck, getSimObj1ID() ) == 0 );
   CPPUNIT_ASSERT( simMgr->negativeCount() == 10 );
}

unsigned int
DynamicOutputManagerTest::getNumElements( OutputManager *getSizeOf,
                                       const ObjectID &objectId ){
  DynamicOutputManagerTestClass *toCheck = dynamic_cast<DynamicOutputManagerTestClass *>( getSizeOf );
  CPPUNIT_ASSERT( toCheck != 0 );
  OutputEvents &oe = toCheck->getOutputEventsFor( objectId );
  vector<const Event *> *events = oe.getEventsSentAtOrAfter( IntVTime::getIntVTimeZero() );
  unsigned int retval = events->size();
  delete events;
  return retval;
}

unsigned int
DynamicOutputManagerTest::getNumLazyCancelEvents( int id, OutputManager *getSizeOf,
                                                   const ObjectID &objectId ){
  DynamicOutputManagerTestClass *toCheck = dynamic_cast<DynamicOutputManagerTestClass *>( getSizeOf );
  CPPUNIT_ASSERT( toCheck != 0 );
  vector<const Event*> lce = toCheck->getLazyCancelEvents(id);
  unsigned int retval = lce.size();
  return retval;
}

