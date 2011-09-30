// See copyright notice in file Copyright in the root directory of this archive.

#include "SimulationStream.h"

ostream& nl(ostream& os){
   ((SimulationStream &)os).flush();
   return os;
}


