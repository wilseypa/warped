#ifndef START_MESSAGE_TEST_H
#define START_MESSAGE_TEST_H

#include "warped/StartMessage.h"
#include "KernelMessageTest.h"

class StartMessageTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( StartMessageTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  StartMessage *testMessage;

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
