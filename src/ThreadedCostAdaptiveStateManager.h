#ifndef THREADEDCOSTADAPTIVESTATEMANAGER_HH
#define THREADEDCOSTADAPTIVESTATEMANAGER_HH


#include <vector>                       // for vector

#include "ObjectID.h"                   // for ObjectID
#include "StopWatch.h"                  // for StopWatch
#include "ThreadedStateManagerImplementationBase.h"
#include "controlkit/FIRFilter.h"
#include "controlkit/IIRFilter.h"       // for IIRFilter

class SimulationConfiguration;
class SimulationObject;
class ThreadedTimeWarpSimulationManager;
class VTime;

/** The ThreadedCostAdaptiveStateManager class.

 This class is an implementation of the adaptive state manager base.
 The adaptive algorithm is Fleischman's and Wilsey's heuristic
 State Saving algorithm.

 */

class ThreadedCostAdaptiveStateManager: public ThreadedStateManagerImplementationBase {
public:

    /**@name Public Class Methods of ThreadedCostAdaptStateManager. */
    //@{

    /** Constructor.

     @param simMgr Simulation manager.
     */
    ThreadedCostAdaptiveStateManager(ThreadedTimeWarpSimulationManager* simMgr);

    // Destructor
    ~ThreadedCostAdaptiveStateManager() {
    }
    ;

    void configure(SimulationConfiguration&) {
    }
    ;

    /** Start StopWatch to time state saving.
     @param id The simulation object id of the object.
     */
    void startStateTiming(unsigned int id);

    /** Stop state StopWatch.
     @param id The simulation object id of the object.
     */
    void stopStateTiming(unsigned int id);

    /** Calculates the new period for the given object.

     @param object The object for which the new period is calculated.
     */
    void calculatePeriod(SimulationObject* object);

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
    void saveState(const VTime& currentTime, SimulationObject* object,
                   int threadId);
    void saveState(const VTime& currentTime, unsigned int eventNumber,
                   SimulationObject* object, const ObjectID senderId, int threadId);

    const unsigned int getEventIdForRollback(int threadId, int objId);

    void updateStateWhileCoastForward(const VTime& currentTime,
                                      unsigned int eventNumber, SimulationObject* object,
                                      const ObjectID senderId, int threadId);

    const VTime& restoreState(const VTime& rollbackTime,
                              SimulationObject* object, int threadID);
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

    /// The StopWatch object.
    vector<StopWatch> stopWatch;

    /// Weighted filtered time to save one state.
    vector<IIRFilter<double> > StateSaveTimeWeighted;

    /// Weighted filtered time to coast forward.
    vector<IIRFilter<double> > CoastForwardTimeWeighted;

    //@} // end of Private Class Attributes
};

#endif
