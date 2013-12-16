

#ifndef THREADEDCHEBYFOSSILCOLLMANAGER_H_
#define THREADEDCHEBYFOSSILCOLLMANAGER_H_

#include "ThreadedOptFossilCollManager.h"

/** The ChebyFossilCollManager class.

    This class implements the decision function using the
    Chebyshev inequality.
*/
class ThreadedChebyFossilCollManager : public ThreadedOptFossilCollManager {
public:

    /**@name Public Class Methods of ChebyFossilCollManager. */
    //@{

    /** Constructor.

        @param simMgr Handle to the simulation manager.
        @param checkPeriod The checkpoint period.
        @param minimumSamples The minimum samples taken before calculation.
        @param maximumSamples The max samples taken before calculation stops.
        @param defaultLen The initial active history length of all objects.
        @param risk The probability of a catastrophic rollback.
    */
    ThreadedChebyFossilCollManager(ThreadedTimeWarpSimulationManager* simMgr,
                                   int checkPeriod,
                                   int minimumSamples,
                                   int maximumSamples,
                                   int defaultLen,
                                   double risk);

    /// Desctructor
    virtual ~ThreadedChebyFossilCollManager();

    /** This function samples the rollback time to determine the
        active history length of the simulation object.

        @param obj The simulation object rolled back.
        @param time The rollback time.
    */
    void sampleRollback(SimulationObject* obj, const VTime& time);

    /** Used to get/release the lock of the data structure used in 
        chebyshev's inequality
    */
    void getOfcChebyLock (int threadId, const string syncMech);

    void releaseOfcChebyLock (int threadId, const string syncMech);


    //@} // End of Public Class Methods of ChebyFossilCollManager.

protected:

    /**@name protected Class Methods of ChebyFossilCollManager. */
    //@{

    /// The rollback times for all of the simulation objects on this
    /// simulation manager.
    vector< vector<unsigned int> > samples;

    /// The current index for the sample vector.
    vector<unsigned int> sampleIndex;

    /// The current total rollback time for each simulation object.
    vector<int> total;

    /// Used for calculating the active history length.
    double errorTerm;
    
    /// cheby lock
    LockState* ofcChebyLock; 
    //@} // End of Protected Class Methods of ChebyFossilCollManager.
};




#endif /* ThreadedCHEBYFOSSILCOLLMANAGER_H_ */
