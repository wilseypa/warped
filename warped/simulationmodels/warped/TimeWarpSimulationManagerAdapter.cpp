#include "TimeWarpSimulationManagerAdapter.h"
#include "ApplicationAdapter.h"

TimeWarpSimulationManagerAdapter::TimeWarpSimulationManagerAdapter() : 
  TimeWarpSimulationManager( 0, new ApplicationAdapter() ){}

TimeWarpSimulationManagerAdapter::~TimeWarpSimulationManagerAdapter(){
}
