#ifndef MATTERN_GVT_MESSAGE_TEST_H
#define MATTERN_GVT_MESSAGE_TEST_H

#include "warped/MatternGVTMessage.h"
#include "KernelMessageTest.h"

class MatternGVTMessageTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( MatternGVTMessageTest);
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  MatternGVTMessage *testMessage;

  static const VTime &getDefaultLastScheduledEventTime();
  static const VTime &getDefaultMinimumTimeStamp();
  static unsigned int getDefaultNumMessagesInTransit();

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
