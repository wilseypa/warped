#ifndef EVENT_MESSAGE_TEST_H
#define EVENT_MESSAGE_TEST_H

#include "EventMessage.h"
#include "EventMessageBaseTest.h"

class EventMessageTest : public EventMessageBaseTest {
  CPPUNIT_TEST_SUITE( EventMessageTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  EventMessage *testMessage;

  static Event *getDefaultEvent();

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
