#ifndef DEBUG_H
#define DEBUG_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <cstdio>
#include <iostream>

using std::ostream;
using std::streambuf;
using std::streamsize;

  
class EatEverythingBuf : public streambuf {
public:
  static EatEverythingBuf *instance();
  
  int sync (){ return 0; }
    
  int overflow(int){ return EOF + 1; }
  
  streamsize xsputn( char *, streamsize n ){ return n; }
  
private:
  EatEverythingBuf(){}
};

namespace utils {
  void enableDebug();
  void disableDebug();
  bool debugEnabled();

  extern ostream debug;

#define CDEBUG( x ) if( utils::debugEnabled() ){ utils::debug << x; }
}

#endif
