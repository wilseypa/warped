#ifndef COSTADAPTIVESTATEMANAGER_HH
#define COSTADAPTIVESTATEMANAGER_HH


#include "AdaptiveStateManagerBase.h"

// The maximum period.
const unsigned int maxDefaultInterval = 30;

/** The CostAdaptiveStateManager class.

    This class is an implementation of the adaptive state manager base.
    The adaptive algorithm is Fleischman's and Wilsey's heuristic 
    State Saving algorithm.

*/
class CostAdaptiveStateManager : public AdaptiveStateManagerBase {
public:

   /**@name Public Class Methods of CostAdaptStateManager. */
   //@{
  
   /** Constructor.
      
      @param simMgr Simulation manager.
   */
   CostAdaptiveStateManager(TimeWarpSimulationManager *simMgr);

   // Destructor
   ~CostAdaptiveStateManager() {};

   /** Calculates the new period for the given object.
    
      @param object The object for which the new period is calculated.
   */
   void calculatePeriod( SimulationObject *object );

   /** Sets the coast foward time.

      @param coastforwardtime This value should be the elapsed time recorded during coast forward phase.
   */
   void coastForwardTiming(unsigned int id, double coastforwardtime);

   /** Returns the coast forward time.
   */
   double getCoastForwardTime(unsigned int id);

   /** Saves the state of the object. After the interval defined by eventsBetweenRecalculation
       is completed, the period is recalulated.

      @param currentTime The current time to save state.
      @param object The object for which the state is to be saved.
   */
   void saveState(const VTime& currentTime, SimulationObject *object);

   /** Sets the number of events to process before recalculating the state period.

      @param eventsBeforeRecalcuate The interval for calls to calculatePeriod.
   */
   void setAdaptiveParameters(unsigned int id, long eventsBeforeRecalcuate);

   //@} // end of Public Class Methods

protected:

   /**@name Private Class Attributes */
   //@{

   // Number of events between recalculation of the checkpoint.
   vector<long> eventsBetweenRecalculation;

   // Events executed since last checkpoint interval recalculation.
   vector<long> forwardExecutionLength;  

   // The value of the cost function.
   vector<double> filteredCostIndex;

   // The previous value of the cost function.
   vector<double> oldCostIndex;

   // Adaption value for period (magnitude of 1 and sign gives direction).
   vector<int> adaptionValue;

   //@} // end of Private Class Attributes
};

#endif
