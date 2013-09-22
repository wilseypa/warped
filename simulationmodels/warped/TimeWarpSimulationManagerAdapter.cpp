#include "TimeWarpSimulationManagerAdapter.h"
#include "ApplicationAdapter.h"

TimeWarpSimulationManagerAdapter::TimeWarpSimulationManagerAdapter() :
    TimeWarpSimulationManager(new ApplicationAdapter()) {}

TimeWarpSimulationManagerAdapter::~TimeWarpSimulationManagerAdapter() {
}
