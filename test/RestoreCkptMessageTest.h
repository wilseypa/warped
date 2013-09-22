#ifndef RESTORE_CKPT_MESSAGE_TEST_H
#define RESTORE_CKPT_MESSAGE_TEST_H

#include "RestoreCkptMessage.h"
#include "KernelMessageTest.h"

class RestoreCkptMessageTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( RestoreCkptMessageTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST_SUITE_END();

private:
  RestoreCkptMessage *testToken;
  
  static int getDefaultCkptTime(){
    static int defaultCkptTime = 123;
    return defaultCkptTime;
  }

  static RestoreCkptMessage::RestoreState getDefaultRestoreState(){
    static RestoreCkptMessage::RestoreState defaultRestoreState =  RestoreCkptMessage::SEND_TO_MASTER;
    return defaultRestoreState;
  }

  static bool getDefaultCkptConsensus(){
    static bool defaultCheckpointConsensus = false;
    return defaultCheckpointConsensus;
  }

  static void checkDefaults( const RestoreCkptMessage *testToken );

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
};

#endif
