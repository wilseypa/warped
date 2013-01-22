#ifndef GVT_UPDATE_MESSAGE_TEST_H
#define GVT_UPDATE_MESSAGE_TEST_H

#include "warped/GVTUpdateMessage.h"
#include "KernelMessageTest.h"

class GVTUpdateMessageTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( GVTUpdateMessageTest);
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  GVTUpdateMessage *testMessage;

  const VTime &getDefaultGVT();

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
