#ifndef SERIALIZABLE_TEST_H
#define SERIALIZABLE_TEST_H

#include "WarpedUnitTest.h"
#include "warped/Serializable.h"

class SerializableTest : public WarpedUnitTest {
  CPPUNIT_TEST_SUITE( SerializableTest );
  CPPUNIT_TEST_SUITE_END();

private:

public:
  // Test interface
  void setUp();
  void tearDown();

  static void testSerialization( const Serializable *toSerialize,
				 const string &dataType );
};

#endif
