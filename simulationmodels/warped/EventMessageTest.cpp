#include "EventMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>
#include "DeserializerManager.h"

Event *
EventMessageTest::getDefaultEvent(){
  return new EventMessageBaseTest::EventMessageBaseTestEvent;
}

void 
EventMessageTest::setUp(){
  static bool doneIt = false;
  if( doneIt == false ){
    DeserializerManager::instance()->registerDeserializer( EventMessageBaseTestEvent::getEventMessageBaseTestEventDataType(),
							   &EventMessageBaseTestEvent::deserialize );

  }

  testMessage = new EventMessage( KernelMessageTest::getDefaultSender(), 
				  KernelMessageTest::getDefaultReceiver(),
				  getDefaultEvent(),
				  getDefaultGVTInfo() );
}

void 
EventMessageTest::tearDown(){
  delete testMessage;
  testMessage = 0;
}

void 
EventMessageTest::testConstructor(){
  EventMessageBaseTest::testConstructor( testMessage );
  CPPUNIT_ASSERT( testMessage->getDataType() == EventMessage::getEventMessageType() );
}

void 
EventMessageTest::testSerialization(){
  EventMessageBaseTest::testSerialization( testMessage, 
					   EventMessage::getEventMessageType() );
}
