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
{}

void
DecentralizedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage* msg = new UtilizationMessage(mySimulationManagerID,
                                                     dest,
                                                     myNumSimulationManagers);
    cout << "beginning measurement" << endl;
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
  if(myRound == 1)
    dat[mySimulationManagerID] = mySimulationManager->effectiveUtilization();
  else if(myRound == 2) {
    for(int i = 0; i < dat.size(); ++i)
      myUtilFilters[i].update(dat[i]);
    adjustFrequency();
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
    cout << "ending measurement" << endl;

  delete kMsg;
}

struct Utilization {
  int cpu;
  double util;
  bool operator()(const Utilization& a, const Utilization& b) const {
      return a.util < b.util;
  }
};


void
DecentralizedClockFrequencyManager::adjustFrequency() {
  vector<Utilization> utils(0);
  for(int i = 0; i < myUtilFilters.size(); i++) {
      Utilization u;
      u.cpu = i;
      u.util = myUtilFilters[i].getData();
      utils.push_back(u);
  }

  sort(utils.begin(), utils.end(), Utilization());

  int top = utils.size() / 2;
  int bottom = top - 1;
  int offset = 0;
  bool speedup = false;
  while(top < utils.size()) {
    if(fabs(utils[top].util - utils[bottom].util) > .1)
      offset++;
    if(utils[top].cpu == myCPU) {
      speedup = true;
      break;
    }
    else if(utils[bottom].cpu == myCPU) {
      speedup = false;
      break;
    }
    else {
      bottom--;
      top++;
    }
  }
  if(offset > 0)
    mySimulatedFrequencyIdx = speedup ? 4 - offset : 4 + offset;
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
                      1./myAvailableFreqs[0]) * 1000000000;
  timespec ts;
  ts.tv_nsec = ns;
  ts.tv_sec = 0;
  nanosleep(&ts, NULL);
  //cout << "slept " << ns << " nanoseconds, util was " << cycles << endl;
}

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
