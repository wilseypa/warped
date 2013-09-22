#ifndef SERIALIZED_INSTANCE_TEST_H
#define SERIALIZED_INSTANCE_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "SerializedInstance.h"

class SerializedInstanceTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( SerializedInstanceTest);
  CPPUNIT_TEST(testBytes);
  CPPUNIT_TEST(testVectorChar);
  CPPUNIT_TEST(testString);
  CPPUNIT_TEST(testUnsigned);
  CPPUNIT_TEST(testInt);
  CPPUNIT_TEST(testInt64);
  CPPUNIT_TEST(testDouble);
  CPPUNIT_TEST(testSerialized);
  CPPUNIT_TEST(testComplex1);
  CPPUNIT_TEST(testComplex2);

  CPPUNIT_TEST_SUITE_END();

private:
  SerializedInstance *toTest;
  static Serializable *getTestSerializable();

public:
  // Test interface
  void setUp();
  void tearDown();

  void testBytes();
  void testVectorChar();
  void testString();
  void testUnsigned();
  void testInt();
  void testInt64();
  void testDouble();
  void testSerialized();
  void testComplex1();
  void testComplex2();
};

#endif
