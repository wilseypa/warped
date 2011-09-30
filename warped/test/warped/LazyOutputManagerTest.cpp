// Copyright (c) 2003 Clifton Labs, Inc.  All rights reserved.

// CLIFTON LABS MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
// SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  CLIFTON LABS SHALL NOT BE
// LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, RESULT
// OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Dale E. Martin              dmartin@cliftonlabs.com
//          Randall King                kingr8@cliftonlabs.com

#include "LazyOutputManagerTest.h"

LazyOutputManagerTest::LazyOutputManagerTest()
 : toTest(0){}

void 
LazyOutputManagerTest::setUp(){
  OutputManagerTest::setUp();
  toTest = new LazyOutputManagerTestClass( getSimManager() );
}

void
LazyOutputManagerTest::teardown(){
  delete toTest;
  toTest = 0;
  OutputManagerTest::tearDown();
}

void
LazyOutputManagerTest::testConstructor(){
  OutputManagerTest::testConstructor( toTest );
}

void
LazyOutputManagerTest::testInsert(){
  OutputManagerTest::testInsert( toTest );
}

void
LazyOutputManagerTest::testRollback(){
  CPPUNIT_ASSERT( toTest != 0 );
  LOMSimMgr *simMgr = dynamic_cast<LOMSimMgr *>(getSimManager());
  CPPUNIT_ASSERT( simMgr != 0 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );

  doDefaultInsert( toTest );
  toTest->rollback( getSimObj1(), IntVTime(1) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 4 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( toTest, *(getSimObj1()->getObjectID()) ) == 0 );
  
  toTest->rollback( getSimObj1(), IntVTime(0) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 0 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( toTest, *(getSimObj1()->getObjectID()) ) == 4 );
}

void
LazyOutputManagerTest::testFossilCollect(){
  OutputManagerTest::testFossilCollect( toTest );
}

void
LazyOutputManagerTest::testFossilCollectInt(){
  OutputManagerTest::testFossilCollectInt( toTest );
}

void
LazyOutputManagerTest::testOfcPurge(){
  OutputManagerTest::testOfcPurge( toTest );
}

void
LazyOutputManagerTest::testLazyCancel(){
  LazyOutputManager *toCheck = dynamic_cast<LazyOutputManager *>( toTest );
  CPPUNIT_ASSERT( toTest != 0 );
  LOMSimMgr *simMgr = dynamic_cast<LOMSimMgr *>(getSimManager());
  CPPUNIT_ASSERT( simMgr != 0 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 0);

  //Insert events into the output manager.
  toTest->insert(new UnitTestEvent( *new IntVTime( 1 ), getSimObj1ID(), *new IntVTime( 0 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 1 ), getSimObj1ID(), *new IntVTime( 0 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 1 ), getSimObj1ID(), *new IntVTime( 0 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 2 ), getSimObj1ID(), *new IntVTime( 1 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 3 ), getSimObj1ID(), *new IntVTime( 2 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 3 ), getSimObj1ID(), *new IntVTime( 2 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 4 ), getSimObj1ID(), *new IntVTime( 3 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 4 ), getSimObj1ID(), *new IntVTime( 3 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 4 ), getSimObj1ID(), *new IntVTime( 3 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 5 ), getSimObj1ID(), *new IntVTime( 4 ) ));
  toTest->insert(new UnitTestEvent( *new IntVTime( 6 ), getSimObj1ID(), *new IntVTime( 5 ) ));

  //Rollback the output manager and check the lazy queue.
  toTest->rollback( getSimObj1(), IntVTime(2) );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 4 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( toTest, *(getSimObj1()->getObjectID()) ) == 7 );

  //Test the case where all events are regenerated.
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 3 ), getSimObj1ID(), *new IntVTime( 2 ) )));
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 3 ), getSimObj1ID(), *new IntVTime( 2 ) )));
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 4 ), getSimObj1ID(), *new IntVTime( 3 ) )));
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 4 ), getSimObj1ID(), *new IntVTime( 3 ) )));
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 4 ), getSimObj1ID(), *new IntVTime( 3 ) )));
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 5 ), getSimObj1ID(), *new IntVTime( 4 ) )));
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 6 ), getSimObj1ID(), *new IntVTime( 5 ) )));
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 11 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( toTest, *(getSimObj1()->getObjectID()) ) == 0 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 0 );
  toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 7 ), getSimObj1ID(), *new IntVTime( 6 ) ));

  //Test the case where some events are not regenerated.
  toTest->rollback( getSimObj1(), IntVTime(2) );
  CPPUNIT_ASSERT( toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 4 ), getSimObj1ID(), *new IntVTime( 3 ) )));
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 5 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( toTest, *(getSimObj1()->getObjectID()) ) == 5 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 2 );

  CPPUNIT_ASSERT( !toCheck->lazyCancel(new UnitTestEvent( *new IntVTime( 8 ), getSimObj1ID(), *new IntVTime( 7 ) )));
  CPPUNIT_ASSERT( getNumElements( toTest, *(getSimObj1()->getObjectID()) ) == 6 );
  CPPUNIT_ASSERT( getNumLazyCancelEvents( toTest, *(getSimObj1()->getObjectID()) ) == 0 );
  CPPUNIT_ASSERT( simMgr->negativeCount() == 7 );
}

unsigned int
LazyOutputManagerTest::getNumElements( OutputManager *getSizeOf,
				       const ObjectID &objectId ){
  LazyOutputManagerTestClass *toCheck = dynamic_cast<LazyOutputManagerTestClass *>( getSizeOf );
  CPPUNIT_ASSERT( toCheck != 0 );
  OutputEvents &oe = toCheck->getOutputEventsFor( objectId );
  vector<const Event *> *events = oe.getEventsSentAtOrAfter( IntVTime::getIntVTimeZero() );
  unsigned int retval = events->size();
  delete events;
  return retval;
}

unsigned int 
LazyOutputManagerTest::getNumLazyCancelEvents( OutputManager *getSizeOf,
                                               const ObjectID &objectId ){
  LazyOutputManagerTestClass *toCheck = dynamic_cast<LazyOutputManagerTestClass *>( getSizeOf );
  CPPUNIT_ASSERT( toCheck != 0 );
  vector<const Event*> lce = toCheck->getLazyCancelEvents();
  unsigned int retval = lce.size();
  return retval;
}
