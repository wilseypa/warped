#include "KernelMessageTest.h"
#include "warped/KernelMessage.h"

// Create anything needed for the tests here
void 
KernelMessageTest::setUp() {    
}

// Release any allocated resources here
void 
KernelMessageTest::tearDown() {
}

void 
KernelMessageTest::testConstructor( const KernelMessage *toTest ){
  CPPUNIT_ASSERT_EQUAL( KernelMessageTest::getDefaultSender(),
			toTest->getSender() );
  CPPUNIT_ASSERT_EQUAL( KernelMessageTest::getDefaultReceiver(),
			toTest->getReceiver() );
}

void 
KernelMessageTest::testSerialization( const KernelMessage *toTest, 
				      const string &dataType ){
  SerializableTest::testSerialization( toTest, 
				       dataType );

  SerializedInstance *serialized = toTest->serialize();
  const KernelMessage *deserialized = dynamic_cast<KernelMessage *>(serialized->deserialize());

  CPPUNIT_ASSERT( deserialized != toTest );
  CPPUNIT_ASSERT( deserialized->getSender() == KernelMessageTest::getDefaultSender() );
  CPPUNIT_ASSERT( deserialized->getReceiver() == KernelMessageTest::getDefaultReceiver() );

  delete serialized;
  delete deserialized;
}
