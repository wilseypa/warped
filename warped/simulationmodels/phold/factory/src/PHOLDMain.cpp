// See copyright notice in file Copyright in the root directory of this archive.

#include "../include/PHOLDApplication.h"
#include <warped/WarpedMain.h>

int
main( int argc, char **argv ){
  WarpedMain wm( new PHOLDApplication() );
  
  return wm.main( argc, argv );
}
