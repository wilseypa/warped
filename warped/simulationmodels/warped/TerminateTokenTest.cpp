#include "TerminateTokenTest.h"
#include <cppunit/extensions/HelperMacros.h>

void 
TerminateTokenTest::setUp(){
  testToken = new TerminateToken( getDefaultSender(), 
				  getDefaultReceiver(),
				  getDefaultTerminator() );
}

void 
TerminateTokenTest::tearDown(){
  delete testToken;
  testToken = 0;
}

void
TerminateTokenTest::checkDefaults( const TerminateToken *testToken ){
  CPPUNIT_ASSERT( testToken->getDataType() == TerminateToken::getTerminateTokenType() );
  CPPUNIT_ASSERT_EQUAL( getDefaultTerminator(),
			testToken->getTerminator() );
  CPPUNIT_ASSERT_EQUAL( getDefaultSender(),
			testToken->getSender() );
  CPPUNIT_ASSERT_EQUAL( getDefaultReceiver(),
			testToken->getReceiver() );
  CPPUNIT_ASSERT_EQUAL( TerminateToken::FIRST_CYCLE,
			testToken->getState() );
}

void 
TerminateTokenTest::testConstructor(){
  KernelMessageTest::testConstructor( testToken );
  checkDefaults( testToken );
}

void 
TerminateTokenTest::testSerialization(){
  KernelMessageTest::testSerialization( testToken, TerminateToken::getTerminateTokenType() );

  SerializedInstance *serialized = static_cast<Serializable *>(testToken)->serialize();
  TerminateToken *deserialized = dynamic_cast<TerminateToken *>(serialized->deserialize());
  checkDefaults( deserialized );
  delete deserialized;
}
