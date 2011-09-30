
// See copyright notice in file Copyright in the root directory of this archive.

#include "Debug.h"
#include <iostream>
using std::cout;

EatEverythingBuf *
EatEverythingBuf::instance(){
  static EatEverythingBuf *singleton = new EatEverythingBuf();
  return singleton;
}

ostream 
utils::debug( EatEverythingBuf::instance() );
  
void 
utils::enableDebug(){
  debug.rdbuf( cout.rdbuf() );
}
  
void 
utils::disableDebug(){
  debug.rdbuf( EatEverythingBuf::instance() );
}

bool
utils::debugEnabled(){
  return !(debug.rdbuf() == EatEverythingBuf::instance());
}


