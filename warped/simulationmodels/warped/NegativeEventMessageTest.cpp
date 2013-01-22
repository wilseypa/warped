#include "NegativeEventMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>
#include "warped/DeserializerManager.h"
#include "warped/EventId.h"
#include "warped/NegativeEvent.h"

void 
NegativeEventMessageTest::setUp(){
  KernelMessageTest::setUp();

  DeserializerManager::instance()->registerDeserializer( UnitTestEvent::getUnitTestEventType(), 
                                                         &UnitTestEvent::deserialize );

  vector<const NegativeEvent*> events;
  Event *eve3 = getEvent3();
  NegativeEvent *neg = new NegativeEvent(eve3->getSendTime(), eve3->getReceiveTime(),
                                         eve3->getSender(), eve3->getReceiver(),
                                         eve3->getEventId());
  events.push_back( neg );
  string gvtInfo = "Some Info";
  testMessage = new NegativeEventMessage( KernelMessageTest::getDefaultSender(), 
					  KernelMessageTest::getDefaultReceiver(),
					  events,
					  gvtInfo );
}

void 
NegativeEventMessageTest::tearDown(){
  delete testMessage;
  testMessage = 0;
}

void 
NegativeEventMessageTest::testConstructor(){
  KernelMessageTest::testConstructor( testMessage );
  CPPUNIT_ASSERT( testMessage->getDataType() == NegativeEventMessage::getNegativeEventMessageType() );
}

void 
NegativeEventMessageTest::testSerialization(){
  KernelMessageTest::testSerialization( testMessage, 
					NegativeEventMessage::getNegativeEventMessageType() );
}
