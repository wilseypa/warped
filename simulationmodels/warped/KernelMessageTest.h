#ifndef KERNEL_MESSAGE_TEST_H
#define KERNEL_MESSAGE_TEST_H


#include "SerializableTest.h"

class KernelMessage;

class KernelMessageTest : public SerializableTest {
  CPPUNIT_TEST_SUITE( KernelMessageTest);
  CPPUNIT_TEST_SUITE_END();

private:

public:
  // Test interface
  void setUp();
  void tearDown();

  static void testConstructor( const KernelMessage *toTest );
  static void testSerialization( const KernelMessage *toTest,
				 const string &dataType );
  
  static unsigned int getDefaultSender(){ return 12; }
  static unsigned int getDefaultReceiver(){ return 72; }

};

#endif
