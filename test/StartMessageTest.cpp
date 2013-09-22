#include "StartMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>

void 
StartMessageTest::setUp(){
  testMessage = new StartMessage( KernelMessageTest::getDefaultSender(), 
				  KernelMessageTest::getDefaultReceiver() );
}

void 
StartMessageTest::tearDown(){
  delete testMessage;
  testMessage = 0;
}

void 
StartMessageTest::testConstructor(){
  KernelMessageTest::testConstructor( testMessage );
  CPPUNIT_ASSERT( testMessage->getDataType() == StartMessage::getStartMessageType() );
}

void 
StartMessageTest::testSerialization(){
  KernelMessageTest::testSerialization( testMessage, 
					StartMessage::getStartMessageType() );
}
