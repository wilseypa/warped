#ifndef INITIALIZATION_MESSAGE_TEST_H
#define INITIALIZATION_MESSAGE_TEST_H

#include "InitializationMessage.h"
#include "KernelMessageTest.h"

class InitializationMessageTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( InitializationMessageTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  InitializationMessage *testMessage;

  static const vector<string> getDefaultObjectNames();
  static unsigned int getDefaultNumSimulationManagers();

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
