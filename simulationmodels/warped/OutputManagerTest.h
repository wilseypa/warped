#ifndef OUTPUT_MANAGER_TEST_H
#define OUTPUT_MANAGER_TEST_H

#include "WarpedUnitTest.h"

class OutputManager;

class OutputManagerTest : public WarpedUnitTest {
public:
  OutputManagerTest();

  void setUp(){ WarpedUnitTest::setUp(); }
  void tearDown(){ WarpedUnitTest::tearDown(); }

  virtual void testConstructor() = 0;
  virtual void testInsert() = 0;
  virtual unsigned int getNumElements( OutputManager *getSizeOf, const ObjectID &objectId ) = 0;

protected:  
  void testConstructor( OutputManager *toTest );
  void testInsert( OutputManager *toTest );
  void testRollback( OutputManager *toTest );
  void testFossilCollect( OutputManager *toTest );
  void testFossilCollectInt( OutputManager *toTest );
  void testOfcPurge( OutputManager *toTest );

  void doDefaultInsert( OutputManager *insertInto );

};

#endif
