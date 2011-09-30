#ifndef ATOMIC_SIMULATION_OBJECT_STATE_H_
#define ATOMIC_SIMULATION_OBJECT_STATE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "VTime.h"
static const int NOBODY= -1;
class AtomicSimulationObjectState
{
public:
	AtomicSimulationObjectState() : lockOwner(NOBODY) {}
	~AtomicSimulationObjectState() {}
	bool releaseLock(const unsigned int &threadNumber) {
		//If Currently Working and we can set it to Available then return true else return false;
		return __sync_bool_compare_and_swap(&lockOwner, threadNumber, NOBODY);
	}
	bool setLock(const unsigned int &threadNumber) {
		//If Available and we can set it to Working then return true else return false;
		return __sync_bool_compare_and_swap(&lockOwner, NOBODY, threadNumber);
	}
	const bool hasLock(const unsigned int &threadNumber) const  {
		return (threadNumber==lockOwner);
	}
	const void showStatus()	{
		cout << "Locked By: " << lockOwner << endl;
	}
private:
	int lockOwner;
};

#endif /* ATOMIC_SIMULATION_OBJECT_STATE_H_ */
