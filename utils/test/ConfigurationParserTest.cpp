
// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include <utils/tokens.h>
#include <utils/ConfigurationParserHandle.h>
#include <utils/StringUtilities.h>
#include "ConfigurationParserTest.h"

#define CHECK(x) if( !(x) ){ retval = -1; goto end; }

ConfigurationParserTest *
ConfigurationParserTest::instance(){
  static ConfigurationParserTest *myConfigurationParserTest = new ConfigurationParserTest();
  
  return myConfigurationParserTest;
}


int
ConfigurationParserTest::checkConfig( const ConfigurationScope &outerScope ){
  int retval = 0;

  if( checkOuterScope( outerScope ) != 0 ){
    retval = -1;
    goto end;
  }

  if( checkDebugChoice( outerScope ) != 0 ){
    retval = -1;
    goto end;
  }

  if( checkSimulatorsScope( outerScope ) != 0 ){
    retval = -1;
    goto end;
  }

 end:
  return retval;
}

int
ConfigurationParserTest::checkOuterScope( const ConfigurationScope &outerScope ){
  int retval = 0;
  const vector<const ConfigurationChoice *> *outerChoices = 0;
  const vector<const ConfigurationScope *> *outerScopes = 0;


  if( outerScope.getScopeName()  != "" ){
    retval = -1;
    goto end;
  }

  outerChoices = outerScope.getNestedChoices();
  if( outerChoices == 0 || outerChoices->size() != 1 ){
    retval = -1;
    goto end;
  }

  outerScopes = outerScope.getNestedScopes();
  if( outerScopes == 0 || outerScopes->size() != 1 ){
    retval = -1;
    goto end;
  }

 end:
  delete outerChoices;
  return retval;
}

int
ConfigurationParserTest::checkDebugChoice( const ConfigurationScope &outerScope ){
  int retval = 0;
  const ConfigurationChoice *debugChoice = 0;
  const ConfigurationValue *debugValue = 0;
  const StringConfigurationValue *stringDebugValue = 0;

  
  debugChoice = outerScope.findChoice("debug");
  if( debugChoice == 0 ){
    retval = -1;
    goto end;
  }
  debugChoice = outerScope.findChoice("Debug");
  if( debugChoice == 0 ){
    retval = -1;
    goto end;
  }
  debugValue = debugChoice->getConfigurationValue();
  if( debugValue == 0 || dynamic_cast<const StringConfigurationValue *>(debugValue) == 0 ){
    retval = -1;
    goto end;
  }
  stringDebugValue = dynamic_cast<const StringConfigurationValue *>(debugValue);
  if( stringDebugValue->getStringValue() != "true" ){
    retval = -1;
    goto end;
  }

 end:
  return retval;
}

int
ConfigurationParserTest::checkSimulatorsScope( const ConfigurationScope &outerScope ){
  int retval = 0;

  const ConfigurationScope *simulatorsScope = 0;
  const ConfigurationScope *testSimulatorScope = 0;
  const vector<const ConfigurationScope *> *nestedScopes = 0;

  simulatorsScope = outerScope.findScope("simulators");
  CHECK( simulatorsScope != 0 );

  nestedScopes = simulatorsScope->getNestedScopes();
  CHECK( nestedScopes != 0 || nestedScopes->size() == 1 );

  testSimulatorScope = simulatorsScope->findScope("testsimulator");
  CHECK( testSimulatorScope != 0 );

  CHECK( checkTestSimulatorScope( *testSimulatorScope ) == 0 );

 end:
  delete nestedScopes;
  return retval;
}

int
ConfigurationParserTest::checkTestSimulatorScope( const ConfigurationScope &testSimulatorScope ){
  int retval = 0;

  const vector<const ConfigurationChoice *> *choices = 0;
  const ConfigurationChoice *modelsChoice = 0;
  const ConfigurationChoice *doublesChoice = 0;
  const ConfigurationChoice *intsChoice = 0;

  choices = testSimulatorScope.getNestedChoices();
  CHECK( choices != 0 || choices->size() == 3 );

  modelsChoice = testSimulatorScope.findChoice( "models" );
  CHECK( checkModelsChoice( modelsChoice ) == 0 );

  doublesChoice = testSimulatorScope.findChoice( "doubles" );
  CHECK( checkDoublesChoice( doublesChoice ) == 0 );

  intsChoice = testSimulatorScope.findChoice( "ints" );
  CHECK( checkIntsChoice( intsChoice ) == 0 );

 end:
  delete choices;
  return retval;
}

