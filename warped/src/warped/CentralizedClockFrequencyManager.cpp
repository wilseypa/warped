// See copyright notice in file Copyright in the root directory of this archive.

#include "CentralizedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "UtilizationMessage.h"
#include "CommunicationManager.h"
#include <utils/Debug.h>

using namespace std;

struct compfir {
    int operator()(FIRFilter<double>& a, FIRFilter<double>& b) const {
		return a.getData() < b.getData();
	}
};

CentralizedClockFrequencyManager::CentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int firsize, bool dummy)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, firsize, dummy)
  ,myLastFreqs(myNumSimulationManagers, 1)
  ,myStartedRound(false)
  ,myFirstTime(true)
   {}

void
CentralizedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    // initiate the measurement cycle
    if(isMaster()) {
      myStartedRound = true;
      myRound = 0;
      int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
      UtilizationMessage* msg = new UtilizationMessage(mySimulationManagerID, dest, myNumSimulationManagers);
      myCommunicationManager->sendMessage(msg, dest);
    }
  }
}

void
CentralizedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UtilizationMessage::dataType(), this);
}

// 2 experimental ways to set frequencies...
// first sends one round, LP0 always sets the frequencies
// second sends two rounds, slowest LP always sets the freqs
void
CentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  resetMeasurementCounter();

  UtilizationMessage* msg = dynamic_cast<UtilizationMessage*>(kMsg);
  ASSERT(msg);

  std::vector<double> dat;
  msg->getData(dat);

  dat[mySimulationManagerID] = mySimulationManager->effectiveUtilization();
  if(isMaster()) {
    for(int i = 0; i < dat.size(); ++i)
      myUtilFilters[i].update(dat[i]);
    adjustFrequencies(dat);
  }
  else {

    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage* newMsg = new UtilizationMessage(mySimulationManagerID, dest, myNumSimulationManagers);
    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}

void
CentralizedClockFrequencyManager::adjustFrequencies(vector<double>& d) {

}

string
CentralizedClockFrequencyManager::toString() {
  ostringstream out;
  if(myIsDummy)
    out << "Dummy ";
   out << "Centralized CFM, Period = " << getPeriod() << ", FIR size = " << myFIRSize;
  return out.str();
}
