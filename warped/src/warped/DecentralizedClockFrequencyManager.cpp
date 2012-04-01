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
  ,myFrequencyIdxs(myNumSimulationManagers, 4)
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
DecentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  UtilizationMessage* msg = dynamic_cast<UtilizationMessage*>(kMsg);
  ASSERT(msg);

  std::vector<double> dat;
  UtilizationMessage::MessageRound round = msg->getRound();
  bool adjust = true;
  msg->getData(dat);
  if(round == UtilizationMessage::COLLECT) {
    dat[mySimulationManagerID] = mySimulationManager->effectiveUtilization();
    if(isMaster()) {
      for(int i = 0; i < dat.size(); i++)
        myUtilFilters[i].update(dat[i]);

      if(!myIsDummy)
        adjust = adjustFrequency(dat);

      for(int i = 0; i < dat.size(); i++)
        writeCSVRow(i,
                    myUtilFilters[i].getData(),
                    simulatedFrequencies[myIsDummy ? 4 : static_cast<int>(dat[i])]);
    }
  }
  else if(round == UtilizationMessage::SETFREQ && !myIsDummy)
  {
    mySimulatedFrequencyIdx = static_cast<int>(dat[mySimulationManagerID]);
    //cout << "simulated frequency idx: " << mySimulatedFrequencyIdx << endl;
  }

  // forward message to next node unless we're the master and either
  // we just received a set frequency message or we're not adjusting frequencies
  if(!(isMaster() && (round == UtilizationMessage::SETFREQ || !adjust))) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UtilizationMessage::MessageRound newRound =
        isMaster() ? UtilizationMessage::SETFREQ : round;
    //cout << "sending " << (newRound == UtilizationMessage::SETFREQ ? "set freq" : "collect")
    //     << " message from " << mySimulationManagerID << " to " << dest << endl;

    UtilizationMessage* newMsg = new UtilizationMessage(mySimulationManagerID,
                                                        dest,
                                                        myNumSimulationManagers,
                                                        newRound);

    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }
  //else
  //  cout << "ending measurement" << endl;

  delete kMsg;
}

struct Utilization {
  int node;
  double util;
  bool operator()(const Utilization& a, const Utilization& b) const {
      return a.util < b.util;
  }
};


bool
DecentralizedClockFrequencyManager::adjustFrequency(vector<double>& d) {
  const float dist = 0.01;

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
  }
  avg /= n;

  sort(utils.begin(), utils.end(), Utilization());
  i = 0;
  int high = n;
  int low = -1;
  for(; i < n; i++) {
    if(utils[i].util < avg - dist)
      low = i;
    else if(utils[i].util > avg + dist)
      break;
  }
  high = i;

  bool adjust = false;
  while(low >= 0 && high < n) {
    // skip over any nodes that are already at the extremes
    while(low >= 0 && myFrequencyIdxs[utils[low].node] == n - 1)
      low--;
    while(high < n && myFrequencyIdxs[utils[high].node] == 0)
      high++;
    if(low >= 0 && high < n) {
      myFrequencyIdxs[utils[low--].node]++;
      myFrequencyIdxs[utils[high++].node]--;
      adjust = true;
    }
  }

  d = myFrequencyIdxs;
  return adjust;
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
  warped64_t extracycles = cycles * (static_cast<double>(myAvailableFreqs[7]) /
                            simulatedFrequencies[mySimulatedFrequencyIdx] - 1);

  warped64_t start = rdtsc();
  warped64_t stop = start;
  while(stop - start < extracycles)
    stop = rdtsc();
}

const int DecentralizedClockFrequencyManager::numSimulatedFrequencies = 9;
const int DecentralizedClockFrequencyManager::simulatedFrequencies[] =
  {1.5e6,
   1.4e6,
   1.3e6,
   1.2e6,
   1.1e6,
   1.0e6,
   0.9e6,
   0.8e6,
   0.7e6};
