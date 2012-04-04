#ifndef DTADAPTIVESTATEMANAGERBASE_HH
#define DTADAPTIVESTATEMANAGERBASE_HH

// See copyright notice in file Copyright in the root directory of this archive.

#include "DTStateManagerImplementationBase.h"
#include "controlkit/FIRFilter.h"
#include "controlkit/IIRFilter.h"
#include "StopWatch.h"

/** The DTAdaptiveStateManagerBase class.

 This class is an implementation of the periodic state manager.
 Various adaptive state managers can be implemented using this class.

 */
class DTAdaptiveStateManagerBase: public DTStateManagerImplementationBase {

public:

	/**@name Public Class Methods of DTAdaptiveStateManagerBase. */
	//@{

	/** Constructor.

	 @param simMgr Simulation manager.
	 */
	DTAdaptiveStateManagerBase(DTTimeWarpSimulationManager *simMgr);

	/// Destructor.
	virtual ~DTAdaptiveStateManagerBase() {
	}
	;

	void configure(SimulationConfiguration &) {
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

	/** Save the simulation object's state.
	 @param currentTime The time of the state save.
	 @param object The simulation object being saved.
	 */
	virtual void saveState(const VTime& currentTime, SimulationObject *object,
			int threadId);

	/** Return time taken for coast forwarding.
	 @param id The simulation object id of the object.
	 @return double The coast forward time.
	 */
	double getCoastForwardTime(unsigned int id);

	//@} // end of Public Class Methods

	/**@name Public Class Attributes */
	//@{


	/// Filtered time to coast forward.
	vector<FIRFilter<double> > coastForwardTime;

	/// Weighted filtered time to save one state.
	vector<IIRFilter<double> > StateSaveTimeWeighted;

	/// Weighted filtered time to coast forward.
	vector<IIRFilter<double> > CoastForwardTimeWeighted;

	/// Weighted filtered rollback length.
	vector<IIRFilter<int> > rollbackLengthWeighted;

	//@} // end of Public Class Attributes

protected:

	/**@name Protected Class Attributes */
	//@{

	/// Flag that says we are in steady state, so no timings are needed.
	vector<bool> steadyState;

	//@} // end of Protected Class Attributes.

private:

	/**@name Private Class Attributes */
	//@{

	/// The StopWatch object.
	vector<StopWatch> stopWatch;

	//@} // end of Private Class Attributes
};

#endif
