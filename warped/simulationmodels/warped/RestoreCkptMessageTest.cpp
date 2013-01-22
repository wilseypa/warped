#include "RestoreCkptMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>

void 
RestoreCkptMessageTest::setUp(){
  testToken = new RestoreCkptMessage( getDefaultSender(), 
                                      getDefaultReceiver(),
                                      getDefaultCkptTime(),
                                      getDefaultRestoreState(),
                                      getDefaultCkptConsensus() );
}

void 
RestoreCkptMessageTest::tearDown(){
  delete testToken;
  testToken = 0;
}

void
RestoreCkptMessageTest::checkDefaults( const RestoreCkptMessage *testToken ){
  CPPUNIT_ASSERT( testToken->getDataType() == RestoreCkptMessage::getRestoreCkptMessageType() );
  CPPUNIT_ASSERT_EQUAL( getDefaultCkptTime(),
                        testToken->getCheckpointTime() );
  CPPUNIT_ASSERT_EQUAL( getDefaultSender(),
                        testToken->getSender() );
  CPPUNIT_ASSERT_EQUAL( getDefaultReceiver(),
                        testToken->getReceiver() );
  CPPUNIT_ASSERT_EQUAL( getDefaultRestoreState(),
                        testToken->getTokenState() );
  CPPUNIT_ASSERT_EQUAL( getDefaultCkptConsensus(),
                        testToken->getCheckpointConsensus() );
}

void 
RestoreCkptMessageTest::testConstructor(){
  KernelMessageTest::testConstructor( testToken );
  checkDefaults( testToken );
}

void 
RestoreCkptMessageTest::testSerialization(){
  KernelMessageTest::testSerialization( testToken, RestoreCkptMessage::getRestoreCkptMessageType() );

  SerializedInstance *serialized = static_cast<Serializable *>(testToken)->serialize();
  RestoreCkptMessage *deserialized = dynamic_cast<RestoreCkptMessage *>(serialized->deserialize());
  checkDefaults( deserialized );
  delete deserialized;
}
