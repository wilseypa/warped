#include "EventMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>

const string
EventMessageBaseTest::getDefaultGVTInfo(){
  return "(12, 23)";
}

void 
EventMessageBaseTest::testConstructor( const EventMessageBase *testMessage ){
  KernelMessageTest::testConstructor( testMessage );
}

void 
EventMessageBaseTest::testSerialization( const EventMessageBase *testMessage,
					 const string &messageType ){
  KernelMessageTest::testSerialization( testMessage, 
					messageType );
}
