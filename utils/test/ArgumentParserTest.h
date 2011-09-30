#ifndef ARG_PARSER_TEST_H
#define ARG_PARSER_TEST_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <string>
using std::string;

class ArgumentParser;

class ArgumentParserTest {
public:
  /**
     This is a method that runs a self test to see if the ArgumentParser class
     is working as expected.

     @return 0 if passed, non zero if failed.
  */
  int regressionTest();
  
  /**
     Return the singleton test object.
  */
  static ArgumentParserTest *instance();

protected:
  /** Destructor */
  ~ArgumentParserTest(){}

private:
  /** Constructor */
  ArgumentParserTest() : boolArg(false), intArg(0), stringArg(""){}

  int testConstruction();
  int testNoneMatch();
  int testBoolMatch();
  int testIntMatch();
  int testStringMatch();
  int testMultiple();

  bool boolArg;
  int  intArg;
  string stringArg;

  ArgumentParser *getDefaultArgumentParser();
};
#endif
