#include "../include/TwoInputAndGateApplication.h"
#include <warped/WarpedMain.h>

int
main(int argc, char **argv){
  WarpedMain wm(new TwoInputAndGateApplication());
  
  return wm.main(argc, argv);
}

