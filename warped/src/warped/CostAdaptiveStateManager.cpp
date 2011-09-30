// See copyright notice in file Copyright in the root directory of this archive.

#include "CostAdaptiveStateManager.h"

// These are the default values.
const unsigned int defaultRecalculationPeriod = 100;
const unsigned int defaultAdaptionValue = 1;

CostAdaptiveStateManager::CostAdaptiveStateManager(TimeWarpSimulationManager *simMgr):
   AdaptiveStateManagerBase(simMgr){

   int numSimObjs = simMgr->getNumberOfSimulationObjects();
   eventsBetweenRecalculation.resize( numSimObjs, defaultRecalculationPeriod );
   forwardExecutionLength.resize( numSimObjs, 0 );
   adaptionValue.resize( numSimObjs, defaultAdaptionValue );
   oldCostIndex.resize( numSimObjs, 0 );
   filteredCostIndex.resize( numSimObjs, 0 );
   forwardExecutionLength.resize( numSimObjs, 0 );
}

void CostAdaptiveStateManager::setAdaptiveParameters(unsigned int id, long eventsBeforeRecalcuate) {
   eventsBetweenRecalculation[id] = eventsBeforeRecalcuate;
}

void CostAdaptiveStateManager::calculatePeriod( SimulationObject *object ) {

   OBJECT_ID *currentObjectID = object->getObjectID();
   unsigned int objId = currentObjectID->getSimulationObjectID();

   // The current period for this object.
   int period = objectStatePeriod[objId];

   // Calculate the raw current cost function.
   double stateTime = StateSaveTimeWeighted[objId].getData();
   double coastTime = CoastForwardTimeWeighted[objId].getData();
   double costIndex = stateTime + coastTime;

   // To prevent oscillations, calculate the filtered cost index.
   filteredCostIndex[objId] = 0.4 * filteredCostIndex[objId] + 0.6 * costIndex;

   // When the cost index is 0, continue to increase the period. Otherwise,
   // change the value as specified.
   if ( oldCostIndex[objId] > 0 ) {
      if (oldCostIndex[objId] > 1.2 * filteredCostIndex[objId] ) {
         period += adaptionValue[objId];
         oldCostIndex[objId] = filteredCostIndex[objId];
      } 
      else if ( oldCostIndex[objId] < 0.8 * filteredCostIndex[objId] ) {
         adaptionValue[objId] = -adaptionValue[objId];
         period += adaptionValue[objId];
         oldCostIndex[objId] = filteredCostIndex[objId];
      }
      if ( coastTime == 0 ) {
         adaptionValue[objId] = 1;
         period++;
         oldCostIndex[objId] = filteredCostIndex[objId];
      }
   } 
   else {
      period++;
      oldCostIndex[objId] = filteredCostIndex[objId];
   }

   // Make sure that the period stays in the range: 0 <= period <= 30.
   if (period < 0) {
      period = 0;
   } 
   else if (period > maxDefaultInterval) {
      period = maxDefaultInterval;
   }

   utils::debug << object->getName() << " period: " << period << "\n";

   // Reset values.
   forwardExecutionLength[objId] = 1;
   StateSaveTimeWeighted[objId].reset();
   CoastForwardTimeWeighted[objId].reset();

   objectStatePeriod[objId] = period;
}

void CostAdaptiveStateManager::coastForwardTiming(unsigned int id, double coastforwardtime ) {
   CoastForwardTimeWeighted[id].update(coastforwardtime);
}

double CostAdaptiveStateManager::getCoastForwardTime(unsigned int id) { 
   return(CoastForwardTimeWeighted[id].getData()); 
}

void CostAdaptiveStateManager::saveState(const VTime& currentTime, SimulationObject *object) {
   ObjectID *currentObjectID = object->getObjectID();
   unsigned int simObjectID = currentObjectID->getSimulationObjectID(); 

   // The period is only recalculated after the specified number of events.
   if (forwardExecutionLength[simObjectID] < eventsBetweenRecalculation[simObjectID]) {
      forwardExecutionLength[simObjectID]++;
   }
   else{
      calculatePeriod( object );
   }

   AdaptiveStateManagerBase::saveState(currentTime, object);
}
