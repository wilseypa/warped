#ifndef CIRCULATE_INITIALIZATION_MESSAGE_TEST_H
#define CIRCULATE_INITIALIZATION_MESSAGE_TEST_H

#include "warped/CirculateInitializationMessage.h"
#include "KernelMessageTest.h"

class CirculateInitializationMessageTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( CirculateInitializationMessageTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  CirculateInitializationMessage *testMessage;

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
