#include "CostAdaptiveStateManagerTest.h"
#include "warped/IntVTime.h"

CostAdaptiveStateManagerTest::CostAdaptiveStateManagerTest(){}

void
CostAdaptiveStateManagerTest::setUp(){
   WarpedUnitTest::setUp();
   toTest = new CASMTestClass( getSimManager() );
}

void
CostAdaptiveStateManagerTest::tearDown(){
   delete toTest;
   toTest = 0;
   WarpedUnitTest::tearDown();
}

void
CostAdaptiveStateManagerTest::testConstructor(){
   CPPUNIT_ASSERT( toTest != 0 );
}

void
CostAdaptiveStateManagerTest::testSaveState(){
   // The actual saving of the state is tested in AdaptiveStateManagerBaseTest.
   // Just check that the forward execution length is being incremented and reset
   // when the period is recalculated.
   CASMTestClass *toCheck = dynamic_cast<CASMTestClass *>(toTest);
   toCheck->setAdaptiveParameters(0, 30);  

   for( int a = 1; a <= 20; a++)
   {
      toTest->saveState( IntVTime(a), getSimObj1() );
   }

   CPPUNIT_ASSERT( toCheck->getForwardExecutionLength()[0] == 20 );

   for( int b = 21; b <= 40; b++)
   {
      toTest->saveState( IntVTime(b), getSimObj1() );
   }

   CPPUNIT_ASSERT( toCheck->getForwardExecutionLength()[0] == 10 );
}

void
CostAdaptiveStateManagerTest::testCalculatePeriod(){

   // Case 1: The first time the period is calculated, it is increased by
   //         1 because the oldCostIndex is initially 0.
   //         filterCostIndex becomes 4.80
   CASMTestClass *toCheck = dynamic_cast<CASMTestClass *>(toTest);
   toCheck->StateSaveTimeWeighted[0].update(10);
   toCheck->CoastForwardTimeWeighted[0].update(10);
   toCheck->calculatePeriod( getSimObj1() );
   CPPUNIT_ASSERT( toCheck->getObjectStatePeriod()[0] == 1 );

   // Case 2: Period remains the same.
   //         filterCostIndex becomes 4.32
   toCheck->StateSaveTimeWeighted[0].update(5);
   toCheck->CoastForwardTimeWeighted[0].update(5);
   toCheck->calculatePeriod(getSimObj1());
   CPPUNIT_ASSERT( toCheck->getObjectStatePeriod()[0] == 1 );

   // Case 3: Period increases by 1.
   //         filteredCostIndex becomes 1.78
   toCheck->StateSaveTimeWeighted[0].update(0.1);
   toCheck->CoastForwardTimeWeighted[0].update(0.1);
   toCheck->calculatePeriod(getSimObj1());
   CPPUNIT_ASSERT( toCheck->getObjectStatePeriod()[0] == 2 );

   // Case 4: Period decreases by 1.
   //         filteredCostIndex becomes 5.51
   toCheck->StateSaveTimeWeighted[0].update(10);
   toCheck->CoastForwardTimeWeighted[0].update(10);
   toCheck->calculatePeriod(getSimObj1());
   CPPUNIT_ASSERT( toCheck->getObjectStatePeriod()[0] == 1 );

   // Case 5: Period is at the maximum. An attempt to increase it should keep it at maximum
   toCheck->setPeriod( getSimObj1(), maxDefaultInterval);
   toCheck->StateSaveTimeWeighted[0].update(0.1);
   toCheck->CoastForwardTimeWeighted[0].update(0.1);
   toCheck->calculatePeriod(getSimObj1());
   CPPUNIT_ASSERT( toCheck->getObjectStatePeriod()[0] == maxDefaultInterval - 1 );
}
