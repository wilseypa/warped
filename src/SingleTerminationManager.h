#ifndef SINGLE_TERMINATION_MANAGER_H_
#define SINGLE_TERMINATION_MANAGER_H_


#include <TerminationManager.h>

class TimeWarpSimulationManager;
class KernelMessage;

/**
   If we are the only simulation manager then use this class
   so when we are done the simulation finishes
*/

class SingleTerminationManager : public TerminationManager {
public:
    SingleTerminationManager(TimeWarpSimulationManager* initSimulationManager)
        : mySimulationManager(initSimulationManager),
          simulationCompleteFlag(false) {};

    /**
       The simulation manager queries this method to see if we need to
       terminate the simulation or not.
    */
    bool terminateSimulation() {
        if (simulationCompleteFlag == true) {
            return true;
        } else {
            return false;
        }
    }

    /**
       Our simulation manager calls this to set our status as active.
    */
    void setStatusActive() {
        simulationCompleteFlag = false;
    }

    /**
       Our simulation manager calls this to set our status to passive.
    */
    void setStatusPassive() {
        simulationCompleteFlag = true;
    }

    void simulationComplete() {
        simulationCompleteFlag = true;
    }

    void receiveKernelMessage(KernelMessage* msg) {
        std::cerr << "Impossible to receive a message with only 1 simulation manager" << std::endl;
        abort();
    }

    void ofcReset() {
        simulationCompleteFlag = false;
        mySimManagerStatus = ACTIVE;
    }

private:

    /**
      Our simulation manager.
    */
    TimeWarpSimulationManager* mySimulationManager;

    /**
       This flag gets set when the simulation has completed.
    */
    bool simulationCompleteFlag;

    /**
       Where we note our simulation manager's status.
    */
    SIM_MGR_IDLE_STATUS mySimManagerStatus;

};

#endif
