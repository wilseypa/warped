#include "MatternGVTMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>
#include "IntVTime.h"

const VTime &
MatternGVTMessageTest::getDefaultLastScheduledEventTime(){
  static IntVTime retval( 10 );
  return retval;
}

const VTime &
MatternGVTMessageTest::getDefaultMinimumTimeStamp(){
  static IntVTime retval( 100 );
  return retval;
}

unsigned int
MatternGVTMessageTest::getDefaultNumMessagesInTransit(){
  return 1172;
}

void 
MatternGVTMessageTest::setUp(){
  testMessage = new MatternGVTMessage( KernelMessageTest::getDefaultSender(), 
				       KernelMessageTest::getDefaultReceiver(),
				       getDefaultLastScheduledEventTime(),
				       getDefaultMinimumTimeStamp(),
				       getDefaultNumMessagesInTransit() );
}

void 
MatternGVTMessageTest::tearDown(){
  delete testMessage;
  testMessage = 0;
}

void 
MatternGVTMessageTest::testConstructor(){
  KernelMessageTest::testConstructor( testMessage );
  CPPUNIT_ASSERT( testMessage->getDataType() == MatternGVTMessage::getMatternGVTMessageType() );
  CPPUNIT_ASSERT( testMessage->getLastScheduledEventTime() == getDefaultLastScheduledEventTime() );
  CPPUNIT_ASSERT( testMessage->getMinimumTimeStamp() == getDefaultMinimumTimeStamp() );
}

void 
MatternGVTMessageTest::testSerialization(){
  KernelMessageTest::testSerialization( testMessage, 
					MatternGVTMessage::getMatternGVTMessageType() );
  
  SerializedInstance *serialized = static_cast<Serializable *>(testMessage)->serialize();
  MatternGVTMessage *deserialized = dynamic_cast<MatternGVTMessage *>( serialized->deserialize() );
  CPPUNIT_ASSERT( deserialized->getDataType() == MatternGVTMessage::getMatternGVTMessageType() );
  CPPUNIT_ASSERT( deserialized->getLastScheduledEventTime() == getDefaultLastScheduledEventTime() );
  CPPUNIT_ASSERT( deserialized->getMinimumTimeStamp() == getDefaultMinimumTimeStamp() );
  delete deserialized;
}
