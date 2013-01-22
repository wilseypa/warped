#include "../include/FullAdderApplication.h"
#include <warped/WarpedMain.h>

int
main(int argc, char **argv){
  WarpedMain wm(new FullAdderApplication());
  
  return wm.main(argc, argv);
}

