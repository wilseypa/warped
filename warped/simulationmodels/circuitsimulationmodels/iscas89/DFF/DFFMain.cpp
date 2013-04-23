#include "DFFApplication.h"
#include <warped/WarpedMain.h>

int 
main(int argc, char **argv){
	WarpedMain wm(new DFFApplication());

	return wm.main(argc, argv);
}

