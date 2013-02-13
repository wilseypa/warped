#include "Iscas85Application.h"
#include <warped/WarpedMain.h>

int 
main(int argc, char **argv){
	WarpedMain wm(new Iscas85Application());

	return wm.main(argc, argv);
}