int
ConfigurationParserTest::checkModelsChoice( const ConfigurationChoice *modelsChoice ){
  int retval = 0;
  
  const ConfigurationValue *modelsValue = 0;
  const VectorConfigurationValue *modelsVectorValue = 0;
  const vector<const ConfigurationValue *> *values = 0;

  CHECK( modelsChoice != 0 );
    
  modelsValue = modelsChoice->getConfigurationValue();
  CHECK( modelsValue != 0 );

  modelsVectorValue = dynamic_cast<const VectorConfigurationValue *>(modelsValue);
  CHECK( modelsVectorValue != 0 );

  values = modelsVectorValue->getVectorValue();
  CHECK( values != 0 );
  CHECK( values->size() == 4 );

  CHECK( (*values)[0]->getStringValue() == "foo" );
  CHECK( (*values)[1]->getStringValue() == "bar" );
  CHECK( (*values)[2]->getStringValue() == "baz" );
  CHECK( (*values)[3]->getStringValue() == "baz2" );

 end:
  return retval;
}

bool
closeEnough( double one, double two ){
  double bigger, smaller;
  if( one > two ){
    bigger = one;
    smaller = two;
  }
  else{
    bigger = two;
    smaller = one;
  }
  
  return (bigger - smaller < 0.0001);
}

int
ConfigurationParserTest::checkDoublesChoice( const ConfigurationChoice *doublesChoice ){
  int retval = 0;
  
  const ConfigurationValue *doublesValue = 0;
  const VectorConfigurationValue *doublesVectorValue = 0;
  const vector<const ConfigurationValue *> *values = 0;

  CHECK( doublesChoice != 0 );
    
  doublesValue = doublesChoice->getConfigurationValue();
  CHECK( doublesValue != 0 );

  doublesVectorValue = dynamic_cast<const VectorConfigurationValue *>(doublesValue);
  CHECK( doublesVectorValue != 0 );

  values = doublesVectorValue->getVectorValue();
  CHECK( values != 0 );
  CHECK( values->size() == 2 );

  CHECK( closeEnough( (*values)[0]->getDoubleValue(), 2.71 ) );
  CHECK( closeEnough( (*values)[1]->getDoubleValue(), -2.17 ) );

 end:
  return retval;
}

int
ConfigurationParserTest::checkIntsChoice( const ConfigurationChoice *intsChoice ){
  int retval = 0;
  
  const ConfigurationValue *intsValue = 0;
  const VectorConfigurationValue *intsVectorValue = 0;
  const vector<const ConfigurationValue *> *values = 0;

  CHECK( intsChoice != 0 );
    
  intsValue = intsChoice->getConfigurationValue();
  CHECK( intsValue != 0 );

  intsVectorValue = dynamic_cast<const VectorConfigurationValue *>(intsValue);
  CHECK( intsVectorValue != 0 );

  values = intsVectorValue->getVectorValue();
  CHECK( values != 0 );
  CHECK( values->size() == 4 );

  CHECK( (*values)[0]->getIntValue() == -1 );
  CHECK( (*values)[1]->getIntValue() == 0 );
  CHECK( (*values)[2]->getIntValue() == 12 );
  CHECK( (*values)[3]->getIntValue() == 69 );

 end:
  return retval;
}


int 
ConfigurationParserTest::parseFileTest(){
  int retval = 0;

  const string testFileName = "configurationParserTest.cfg";
  const ConfigurationScope *outerScope = ConfigurationParserHandle::parseFile( testFileName );
  
  if( outerScope == 0 ){
    retval = -1;
  }
  else{
    retval = checkConfig( *outerScope );
  }

  return retval;
}

int 
ConfigurationParserTest::regressionTest(){
  int retval = 0;

  utils::enableDebug();

  retval = parseFileTest();
  if( retval != 0 ){
    goto end;
  }

 end:
  return retval;
}
