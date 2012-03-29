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
  ,mySimulatedFrequencyIdx(4) // index of simulatedFrequencies
{
  ostringstream path;
  path << "cfmoutput_lp" << mySimulationManagerID << ".csv";

  myFile.open(path.str().c_str(), ios_base::app);
}

void
DecentralizedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage* msg = new UtilizationMessage(mySimulationManagerID,
                                                     dest,
                                                     myNumSimulationManagers);
    utils::debug << "beginning measurement" << endl;
    myCommunicationManager->sendMessage(msg, dest);
  }
}

void
DecentralizedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UtilizationMessage::dataType(),
                                              this);
}

void
DecentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  myRound++;

  UtilizationMessage* msg = dynamic_cast<UtilizationMessage*>(kMsg);
  ASSERT(msg);

  std::vector<double> dat;
  msg->getData(dat);
  if(myRound == 1) {
    dat[mySimulationManagerID] = mySimulationManager->effectiveUtilization();
    if(isMaster()) {
      for(int i = 0; i < dat.size(); i++) {
        myUtilFilters[i].update(dat[i]);
        if(myIsDummy)
          writeCSVRow(i, dat[i], simulatedFrequencies[4]);
      }
      if(myIsDummy)
        return;

      std::vector<double> utils(dat);
      adjustFrequency(dat);
      for(int i = 0; i < dat.size(); i++)
        writeCSVRow(i, utils[i], simulatedFrequencies[static_cast<int>(dat[i])]);

      // update the master's simulated frequency first
      mySimulatedFrequencyIdx = static_cast<int>(dat[mySimulationManagerID]);
    }
  }
  else if(myRound == 2) {
    mySimulatedFrequencyIdx = static_cast<int>(dat[mySimulationManagerID]);
    myRound = 0;
  }

  if(!(isMaster() && myRound == 0)) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage* newMsg = new UtilizationMessage(
                mySimulationManagerID, dest, myNumSimulationManagers);
    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }
  else
    utils::debug << "ending measurement" << endl;

  delete kMsg;
}

struct Utilization {
  int node;
  double util;
  bool operator()(const Utilization& a, const Utilization& b) const {
      return a.util < b.util;
  }
};


void
DecentralizedClockFrequencyManager::adjustFrequency(vector<double>& d) {
  const int slowIdx = 5;
  const int fastIdx = 3;
  const int medIdx = 4;
  const float dist = 0.05;

  vector<Utilization> utils(0);
  double avg = 0.;
  int i = 0;
  int n = myUtilFilters.size();
  for(; i < n; i++) {
    Utilization u;
    u.node = i;
    u.util = myUtilFilters[i].getData();
    avg += u.util;
    utils.push_back(u);
    d[i] = medIdx;
  }
  avg /= n;

  sort(utils.begin(), utils.end(), Utilization());
  i = 0;
  int high = n;
  int low = -1;
  for(; i < utils.size(); i++) {
    if(utils[i].util < avg - dist)
      low = i;
    else if(utils[i].util > avg + dist)
      break;
  }
  high = i;

  while(low >= 0 && high < n) {
    d[utils[low--].node] = slowIdx;
    d[utils[high++].node] = fastIdx;
  }

  //myFile << myUtilFilters[mySimulationManagerID].getData() << ','
  //       << simulatedFrequencies[mySimulatedFrequencyIdx] << endl;

  //cout << "(" << myCPU << "): " << utils[speedup ? top : bottom].util
  //     << ", " << simulatedFrequencies[mySimulatedFrequencyIdx] << endl;


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
  long ns = cycles * (1./simulatedFrequencies[mySimulatedFrequencyIdx] -
                      1./myAvailableFreqs[0]) * 1000000;
  timespec ts;
  ts.tv_nsec = ns;
  ts.tv_sec = 0;
  if(nanosleep(&ts, NULL))
    cout << "nanosleep error" << endl;
  //cout << "slept " << ns << " nanoseconds, util was " << cycles << endl;
}

const int DecentralizedClockFrequencyManager::numSimulatedFrequencies = 9;
const int DecentralizedClockFrequencyManager::simulatedFrequencies[] =
  {2100000,
   2000000,
   1900000,
   1800000,
   1700000,
   1600000,
   1500000,
   1400000,
   1300000};
