#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::ostream;
using std::vector;

/** The ArgumentParser class.  

    This file contains the declarations for a class which accepts a
    list of arguments with addresses of the variables they affect, and
    parses argv and argc to check for them.  Errors are dealt with as
    well.  The variables can either be boolean, char*, or
    list<char*>; which type a variable is is determined by a value
    of the enumerated type ArgumentParser::ArgType.  The last value in
    the list is a boolean that indicates if this paramter is mandatory.
    Mandatory parameters are displayed with a "*" when they are listed

    Here is an example of the use of this class:
    bool boolArg;
    bool intArg;
    string stringArg;

    static ArgumentParser::ArgRecord argList[] = {
      { "-boolArg", "help for boolean argument", &boolArg, ArgumentParser::BOOLEAN, true }, 
      { "-intArg", "help for integer argument", &intArg, ArgumentParser::INTEGER, true }, 
      { "-stringArg", "help for string argument", &stringArg, ArgumentParser::STRING, false },
      { "", "", 0 }
    };


    int main( int argc, char *argv[] ){
      arg1 = true;    // default initialization must occur before the
      arg2 = false;   // ArgParser is called!
      arg3 = NULL;
    
      ArgumentParser ap( argList );
      ap.checkArgs( argc, argv );
    }
    
*/
class ArgumentParser {
   friend ostream &operator<<(ostream &, ArgumentParser &);
public:
  enum ArgType {BOOLEAN, INTEGER, STRING};
   
  class ArgRecord {
  public:
    string argText;
    string argHelp;
    void *data;
    ArgType type;
    bool mandatoryFlag;
  };
  
  ArgumentParser(){}
  
  ArgumentParser( ArgRecord *recordPtr ) :
    argRecordArray( getArgArray( recordPtr ) ){}
  
  ~ArgumentParser() {}

  /**
     Takes a null terminated array of argument records and builds a vector
     out of them.
  */
  static vector<ArgRecord> getArgArray( ArgRecord[] );

  static vector<string> vectorifyArguments( int argc, char **argv, bool skipFirst );
  
  /**
      Check the arguments passed in.  Will take actions as per the argument
      records passed in.

      @param args A vector containing strings representing argv in a
      traditional system.
      @param complainOnError Whether to complain about unrecognized
      arguments or not.
  */
  void checkArgs( vector<string> &args, bool = true );

  void printUsage( const string &binaryName, ostream &stream ) const;

  friend ostream &operator<<(ostream &, const ArgumentParser &);
   
private:
  vector<ArgRecord> argRecordArray;
  
  /** This method checks the arguments passed in to see if there are any in
      the form "-blah" and complains, if the global var
      "complainAndExitOnError" is set to true. */
  void checkRemaining( vector<string> &args,
		       bool complainAndExitOnError) const;
};

ostream &operator<<(ostream &os, const ArgumentParser &ap);

#endif
