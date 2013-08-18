
#include "SimulationStream.h"

ostream& nl(ostream& os){
   ((SimulationStream &)os).flush();
   return os;
}


