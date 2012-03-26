// See copyright notice in file Copyright in the root directory of this archive.

#include "CentralizedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackVectorMessage.h"
#include "CommunicationManager.h"
#include <utils/Debug.h>

using namespace std;

struct compfir {
	int operator()(FIRFilter<int>& a, FIRFilter<int>& b) const {
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
      CFRollbackVectorMessage* msg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
      myCommunicationManager->sendMessage(msg, dest);
    }
  }
}

void
CentralizedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(CFRollbackVectorMessage::dataType(), this);
}

// 2 experimental ways to set frequencies...
// first sends one round, LP0 always sets the frequencies
// second sends two rounds, slowest LP always sets the freqs
void
CentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  resetMeasurementCounter();

  CFRollbackVectorMessage* msg = dynamic_cast<CFRollbackVectorMessage*>(kMsg);
  ASSERT(msg);

  std::vector<int> dat;
  msg->getData(dat);

  int r = mySimulationManager->getRollbacks();
  dat[mySimulationManagerID] = r - myLastRollbacks;
  myLastRollbacks = r;
  if(isMaster()) {
    for(int i = 0; i < dat.size(); ++i)
      myRollbackFilters[i].update(dat[i]);
    adjustFrequencies(dat);
  }
  else {

    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    CFRollbackVectorMessage* newMsg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}


//void
//CentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
//  resetMeasurementCounter();
//  myRound++;
//
//  if(!(myRound == 2 && myStartedRound)) {
//    CFRollbackVectorMessage* msg = dynamic_cast<CFRollbackVectorMessage*>(kMsg);
//    ASSERT(msg);
//
//    std::vector<int> dat;
//    msg->getData(dat);
//    if(myRound == 1 && myStartedRound) {
//      int r = mySimulationManager->getRollbacks();
//      dat[mySimulationManagerID] = r - myLastRollbacks;
//      myLastRollbacks = r;
//    }
//
//    if(dat[mySimulationManagerID]) {
//      for(int i = 0; i < dat.size(); ++i)
//        myRollbackFilters[i].update(dat[i]);
//      if(myStartedRound && !myFirstTime) {
//        adjustFrequencies();
//        setMaster(false);
//      }
//
//      if(myFirstTime)
//        myFirstTime = false;
//      else {
//        vector<FIRFilter>::iterator itmax = max_element(myRollbackFilters.begin(), myRollbackFilters.end());
//        setMaster(mySimulationManagerID == itmax - myRollbackFilters.begin());
//        if(isMaster())
//          cout << "set master to " << mySimulationManagerID << endl;
//      }
//    }
//    else {
//      int r = mySimulationManager->getRollbacks();
//      dat[mySimulationManagerID] = r - myLastRollbacks;
//      myLastRollbacks = r;
//    }
//
//    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
//    CFRollbackVectorMessage* newMsg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
//    newMsg->setData(dat);
//    myCommunicationManager->sendMessage(newMsg, dest);
//  }
//  else {
//    myStartedRound = false;
//  }
//
//  delete kMsg;
//}

int
CentralizedClockFrequencyManager::variance(vector<FIRFilter<int> >& x) {
  float avg = 0.f;
  float v = 0.f;
  float n = (float)x.size();
  for(vector<FIRFilter<int> >::iterator it(x.begin()); it!=x.end(); ++it)
    avg += (float)(*it).getData();
  avg /= n;

  for(vector<FIRFilter<int> >::iterator it(x.begin()); it!=x.end(); ++it) {
    float d = (float)(*it).getData() - avg;
    v += d * d;
  }
  v /= n - 1;
  return (int)v;
}


void
CentralizedClockFrequencyManager::adjustFrequencies(vector<int>& d) {
  vector<int> freqs(myNumSimulationManagers, 1);

  int rollbacks = myRollbackFilters[mySimulationManagerID].getData();
  int delay = getNominalDelay();
  float min = myRollbackFilters[0].getData();
  float max = min;
  float avg = (float)min / (float)myNumSimulationManagers;
  for(int i = 1; i < myNumSimulationManagers; ++i) {
    float dat = myRollbackFilters[i].getData();
    min = MIN_FUNC(min,dat);
    max = MAX_FUNC(max,dat);
    avg += (float)dat / (float)myNumSimulationManagers;
  }

  float hyst = (max-min)*.1;
  int hystlow = avg - hyst/2;
  int hysthigh = hystlow + hyst;

  //  if(!myIsDummy) {
  //    vector<FIRFilter<int> >::iterator itmin = min_element(myRollbackFilters.begin(), myRollbackFilters.end(), compfir());
  //    vector<FIRFilter<int> >::iterator itmax = max_element(myRollbackFilters.begin(), myRollbackFilters.end(), compfir());
  //    freqs[itmin - myRollbackFilters.begin()] = 0;
  //    freqs[itmax - myRollbackFilters.begin()] = 2;
  //  }

  if(!myIsDummy) {
    for(int i = 0; i < myRollbackFilters.size(); ++i) {
        if(myRollbackFilters[i].getData() < hystlow)
          freqs[i] = 0;
        else if(myRollbackFilters[i].getData() > hysthigh)
          freqs[i] = 2;
        else
          freqs[i] = myLastFreqs[i];
        if(freqs[i] != myLastFreqs[i])
          setCPUFrequency(i, myAvailableFreqs[freqs[i]]);
        myLastFreqs[i] = freqs[i];
    }
  }

  for(int i = 0; i < myRollbackFilters.size(); ++i) {
    writeCSVRow(i, myRollbackFilters[i].getData(), d[i], myAvailableFreqs[freqs[i]], hystlow, hysthigh);
  }
}

string
CentralizedClockFrequencyManager::toString() {
  ostringstream out;
  if(myIsDummy)
    out << "Dummy ";
   out << "Centralized CFM, Period = " << getPeriod() << ", FIR size = " << myFIRSize;
  return out.str();
}
