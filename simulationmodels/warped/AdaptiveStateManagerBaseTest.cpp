#include "AdaptiveStateManagerBaseTest.h"
#include "IntVTime.h"

AdaptiveStateManagerBaseTest::AdaptiveStateManagerBaseTest(){}

void
AdaptiveStateManagerBaseTest::setUp(){
   WarpedUnitTest::setUp();
   toTest = new ASMBTestClass( getSimManager() );
}

void
AdaptiveStateManagerBaseTest::tearDown(){
   delete toTest;
   toTest = 0;
   WarpedUnitTest::tearDown();
}

void
AdaptiveStateManagerBaseTest::testConstructor(){
  CPPUNIT_ASSERT( toTest != 0 );
}

void
AdaptiveStateManagerBaseTest::testSaveState(){
   // There should be saved states every 4th state.
   ASMBTestClass *toCheck = dynamic_cast<ASMBTestClass *>(toTest);
   toCheck->setPeriod( getSimObj1(), 3 );
   defaultSaveState();

   // Go through the state queue. The key is the object. There should then be a queue of
   // pairs SetObject. The pair is (time, state). Examine the time and state. In this case
   // the state should be 10*time.   

   multiset< SetObject<State> > *stateQueue = toCheck->getStateQueue();
   
   multiset< SetObject<State> >::iterator iter_begin =
   stateQueue[ getSimObj1()->getObjectID()->getSimulationObjectID() ].begin();

   multiset< SetObject<State> >::iterator iter_end =
   stateQueue[ getSimObj1()->getObjectID()->getSimulationObjectID() ].end();

   int check = 1;
   while( iter_begin != iter_end ){
      const UnitTestState *checkState = dynamic_cast<const UnitTestState *>((*iter_begin).getElement());
      CPPUNIT_ASSERT( checkState->intMember == check * 10 );
      IntVTime time = atoi( (*iter_begin).getMainTime().toString().c_str() );
      CPPUNIT_ASSERT( time == check );
      check += 4;
      iter_begin++;
   }
}

void
AdaptiveStateManagerBaseTest::testRestoreState(){
   // Create the same state queue as above.
   ASMBTestClass *toCheck = dynamic_cast<ASMBTestClass *>(toTest);
   toCheck->setPeriod( getSimObj1(), 3 );
   defaultSaveState();   
   
   // Call restoreState.
   toTest->restoreState( IntVTime(7), getSimObj1() );

   // Check the current state of the object
   UnitTestState *testState = dynamic_cast<UnitTestState *>( getSimObj1()->getState() );
   CPPUNIT_ASSERT( testState->intMember == 50 );

   // May also want to check the state queue and see if its last element matches the current state.
   // This includes checking the size.
   multiset< SetObject<State> > *stateQueue = toCheck->getStateQueue();

   multiset< SetObject<State> >::iterator iter_end =
   stateQueue[ getSimObj1()->getObjectID()->getSimulationObjectID() ].end();

   iter_end--;

   const UnitTestState *checkState = dynamic_cast<const UnitTestState *>((*iter_end).getElement());
   IntVTime time = atoi( (*iter_end).getMainTime().toString().c_str() );
   CPPUNIT_ASSERT( time == 5 );
   CPPUNIT_ASSERT( checkState->intMember == 50 );
}

void
AdaptiveStateManagerBaseTest::testFossilCollect(){
   // Create the same state queue as above.
   ASMBTestClass *toCheck = dynamic_cast<ASMBTestClass *>(toTest);
   toCheck->setPeriod( getSimObj1(), 3 );
   defaultSaveState();

   // Call fossilCollect.
   CPPUNIT_ASSERT( toTest->fossilCollect( getSimObj1(), IntVTime(7) ) == IntVTime(5) );

   // Check the size of the state queue. May also want to check the entire queue.
   multiset< SetObject<State> > *stateQueue = toCheck->getStateQueue();

   CPPUNIT_ASSERT( stateQueue[ getSimObj1()->getObjectID()->getSimulationObjectID() ].size() == 2 );

   multiset< SetObject<State> >::iterator iter_begin =
   stateQueue[ getSimObj1()->getObjectID()->getSimulationObjectID() ].begin();

   const UnitTestState *checkState = dynamic_cast<const UnitTestState *>((*iter_begin).getElement());
   IntVTime time = atoi( (*iter_begin).getMainTime().toString().c_str() );
   CPPUNIT_ASSERT( time == 5 );
   CPPUNIT_ASSERT( checkState->intMember == 50 );

   iter_begin++;

   checkState = dynamic_cast<const UnitTestState *>((*iter_begin).getElement());
   time = atoi( (*iter_begin).getMainTime().toString().c_str() );
   CPPUNIT_ASSERT( time == 9 );
   CPPUNIT_ASSERT( checkState->intMember == 90 );
}

void
AdaptiveStateManagerBaseTest::defaultSaveState(){
   for( int a = 1; a <= 10; a++) {
     UnitTestState *testState = dynamic_cast<UnitTestState *>( getSimObj1()->getState() );
     testState->intMember = a * 10;
     toTest->saveState( IntVTime(a), getSimObj1() );
   }
}
