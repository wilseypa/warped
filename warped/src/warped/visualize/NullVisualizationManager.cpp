// See copyright notice in file Copyright in the root directory of this archive.

#include "NullVisualizationManager.h"

NullVisualizationManager::NullVisualizationManager(TimeWarpSimMgrWithVisualization *mySimMgr) : VisualizationManager(mySimMgr) {}

NullVisualizationManager::~NullVisualizationManager() {}

int 
NullVisualizationManager::openVisualizationObject(const char *, VisualizationManager::DisplayKind, NullVisualizationManager::VisualizationTools) {
  return 0; // Quiet the compiler down.
}

int 
NullVisualizationManager::openVisualizationObject(const char *, VisualizationManager::DisplayKind, VisualizationManager::VisualizationTools,  VisualizationManager::DataType, void *) {
  return 0; // Quiet the compiler down.
}

void 
NullVisualizationManager::closeVisualizationObject(int) {}

void 
NullVisualizationManager::addListener(int, void *) {}

void 
NullVisualizationManager::updateVisalizationValue(int) {}

