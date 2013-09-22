#include "SerializedInstanceTest.h"
#include "IntVTime.h"

// Create anything needed for the tests here
void 
SerializedInstanceTest::setUp() {
  toTest = new SerializedInstance( "fake" );
}

// Release any allocated resources here
void 
SerializedInstanceTest::tearDown() {
  delete toTest;
  toTest = 0;
}

Serializable *
SerializedInstanceTest::getTestSerializable(){
  static IntVTime serializable( 12 );
  return &serializable;
}


void 
SerializedInstanceTest::testBytes(){
  char *testString = "Foo bar baz";
  toTest->addBytes( testString, strlen( testString ) );
  char *returnString = new char[strlen(testString)];
  toTest->getBytes( returnString, strlen(testString) );
  CPPUNIT_ASSERT( strncmp( testString, returnString, strlen(testString ) ) == 0 );
  delete [] returnString;
}

void 
SerializedInstanceTest::testVectorChar(){
  char *testString = "Foo bar baz";
  vector<char> testVector;
  testVector.insert( testVector.end(), testString, testString + strlen( testString ) );
  toTest->addCharVector( testVector );
  vector<char> returnVector = toTest->getVectorChar();
  CPPUNIT_ASSERT( testVector == returnVector );
}

void 
SerializedInstanceTest::testString(){
  const string testString( "Foo bar baz" );
  toTest->addString( testString );	       
  CPPUNIT_ASSERT( testString == toTest->getString() );
}

void 
SerializedInstanceTest::testUnsigned(){
  unsigned int testInt = 112;
  toTest->addUnsigned( testInt );	       
  CPPUNIT_ASSERT( testInt == toTest->getUnsigned() );
}

void 
SerializedInstanceTest::testInt(){
  int testInt = 113;
  toTest->addInt( testInt );
  CPPUNIT_ASSERT( testInt == toTest->getInt() );
}

void 
SerializedInstanceTest::testInt64(){
  long long testLL = 119l;
  toTest->addInt64( testLL );
  CPPUNIT_ASSERT( testLL == toTest->getInt64() );
}

void 
SerializedInstanceTest::testSerialized(){
  toTest->addSerializable( getTestSerializable() );

  Serializable *returned = toTest->getSerializable();

  CPPUNIT_ASSERT( returned != 0 );
  CPPUNIT_ASSERT( returned->getDataType() == getTestSerializable()->getDataType() );

  delete returned;
}

void 
SerializedInstanceTest::testDouble(){
  double testDouble = 176.0;
  toTest->addDouble( testDouble );
  double diff = 0.0;
  double returned = toTest->getDouble();
  if( testDouble > returned ){
    diff = testDouble - returned;
  }
  else{
    diff = returned - testDouble;
  }
  CPPUNIT_ASSERT( diff >= 0.0 && diff < 0.00001 );
}

void 
SerializedInstanceTest::testComplex1(){
  const int myInt = 1264;
  const unsigned int myUnsigned = 172;
  const string myString = "never underestimate the power of denial";

  toTest->addInt( myInt );
  toTest->addUnsigned( myUnsigned );
  toTest->addString( myString );

  CPPUNIT_ASSERT( myInt == toTest->getInt() );
  CPPUNIT_ASSERT( myUnsigned == toTest->getUnsigned() );
  CPPUNIT_ASSERT( myString == toTest->getString() );
}

void 
SerializedInstanceTest::testComplex2(){
  const int myInt = 1264;
  const unsigned int myUnsigned = 172;
  const string myString = "always estimate the power of denial";

  toTest->addInt( myInt );
  toTest->addString( myString );
  toTest->addSerializable( getTestSerializable() );
  toTest->addUnsigned( myUnsigned );

  CPPUNIT_ASSERT( myInt == toTest->getInt() );
  CPPUNIT_ASSERT( myString == toTest->getString() );

  Serializable *returned = toTest->getSerializable();

  CPPUNIT_ASSERT( returned != 0 );
  CPPUNIT_ASSERT( getTestSerializable()->getDataType() == returned->getDataType() );
  delete returned;
  CPPUNIT_ASSERT( myUnsigned == toTest->getUnsigned() );
}
