#ifndef CONFIGURATION_PARSER_TEST_H
#define CONFIGURATION_PARSER_TEST_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <string>

class ConfigurationScope;
class ConfigurationChoice;

class ConfigurationParserTest {
public:
  /**
     This is a method that runs a self test to see if the ConfigurationParser class
     is working as expected.

     @return 0 if passed, non zero if failed.
  */
  int regressionTest();
  
  /**
     Return the singleton test object.
  */
  static ConfigurationParserTest *instance();

protected:
  /** Destructor */
  ~ConfigurationParserTest(){}

private:
  /** Constructor */
  ConfigurationParserTest(){}
  /** Check for proper contents of the config file. */
  int checkConfig( const ConfigurationScope &outerScope );
  int checkOuterScope( const ConfigurationScope &outerScope );
  int checkDebugChoice( const ConfigurationScope &outerScope );
  int checkSimulatorsScope( const ConfigurationScope &outerScope );
  int checkTestSimulatorScope( const ConfigurationScope &outerScope );
  int checkModelsChoice( const ConfigurationChoice *modelsChoice );
  int checkDoublesChoice( const ConfigurationChoice *doublesChoice );
  int checkIntsChoice( const ConfigurationChoice *intsChoice );

  int parseFileTest();
};
#endif
