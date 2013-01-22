// CppUnit Headers
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

// Add test case headers here
#include "AdaptiveOutputManagerTest.h"
#include "AdaptiveStateManagerBaseTest.h"
#include "AggressiveOutputManagerTest.h"
#include "CirculateInitializationMessageTest.h"
#include "CostAdaptiveStateManagerTest.h"
#include "DefaultSchedulingManagerTest.h"
#include "DefaultTimeWarpEventSetTest.h"
#include "DefaultTimeWarpEventSetOneAntiMsgTest.h"
#include "EventMessageTest.h"
#include "EventTest.h"
#include "KernelMessageTest.h"
#include "GVTUpdateMessageTest.h"
#include "InitializationMessageTest.h"
#include "LazyOutputManagerTest.h"
#include "MatternGVTMessageTest.h"
#include "NegativeEventMessageTest.h"
#include "RestoreCkptMessageTest.h"
#include "SerializedInstanceTest.h"
#include "SerializableTest.h"
#include "StartMessageTest.h"
#include "TerminateTokenTest.h"
#include "TimeWarpMultiSetTest.h"
#include "TimeWarpMultiSetOneAntiMsgTest.h"
#include "warped/WarpedMain.h"

// Add your tests to the suite here
CPPUNIT_TEST_SUITE_REGISTRATION( DynamicOutputManagerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( AdaptiveStateManagerBaseTest );
CPPUNIT_TEST_SUITE_REGISTRATION( AggressiveOutputManagerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( CirculateInitializationMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( CostAdaptiveStateManagerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( DefaultSchedulingManagerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( DefaultTimeWarpEventSetTest );
CPPUNIT_TEST_SUITE_REGISTRATION( DefaultTimeWarpEventSetOneAntiMsgTest );
CPPUNIT_TEST_SUITE_REGISTRATION( EventMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( EventTest );
CPPUNIT_TEST_SUITE_REGISTRATION( KernelMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( GVTUpdateMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( InitializationMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( LazyOutputManagerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( MatternGVTMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( NegativeEventMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RestoreCkptMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SerializedInstanceTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SerializableTest );
CPPUNIT_TEST_SUITE_REGISTRATION( StartMessageTest );
CPPUNIT_TEST_SUITE_REGISTRATION( TerminateTokenTest );
CPPUNIT_TEST_SUITE_REGISTRATION( TimeWarpMultiSetTest );
CPPUNIT_TEST_SUITE_REGISTRATION( TimeWarpMultiSetOneAntiMsgTest );
// CPPUNIT_TEST_SUITE_REGISTRATION( TimeWarpAppendQueueTest );

int main() { 
  WarpedMain::registerKernelDeserializers();

  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  bool result =  runner.run();

  return result;
}
