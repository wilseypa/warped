// See copyright notice in file Copyright in the root directory of this archive.

#include "AdaptTestApplication.h"
#include <warped/WarpedMain.h>

int
main( int argc, char **argv ){
  WarpedMain wm( new AdaptTestApplication( 6, 6, "lazy", false ) );
  
  // Defaults - 6 objects, 6 stragglers, regenerate same events, no delays.
  return wm.main( argc, argv );
}

