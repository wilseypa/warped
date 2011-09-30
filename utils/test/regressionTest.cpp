
// See copyright notice in file Copyright in the root directory of this archive.

#include <iostream>
#include "ArgumentParserTest.h"
#include "ConfigurationParserTest.h"
#include "FileManagerTest.h"
#include "PluginManagerTest.h"
#include "FieldIteratorTest.h"
using std::cerr;
using std::endl;

bool oneFailed = false;

void testFileManager(){
  int fileManagerStatus = FileManagerTest::instance()->regressionTest();
  if( fileManagerStatus != 0 ){
    cerr << "FileManager test failed." << endl;
    oneFailed = true;
  }
  else{
    cerr << "FileManager test passed." << endl;
  }
}

void testPluginManager(){
  int pluginManagerStatus = PluginManagerTest::instance()->regressionTest();
  if( pluginManagerStatus != 0 ){
    cerr << "PluginManager test failed." << endl;
    oneFailed = true;
  }
  else{
    cerr << "PluginManager test passed." << endl;
  }
}

void testConfigurationParser(){
  int configurationParserStatus = ConfigurationParserTest::instance()->regressionTest();
  if( configurationParserStatus != 0 ){
    cerr << "ConfigurationParser test failed." << endl;
    oneFailed = true;
  }
  else{
    cerr << "ConfigurationParser test passed." << endl;
  }
}

void testArgumentParser(){
  int argParserStatus = ArgumentParserTest::instance()->regressionTest();
  if( argParserStatus != 0 ){
    cerr << "ArgumentParser test failed." << endl;
    oneFailed = true;
  }
  else{
    cerr << "ArgumentParser test passed." << endl;
  }
}

void testFieldIterator(){
  int tokenizingIteratorStatus = FieldIteratorTest::instance()->regressionTest();
  if( tokenizingIteratorStatus != 0 ){
    cerr << "FieldIterator test failed." << endl;
    oneFailed = true;
  }
  else{
    cerr << "FieldIterator test passed." << endl;
  }
}

int
main( int argc, char *argv[] ){
  testArgumentParser();
  testConfigurationParser();
  testFileManager();
  testPluginManager();
  testFieldIterator();

  if( oneFailed == true ){
    cerr << "WARNING - AT LEAST ONE TEST FAILED" << endl;
  }
  else{
    cerr << "All tests passed" << endl;
  }
}

