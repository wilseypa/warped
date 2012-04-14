// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "warped.h"
#include "SimulatedDVFSManager.h"
#include "TimeWarpSimulationManager.h"
#include "UsefulWorkMessage.h"
#include "CommunicationManager.h"
#include <cmath>
#include <ctime>

SimulatedDVFSManager::SimulatedDVFSManager(TimeWarpSimulationManager* simMgr,
                                           int measurementPeriod,
                                           int firsize,
                                           bool fixed,
                                           bool debug,
                                           OptimizationGoal og,
                                           UsefulWorkMetric uwm)
  :DVFSManagerImplementationBase(simMgr,
                                 measurementPeriod,
                                 firsize,
                                 fixed,
                                 debug,
                                 og,
                                 uwm)
  ,mySimulatedFrequencyIdx((numSimulatedFrequencies - 1) / 2)
{}

void
SimulatedDVFSManager::poll() {
  if(checkMeasurementPeriod()) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UsefulWorkMessage* msg = new UsefulWorkMessage(mySimulationManagerID,
                                                     dest,
                                                     myNumSimulationManagers,
                                                     UsefulWorkMessage::COLLECT);
    //cout << "beginning measurement" << endl;
    myCommunicationManager->sendMessage(msg, dest);
  }
}

void
SimulatedDVFSManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UsefulWorkMessage::dataType(),
                                              this);
}

void
SimulatedDVFSManager::configure(SimulationConfiguration &config) {
  DVFSManagerImplementationBase::configure(config);
  
  // set to highest frequency
  int freq = myAvailableFreqs[0];
  cout << "(" << mySimulationManagerID << "): bound to PE " << myCPU
       << "; initializing freq to " << freq << endl;
  setCPUFrequency(myCPU, freq);

  initializeFrequencyIdxs(numSimulatedFrequencies - 1);
}

void
SimulatedDVFSManager::receiveKernelMessage(KernelMessage* kMsg) {
  UsefulWorkMessage* msg = dynamic_cast<UsefulWorkMessage*>(kMsg);
  ASSERT(msg);

  std::vector<double> dat;
  UsefulWorkMessage::MessageRound round = msg->getRound();
  bool idxsChanged = false;
  msg->getData(dat);
  if(round == UsefulWorkMessage::COLLECT) {
    fillUsefulWork(dat);
    if(isMaster()) {
      for(int i = 0; i < dat.size(); i++)
        myUtilFilters[i].update(dat[i]);

      if(!isDummy())
        idxsChanged = updateFrequencyIdxs();

      if(debugPrint())
        for(int i = 0; i < dat.size(); i++)
          writeCSVRow(i, 
                      myUtilFilters[i].getData(), 
                      simulatedFrequencies[myFrequencyIdxs[i]]);
    }
  }
  else if(round == UsefulWorkMessage::SETFREQ && !isDummy())
    mySimulatedFrequencyIdx = static_cast<int>(dat[mySimulationManagerID]);

  // forward message to next node unless we're the master and either
  // we just received a set frequency message or we're not adjusting frequencies
  if(!(isMaster() && (round == UsefulWorkMessage::SETFREQ || !idxsChanged))) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UsefulWorkMessage::MessageRound newRound =
        isMaster() ? UsefulWorkMessage::SETFREQ : round;

    UsefulWorkMessage* newMsg = new UsefulWorkMessage(mySimulationManagerID,
                                                        dest,
                                                        myNumSimulationManagers,
                                                        newRound);

    if(isMaster())
      for(int i=0; i < myFrequencyIdxs.size(); i++)
        dat[i] = static_cast<double>(myFrequencyIdxs[i]);

    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}

string
SimulatedDVFSManager::toString() {
  return "Simulated DVFS, " + DVFSManagerImplementationBase::toString();
}

void
SimulatedDVFSManager::delay(int cycles) {
  warped64_t extracycles = 10 * cycles * (static_cast<double>(myAvailableFreqs[0]) /
                            simulatedFrequencies[mySimulatedFrequencyIdx] - 1);

  warped64_t start = rdtsc();
  warped64_t stop = start;
  int count = 0;
  while(stop - start < extracycles) {
    count++;
    stop = rdtsc();
  }
}

const int SimulatedDVFSManager::simulatedFrequencies[] =
  {2.8e6,
   2.7e6,
   2.6e6,
   2.5e6,
   2.4e6,
   2.3e6,
   2.2e6,
   2.1e6,
   2.0e6,
   1.9e6,
   1.8e6,
   1.7e6,
   1.6e6,
   1.5e6,
   1.4e6,
   1.3e6,
   1.2e6,
   1.1e6,
   1.0e6,
   0.9e6,
   0.8e6,
   0.7e6,
   0.6e6,
   0.5e6,
   0.4e6,
   0.3e6,
   0.2e6,
   0.1e6
};
const int SimulatedDVFSManager::numSimulatedFrequencies = 
  sizeof(simulatedFrequencies) / sizeof(int);
