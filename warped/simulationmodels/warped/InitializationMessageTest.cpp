#include "InitializationMessageTest.h"
#include <cppunit/extensions/HelperMacros.h>

const vector<string>
InitializationMessageTest::getDefaultObjectNames(){
  static vector<string> *retval = 0;
  if( retval == 0 ){
    retval = new vector<string>;
    for( int i = 0; i < 27; i++ ){
      retval->push_back(string("SimulationObject" + to_string(i)));
    }
  }
  return *retval;
}

unsigned int
InitializationMessageTest::getDefaultNumSimulationManagers(){
  return 77;
}

void 
InitializationMessageTest::setUp(){
  testMessage = new InitializationMessage( KernelMessageTest::getDefaultSender(), 
					   KernelMessageTest::getDefaultReceiver(),
					   getDefaultObjectNames(),
					   getDefaultNumSimulationManagers() );
}

void 
InitializationMessageTest::tearDown(){
  delete testMessage;
  testMessage = 0;
}

void 
InitializationMessageTest::testConstructor(){
  KernelMessageTest::testConstructor( testMessage );
  CPPUNIT_ASSERT( testMessage->getDataType() == InitializationMessage::getInitializationMessageType() );

  CPPUNIT_ASSERT( testMessage->getObjectNames() == getDefaultObjectNames() );
  CPPUNIT_ASSERT( testMessage->getNumSimulationManagers() == getDefaultNumSimulationManagers() );

}

void 
InitializationMessageTest::testSerialization(){
  KernelMessageTest::testSerialization( testMessage, 
					InitializationMessage::getInitializationMessageType() );
  
  SerializedInstance *serialized = static_cast<Serializable *>(testMessage)->serialize();
  InitializationMessage *deserialized = 
    dynamic_cast<InitializationMessage *>( serialized->deserialize() );

  CPPUNIT_ASSERT( deserialized->getDataType() == InitializationMessage::getInitializationMessageType() );

  CPPUNIT_ASSERT( deserialized->getObjectNames() == getDefaultObjectNames() );
  CPPUNIT_ASSERT( deserialized->getNumSimulationManagers() == getDefaultNumSimulationManagers() );
  delete deserialized;
}
