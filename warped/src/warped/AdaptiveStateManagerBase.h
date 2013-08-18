#ifndef ADAPTIVESTATEMANAGERBASE_HH
#define ADAPTIVESTATEMANAGERBASE_HH


#include "StateManagerImplementationBase.h"
#include "controlkit/FIRFilter.h"
#include "controlkit/IIRFilter.h"
#include "StopWatch.h"

/** The AdaptiveStateManagerBase class.

    This class is an implementation of the periodic state manager.
    Various adaptive state managers can be implemented using this class.

*/
class AdaptiveStateManagerBase: public StateManagerImplementationBase {

public:

   /**@name Public Class Methods of AdaptiveStateManagerBase. */
   //@{
  
   /** Constructor.
      
      @param simMgr Simulation manager.
   */
   AdaptiveStateManagerBase(TimeWarpSimulationManager *simMgr);

   /// Destructor.
   virtual ~AdaptiveStateManagerBase() {};

   void configure( SimulationConfiguration & ){};

   /** Start StopWatch to time event execution.
       @param id The simulation object id of the object.
   */
   void startEventTiming(unsigned int id);

   /** Stop StopWatch.
       @param id The simulation object id of the object.
   */
   bool stopEventTiming(unsigned int id);

   /** Pause StopWatch.
       @param id The simulation object id of the object.
   */
   void pauseEventTiming(unsigned int id);

   /** Resume StopWatch.
       @param id The simulation object id of the object.
   */
   void resumeEventTiming(unsigned int id);

   /** Start StopWatch to time rollback.
       @param id The simulation object id of the object.
   */
   void startRollbackTiming(unsigned int id);

   /** Stop rollback StopWatch.
       @param id The simulation object id of the object.
   */
   void finishRollbackTiming(unsigned int id);

   /** Start StopWatch to time state saving.
       @param id The simulation object id of the object.
   */
   void startStateTiming(unsigned int id);

   /** Stop state StopWatch.
       @param id The simulation object id of the object.
   */
   void stopStateTiming(unsigned int id);

   /** Save the simulation object's state.
       @param currentTime The time of the state save.
       @param object The simulation object being saved.
   */
   virtual void saveState(const VTime& currentTime, SimulationObject *object);

   /** Return time taken for state saving.
       @param id The simulation object id of the object.
       @return double The state save time.
   */
   double getStateSavingTime(unsigned int id);

   /** Return time taked for event execution.
       @param id The simulation object id of the object.
       @return double The event execution time.
   */
   double getEventExecutionTime(unsigned int id);

   /** Return time taken for coast forwarding.
       @param id The simulation object id of the object.
       @return double The coast forward time.
   */
   double getCoastForwardTime(unsigned int id);

   //@} // end of Public Class Methods

   /**@name Public Class Attributes */
   //@{

   /// filtered time to save one state.
   vector< FIRFilter<double> > stateSaveTime;

   /// Filtered time to execute one event.
   vector< FIRFilter<double> > eventExecutionTime;

   /// Summation of the event execution time.
   vector<double> sumEventTime;

   /// Filtered time to coast forward.
   vector< FIRFilter<double> > coastForwardTime;

   /// Filtered rollback length.
   vector< FIRFilter<int> > rollbackLength;

   /// Weighted filtered time to save one state.
   vector< IIRFilter<double> > StateSaveTimeWeighted;

   /// Weighted filtered time to execute one Event.
   vector< IIRFilter<double> > EventExecutionTimeWeighted;

   /// Weighted filtered time to coast forward.
   vector< IIRFilter<double> > CoastForwardTimeWeighted;

   /// Weighted filtered rollback length.
   vector< IIRFilter<int> > rollbackLengthWeighted;

   //@} // end of Public Class Attributes

protected:

   /**@name Protected Class Methods */
   //@{

   /** Add execution time to the total time .
   */
   bool executionTiming(unsigned int id, double execTime);

   //@}

   /**@name Protected Class Attributes */
   //@{

   /// Variable used to store the starting time of save state operation.
   vector<double> stateStartTime;

   /// Variable used to store the stop time of save state operation.
   vector<double> stateStopTime;

   /// Variable used to store the time of event execution.
   vector<double> eventTime;

   /// Variable used to store the time of rollback operation.
   vector<double> rollbackTime;

   /// Variable used to store the time of event operation.
   vector<double> pausedTime;

   /// Rollback count of last iteration.
   vector<int> lastRollbackCount;

   /// Flag that says we are in steady state, so no timings are needed.
   vector<bool> steadyState;

   //@} // end of Protected Class Attributes.

private:

   /**@name Private Class Attributes */
   //@{

   /// The StopWatch object.
   vector<StopWatch> stopWatch;

   /// The event StopWatch object.
   vector<StopWatch> eventStopWatch;

   /// The rollback StopWatch object.
   vector<StopWatch> rollbackStopWatch;

   //@} // end of Private Class Attributes
};

#endif
