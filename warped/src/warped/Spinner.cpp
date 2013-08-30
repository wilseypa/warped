#include <iostream>

#include "WarpedConfig.h"
#include "Spinner.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

void 
Spinner::spinIfRequested( const string &keyName,
			  SimulationConfiguration &configuration ){
  if( configuration.spinKeySet( keyName )){
    std::cerr << "Key " << keyName << " is set in the configuration.  About to "
	 << "enter infinite loop - you'll need to break it with the"
	 << " debugger.  See Spinner:" << __LINE__ 
	 << " in gdb to break this loop.";
#ifdef HAVE_UNISTD_H
    std::cerr << "\npid = " << getpid();
#endif
    std::cerr << std::endl;
    
    // This has to be volatile so it doesn't get optimized out.
    volatile bool x = true;
    // Spin until someone uses a debugger and flips x to false.
    while( x ){}
  }
}

