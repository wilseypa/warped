#include "GVTUpdateMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>
#include "warped/IntVTime.h"

const VTime &
GVTUpdateMessageTest::getDefaultGVT(){
  static IntVTime retval( 13 );
  return retval;
}

void 
GVTUpdateMessageTest::setUp(){
  testMessage = new GVTUpdateMessage( KernelMessageTest::getDefaultSender(), 
				      KernelMessageTest::getDefaultReceiver(),
				      getDefaultGVT() );
}

void 
GVTUpdateMessageTest::tearDown(){
  delete testMessage;
  testMessage = 0;
}

void 
GVTUpdateMessageTest::testConstructor(){
  KernelMessageTest::testConstructor( testMessage );
  CPPUNIT_ASSERT( testMessage->getDataType() == GVTUpdateMessage::getGVTUpdateMessageType() );
  CPPUNIT_ASSERT( testMessage->getNewGVT() == getDefaultGVT() );
}

void 
GVTUpdateMessageTest::testSerialization(){
  KernelMessageTest::testSerialization( testMessage, 
					GVTUpdateMessage::getGVTUpdateMessageType() );
  
  SerializedInstance *serialized = static_cast<Serializable *>(testMessage)->serialize();
  GVTUpdateMessage *deserialized = dynamic_cast<GVTUpdateMessage *>( serialized->deserialize() );
  CPPUNIT_ASSERT( deserialized->getDataType() == GVTUpdateMessage::getGVTUpdateMessageType() );
  CPPUNIT_ASSERT( deserialized->getNewGVT() == getDefaultGVT() );
  delete deserialized;
}
