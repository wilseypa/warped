#include "Iscas89Application.h"
#include <warped/WarpedMain.h>

int 
main(int argc, char **argv){
	WarpedMain wm(new Iscas89Application());

	return wm.main(argc, argv);
}

