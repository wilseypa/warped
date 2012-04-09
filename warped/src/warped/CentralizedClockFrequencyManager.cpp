// See copyright notice in file Copyright in the root directory of this archive.

#include "CentralizedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "UtilizationMessage.h"
#include "CommunicationManager.h"
#include <utils/Debug.h>

using namespace std;

CentralizedClockFrequencyManager::CentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int firsize, bool dummy)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, firsize, dummy)
   {}

void
CentralizedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    // initiate the measurement cycle
    if(isMaster()) {
      int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
      UtilizationMessage* msg = new UtilizationMessage(mySimulationManagerID,
                                                       dest,
                                                       myNumSimulationManagers,
                                                       UtilizationMessage::COLLECT);
      myCommunicationManager->sendMessage(msg, dest);
    }
  }
}

void
CentralizedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UtilizationMessage::dataType(), this);
}

void
CentralizedClockFrequencyManager::configure(SimulationConfiguration &config) {
  // populate available frequencies and our CPU id, set userspace governor
  ClockFrequencyManagerImplementationBase::configure(config);

  // initialize the frequency index array now that we know how many
  // frequencies are available
  int n = myAvailableFreqs.size();
  vector<int>::iterator it(myFrequencyIdxs.begin());
  for(int i=0; i < myNumSimulationManagers; i++)
    myFrequencyIdxs.push_back(n / 2);
  myMaxFreqIdx = n - 1;

  // initialize my frequency to the median frequency
  int freq = myAvailableFreqs[n / 2];
  cout << "(" << mySimulationManagerID << "): bound to PE " << myCPU
       << "; initializing freq to " << freq << endl;
  setCPUFrequency(myCPU, freq);
}

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

    bool changed = false;
    if(!myIsDummy)
      changed = updateFrequencyIdxs();

    if(changed && !myIsDummy) {
      cout << "setting freqs..." << endl;
      for(int i=0; i < myFrequencyIdxs.size(); i++)
        setCPUFrequency(i, myAvailableFreqs[myFrequencyIdxs[i]]);
    }

    for(int i=0; i < myFrequencyIdxs.size(); i++)
      writeCSVRow(i, myUtilFilters[i].getData(), myAvailableFreqs[myIsDummy ?
        myAvailableFreqs.size() / 2 : myFrequencyIdxs[i]]);

  }
  else {

    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage* newMsg = new UtilizationMessage(mySimulationManagerID,
                                                        dest,
                                                        myNumSimulationManagers,
                                                        UtilizationMessage::COLLECT);
    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}

string
CentralizedClockFrequencyManager::toString() {
  ostringstream out;
  if(myIsDummy)
    out << "Dummy ";
   out << "Centralized CFM, Period = " << getPeriod() << ", FIR size = " << myFIRSize;
  return out.str();
}
