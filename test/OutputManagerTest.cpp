#include "OutputManagerTest.h"
#include "OutputManager.h"
#include "IntVTime.h"

OutputManagerTest::OutputManagerTest(){}

void
OutputManagerTest::testConstructor( OutputManager *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
}

void
OutputManagerTest::doDefaultInsert( OutputManager *insertInto ){
  CPPUNIT_ASSERT( insertInto != 0 );
  vector<Event *> &defaultEvents = getDefaultEvents();
  for( vector<Event *>::iterator i = defaultEvents.begin();
       i < defaultEvents.end();
       i++ ){
    insertInto->insert( *i );
  }
}

void
OutputManagerTest::testInsert( OutputManager *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );
  CPPUNIT_ASSERT( getNumElements( toTest, getDefaultSender() ) == getDefaultEvents().size() );
}

void
OutputManagerTest::testFossilCollect( OutputManager *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );
  CPPUNIT_ASSERT( getNumElements( toTest, getDefaultSender() ) == getDefaultEvents().size() );
  toTest->fossilCollect( getSimObj1() , IntVTime( 1 ) );
  CPPUNIT_ASSERT( getNumElements( toTest, getDefaultSender() ) == 0 );
}

void
OutputManagerTest::testFossilCollectInt( OutputManager *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );
  CPPUNIT_ASSERT( getNumElements( toTest, getDefaultSender() ) == getDefaultEvents().size() );
  toTest->fossilCollect( getSimObj1() , 1 );
  CPPUNIT_ASSERT( getNumElements( toTest, getDefaultSender() ) == 0 );
}

void
OutputManagerTest::testOfcPurge( OutputManager *toTest ){
  CPPUNIT_ASSERT( toTest != 0 );
  doDefaultInsert( toTest );
  CPPUNIT_ASSERT( getNumElements( toTest, getDefaultSender() ) == getDefaultEvents().size() );
  toTest->ofcPurge();
  CPPUNIT_ASSERT( getNumElements( toTest, getDefaultSender() ) == 0 );
}
