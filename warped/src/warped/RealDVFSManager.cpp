// See copyright notice in file Copyright in the root directory of this archive.

#include "RealDVFSManager.h"
#include "TimeWarpSimulationManager.h"
#include "UsefulWorkMessage.h"
#include "CommunicationManager.h"
#include <utils/Debug.h>

using namespace std;

RealDVFSManager::RealDVFSManager(TimeWarpSimulationManager* simMgr,
                                 int measurementPeriod,
                                 int firsize,
                                 bool dummy,
                                 bool powersave,
                                 UsefulWorkMetric uwm)
  :DVFSManagerImplementationBase(simMgr,
                                 measurementPeriod,
                                 firsize,
                                 dummy,
                                 powersave,
                                 uwm)
{}

void
RealDVFSManager::poll() {
  if(checkMeasurementPeriod()) {
    // initiate the measurement cycle
    if(isMaster()) {
      int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
      UsefulWorkMessage* msg = new UsefulWorkMessage(mySimulationManagerID,
                                                       dest,
                                                       myNumSimulationManagers,
                                                       UsefulWorkMessage::COLLECT);
      myCommunicationManager->sendMessage(msg, dest);
    }
  }
}

void
RealDVFSManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UsefulWorkMessage::dataType(), this);
}

void
RealDVFSManager::configure(SimulationConfiguration &config) {
  // populate available frequencies and our CPU id, set userspace governor
  DVFSManagerImplementationBase::configure(config);

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
RealDVFSManager::receiveKernelMessage(KernelMessage* kMsg) {
  resetMeasurementCounter();

  UsefulWorkMessage* msg = dynamic_cast<UsefulWorkMessage*>(kMsg);
  ASSERT(msg);

  std::vector<double> dat;
  msg->getData(dat);

  fillUsefulWork(dat);
  if(isMaster()) {

    for(int i = 0; i < dat.size(); ++i)
      myUtilFilters[i].update(dat[i]);

    bool changed = false;
    if(!isDummy())
      changed = updateFrequencyIdxs();

    if(changed && !isDummy()) {
      cout << "setting freqs..." << endl;
      for(int i=0; i < myFrequencyIdxs.size(); i++)
        setCPUFrequency(i, myAvailableFreqs[myFrequencyIdxs[i]]);
    }

    for(int i=0; i < myFrequencyIdxs.size(); i++)
      writeCSVRow(i, myUtilFilters[i].getData(), myAvailableFreqs[isDummy() ?
        myAvailableFreqs.size() / 2 : myFrequencyIdxs[i]]);

  }
  else {

    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UsefulWorkMessage* newMsg = new UsefulWorkMessage(mySimulationManagerID,
                                                        dest,
                                                        myNumSimulationManagers,
                                                        UsefulWorkMessage::COLLECT);
    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}

string
RealDVFSManager::toString() {
  return "Real DVFS, " + DVFSManagerImplementationBase::toString();
}
