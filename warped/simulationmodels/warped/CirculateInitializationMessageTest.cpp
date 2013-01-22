#include "CirculateInitializationMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>

void 
CirculateInitializationMessageTest::setUp(){
  testMessage = new CirculateInitializationMessage( KernelMessageTest::getDefaultSender(), 
						    KernelMessageTest::getDefaultReceiver() );
}

void 
CirculateInitializationMessageTest::tearDown(){
  delete testMessage;
  testMessage = 0;
}

void 
CirculateInitializationMessageTest::testConstructor(){
  KernelMessageTest::testConstructor( testMessage );
  CPPUNIT_ASSERT( testMessage->getDataType() == CirculateInitializationMessage::getCirculateInitializationMessageType() );
}

void 
CirculateInitializationMessageTest::testSerialization(){
  KernelMessageTest::testSerialization( testMessage, 
					CirculateInitializationMessage::getCirculateInitializationMessageType() );
}
