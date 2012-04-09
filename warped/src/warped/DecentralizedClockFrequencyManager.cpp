// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "warped.h"
#include "DecentralizedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "UtilizationMessage.h"
#include "CommunicationManager.h"
#include <cmath>
#include <ctime>

struct compfir {
	int operator()(FIRFilter<int>& a, FIRFilter<int>& b) const {
		return a.getData() < b.getData();
	}
};

DecentralizedClockFrequencyManager::DecentralizedClockFrequencyManager(
    TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int firsize, bool dummy)
  :ClockFrequencyManagerImplementationBase(simMgr,
                                           measurementPeriod,
                                           firsize,
                                           dummy)
  ,mySimulatedFrequencyIdx(numSimulatedFrequencies / 2)
{}

void
DecentralizedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage* msg = new UtilizationMessage(mySimulationManagerID,
                                                     dest,
                                                     myNumSimulationManagers,
                                                     UtilizationMessage::COLLECT);
    //cout << "beginning measurement" << endl;
    myCommunicationManager->sendMessage(msg, dest);
  }
}

void
DecentralizedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UtilizationMessage::dataType(),
                                              this);
}

void
DecentralizedClockFrequencyManager::configure(SimulationConfiguration &config) {
  ClockFrequencyManagerImplementationBase::configure(config);
  
  // set to highest frequency
  int freq = myAvailableFreqs[0];
  cout << "(" << mySimulationManagerID << "): bound to PE " << myCPU
       << "; initializing freq to " << freq << endl;
  setCPUFrequency(myCPU, freq);

  myMaxFreqIdx = numSimulatedFrequencies - 1;
}

void
DecentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  UtilizationMessage* msg = dynamic_cast<UtilizationMessage*>(kMsg);
  ASSERT(msg);

  std::vector<double> dat;
  UtilizationMessage::MessageRound round = msg->getRound();
  bool idxsChanged = false;
  msg->getData(dat);
  if(round == UtilizationMessage::COLLECT) {
    dat[mySimulationManagerID] = mySimulationManager->effectiveUtilization();
    if(isMaster()) {
      for(int i = 0; i < dat.size(); i++)
        myUtilFilters[i].update(dat[i]);

      if(!myIsDummy)
        idxsChanged = updateFrequencyIdxs();

      for(int i = 0; i < dat.size(); i++)
        writeCSVRow(i,
                    myUtilFilters[i].getData(),
                    simulatedFrequencies[myIsDummy ? 
                      numSimulatedFrequencies / 2 : myFrequencyIdxs[i]]);
    }
  }
  else if(round == UtilizationMessage::SETFREQ && !myIsDummy)
    mySimulatedFrequencyIdx = static_cast<int>(dat[mySimulationManagerID]);

  // forward message to next node unless we're the master and either
  // we just received a set frequency message or we're not adjusting frequencies
  if(!(isMaster() && (round == UtilizationMessage::SETFREQ || !idxsChanged))) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage::MessageRound newRound =
        isMaster() ? UtilizationMessage::SETFREQ : round;

    UtilizationMessage* newMsg = new UtilizationMessage(mySimulationManagerID,
                                                        dest,
                                                        myNumSimulationManagers,
                                                        newRound);

    if(isMaster())
      for(int i=0; i < myFrequencyIdxs.size(); i++)
        dat[i] = static_cast<double>(myFrequencyIdxs[i]);

    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }
  //else
  //  cout << "ending measurement" << endl;

  delete kMsg;
}

string
DecentralizedClockFrequencyManager::toString() {
  ostringstream out;
  if(myIsDummy)
    out << "Dummy ";
  out << "Decentralized CFM, Period = " << getPeriod() << ", FIR size = "
      << myFIRSize;
  return out.str();
}

void
DecentralizedClockFrequencyManager::delay(int cycles) {
  warped64_t extracycles = cycles * (static_cast<double>(myAvailableFreqs[0]) /
                            simulatedFrequencies[mySimulatedFrequencyIdx] - 1);

  warped64_t start = rdtsc();
  warped64_t stop = start;
  while(stop - start < extracycles)
    stop = rdtsc();
}

const int DecentralizedClockFrequencyManager::simulatedFrequencies[] =
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
   0.8e6
};
const int DecentralizedClockFrequencyManager::numSimulatedFrequencies = 
  sizeof(simulatedFrequencies) / sizeof(int);
