#ifndef TERMINATE_TOKEN_TEST_H
#define TERMINATE_TOKEN_TEST_H

#include "TerminateToken.h"
#include "KernelMessageTest.h"

class TerminateTokenTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( TerminateTokenTest);
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  TerminateToken *testToken;
  
  static unsigned int getDefaultTerminator(){
    static unsigned int defaultTerminator = 123;
    return defaultTerminator;
  }

  static void checkDefaults( const TerminateToken *testToken );

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
