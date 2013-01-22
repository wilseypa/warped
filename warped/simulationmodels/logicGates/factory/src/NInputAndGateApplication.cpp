#include "../include/TwoInputAndGateApplication.h"
#include "../include/NInputAndGate.h"
#include "../incoude/"

ArgumentParser & 
TwoInputAndGateApplication::getArgumentParser(){
  static ArgumentParser::ArgRecord args[] = {
    {"-simulate", "input file name",&inputFileName,ArgumentParser::STRING,false},
    
    {"","",0 }
   
  };

  static ArgumentParser *myArgParser = new ArgumentParser(args);
  return *myArgParser;
}
