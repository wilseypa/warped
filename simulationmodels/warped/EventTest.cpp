#include "EventTest.h"
#include <cppunit/extensions/HelperMacros.h>
#include "SerializedInstance.h"
#include "DeserializerManager.h"

void 
EventTest::setUp(){
  static bool doneIt = false;
  if( doneIt == false ){
    DeserializerManager::instance()->registerDeserializer( EventTestTestEvent::getEventTestTestEventType(),
							   &EventTestTestEvent::deserialize );

  }
  
}

void 
EventTest::tearDown(){
}

void 
EventTest::testSerialization( const Event *toTest, const string &dataType ){
  SerializableTest::testSerialization( toTest, dataType );

  const VTime &sendTime = toTest->getSendTime();
  const VTime &recvTime = toTest->getReceiveTime();
  const ObjectID &receiver = toTest->getReceiver();
  const ObjectID &sender = toTest->getSender();

  SerializedInstance *serialized = static_cast<const Serializable *>(toTest)->serialize();
  Event *deserialized = dynamic_cast<Event *>(serialized->deserialize());
  CPPUNIT_ASSERT( sendTime == deserialized->getSendTime() );
  CPPUNIT_ASSERT( recvTime == deserialized->getReceiveTime() );
  CPPUNIT_ASSERT( sender == deserialized->getSender() );
  CPPUNIT_ASSERT( receiver == deserialized->getReceiver() );
}

void 
EventTest::testSerialization(){
  Event *toTest = new EventTestTestEvent();
  testSerialization( toTest, EventTestTestEvent::getEventTestTestEventType() );
  delete toTest;
}

void 
EventTest::testEventIds(){
  ASSERT( getEvent1()->getEventId() != getEvent2()->getEventId() );
  ASSERT( getEvent1()->getEventId() != getEvent3()->getEventId() );
  ASSERT( getEvent1()->getEventId() != getEvent4()->getEventId() );
  ASSERT( getEvent2()->getEventId() != getEvent3()->getEventId() );
  ASSERT( getEvent2()->getEventId() != getEvent4()->getEventId() );
  ASSERT( getEvent3()->getEventId() != getEvent4()->getEventId() );
}
