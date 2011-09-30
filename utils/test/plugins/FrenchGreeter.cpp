
// See copyright notice in file Copyright in the root directory of this archive.

#include "FrenchGreeter.h"

const string 
FrenchGreeter::getGreetings() const {
  static string myGreetings = "Bon Jour";
  return myGreetings;
}
  
const string 
FrenchGreeter::getPluginType() const {
  return "test";
}

const string 
FrenchGreeter::getPluginName() const {
  return "FrenchGreeter";
}


FrenchGreeter::FrenchGreeter(){}

FrenchGreeter::~FrenchGreeter(){}

extern "C"{
  void *allocateFrenchGreeter(){
    return new FrenchGreeter();
  }
}
