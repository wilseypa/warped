// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "warped.h"
#include "DistributedDVFSManager.h"
#include "TimeWarpSimulationManager.h"
#include "UsefulWorkMessage.h"
#include "CommunicationManager.h"
#include <cmath>
#include <ctime>

DistributedDVFSManager::DistributedDVFSManager(TimeWarpSimulationManager* simMgr,
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
{}

void
DistributedDVFSManager::poll() {
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
DistributedDVFSManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UsefulWorkMessage::dataType(),
                                              this);
}

void
DistributedDVFSManager::configure(SimulationConfiguration &config) {
  DVFSManagerImplementationBase::configure(config);
  
  // initialize the frequency index array now that we know how many
  // frequencies are available
  int maxidx = myAvailableFreqs.size() - 1;

  // build the frequency index array based on the number of P states available
  initializeFrequencyIdxs(maxidx);

  // initialize my frequency to the median frequency
  int freqIdx = maxidx / 2;
  cout << "initializing frequencies to " << myAvailableFreqs[freqIdx] << endl;
  setFrequencies(freqIdx);
}

void
DistributedDVFSManager::receiveKernelMessage(KernelMessage* kMsg) {
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
                      myAvailableFreqs[myFrequencyIdxs[i]]);
    }
  }
  else if(round == UsefulWorkMessage::SETFREQ && !isDummy())
    setFrequencies(static_cast<int>(dat[mySimulationManagerID]));

  // forward message to next node unless we're the master and either
  // we just received a set frequency message or the frequencies haven't changed
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
DistributedDVFSManager::toString() {
  return "Simulated DVFS, " + DVFSManagerImplementationBase::toString();
}

/*
void
DistributedDVFSManager::delay(int cycles) {
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
*/

void
DistributedDVFSManager::setFrequencies(int idx) {
  // for my thesis, i will only be using the distributed DVFS manager with the
  // school's beowulf cluster, in which the P-states of each core and physical
  // package in a node must be coordinated.  i will only be running one LP
  // on each node, so set the frequency of ALL cpus on that node.
  for(int i = 0; i < 8; i++) {
    setCPUFrequency(i, myFrequencyIdxs[idx]);
  }
}
