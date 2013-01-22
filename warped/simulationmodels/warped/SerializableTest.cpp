#include "SerializableTest.h"
#include "warped/SerializedInstance.h"

// Create anything needed for the tests here
void 
SerializableTest::setUp() {    
}

// Release any allocated resources here
void 
SerializableTest::tearDown() {
}

void
SerializableTest::testSerialization( const Serializable *toSerialize,
				     const string &dataType ){
  CPPUNIT_ASSERT( toSerialize != 0 );
  const SerializedInstance *instance1 = toSerialize->serialize();
  CPPUNIT_ASSERT( instance1 != 0 );
  
  const SerializedInstance *instance2 = toSerialize->serialize();
  CPPUNIT_ASSERT( instance2 != 0 );

  CPPUNIT_ASSERT ( instance1 != instance2 );
  
  CPPUNIT_ASSERT_EQUAL( instance1->getSize(), instance2->getSize() );
  CPPUNIT_ASSERT( memcmp( &(*instance1->getData().begin()),
			  &(*instance2->getData().begin()), 
			  instance1->getSize() ) == 0 );

  CPPUNIT_ASSERT_EQUAL( dataType, instance1->getDataType() );
}

