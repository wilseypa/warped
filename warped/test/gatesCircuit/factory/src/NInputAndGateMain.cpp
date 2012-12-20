#include "../include/NInputAndGateApplication.h"
#include <warped/WarpedMain.h>

int
main(int argc, char **argv){
  WarpedMain wm(new NInputAndGateApplication());
  
  return wm.main(argc, argv);
}

