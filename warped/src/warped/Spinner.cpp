#include <WarpedConfig.h>
#include <utils/ConfigurationScope.h>
#include "Spinner.h"

// See copyright notice in file Copyright in the root directory of this archive.

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

void 
Spinner::spinIfRequested( const string &keyName,
			  SimulationConfiguration &configuration ){
  if( configuration.spinKeySet( keyName )){
    cerr << "Key " << keyName << " is set in the configuration.  About to "
	 << "enter infinite loop - you'll need to break it with the"
	 << " debugger.  See Spinner:" << __LINE__ 
	 << " in gdb to break this loop.";
#ifdef HAVE_UNISTD_H
    cerr << "\npid = " << getpid();
#endif
    cerr << endl;
    
    // This has to be volatile so it doesn't get optimized out.
    volatile bool x = true;
    // Spin until someone uses a debugger and flips x to false.
    while( x ){}
  }
}

