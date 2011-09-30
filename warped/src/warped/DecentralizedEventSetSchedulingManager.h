#ifndef DECENTRALIZED_EVENTSET_SCHEDULING_MANAGER_H
#define DECENTRALIZED_EVENTSET_SCHEDULING_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "SchedulingManager.h"

class TimeWarpSimulationManager;
class SimulationObject;
class TimeWarpEventSet;
class SchedulingData;

/** The SchedulingManager abstract base class.

    This is the abstract base class for all decentralized eventset
    scheduler implementations in the kernel. Different scheduler
    implementations must be derived from this abstract class.

*/
class DecentralizedEventSetSchedulingManager : public SchedulingManager {
public:
   /**@name Public Class Methods of SchedulingManager. */
   //@{
   
   /// Virtual Destructor.
   virtual ~DecentralizedEventSetSchedulingManager();

   /** Initialize the scheduler.
      
       This is a pure virtual function that has to be overriden.

       @param eventSet Set of events that constitute the input-queue.
       @param simObjArray Vector of simulation objects.
   */
   virtual void initialize(TimeWarpEventSet *eventSet,
                           vector<SimulationObject*>* simObjArray);
  
   /** Set the Scheduling Data.
      
       @param schedulingData handle to scheduling Data having head of eventsets.
   */
   void setSchedulingData(SchedulingData* schedulingData) {
     mySchedulingData = schedulingData;
   }

   //@} // End of Public Class Methods of SchedulingManager.
protected:
   /**@name Protected Class Methods of SchedulingManager. */
   //@{

   /// Default Constructor.
  DecentralizedEventSetSchedulingManager(TimeWarpSimulationManager* simMgr);

   /// Handle to my simulation manager.
   TimeWarpSimulationManager *mySimulationManager;
  
   /// Handle to the input queue.
   TimeWarpEventSet *myEventSet;
  
   /// The scheduler's list of objects.
   vector<SimulationObject*>* simulationObjectsArray;

   /// The class containing the head of eventsets
   SchedulingData* mySchedulingData;

   //@} // End of Protected Class Methods of SchedulingManager.
};

#endif //ifdef CENTRALIZED_EVENTSET_SCHEDULING_MANAGER_H
