
#include "SimulationStream.h"

std::ostream& nl(std::ostream& os){
   ((SimulationStream &)os).flush();
   return os;
}


