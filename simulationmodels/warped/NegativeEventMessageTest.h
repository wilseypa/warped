#ifndef NEGATIVE_EVENT_MESSAGE_TEST_H
#define NEGATIVE_EVENT_MESSAGE_TEST_H

#include "warped/NegativeEventMessage.h"
#include "KernelMessageTest.h"

class NegativeEventMessageTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( NegativeEventMessageTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  NegativeEventMessage *testMessage;

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
