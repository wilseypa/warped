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

CentralizedClockFrequencyManager::CentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int numCPUs)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs)
  ,myRollbackFilters(myNumSimulationManagers, 16)
  ,myFirstTime(true)
  ,myStartedRound(false)
  ,myLastRollbacks(0)
  ,myRound(0)
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


void
CentralizedClockFrequencyManager::configure(SimulationConfiguration &configuration) {
  ClockFrequencyManagerImplementationBase::configure(configuration);
  //populateAvailableFrequencies();
  if(isMaster()) {
//    for(int i=0; i < myNumSimulationManagers; ++i)
//      setCPUFrequency(i, myAvailableFreqs[1].c_str());
  }
}
/*
int
CentralizedClockFrequencyManager::variance(vector<FIRFilter>& x) {
  float avg = 0.f;
  float v = 0.f;
  float n = (float)x.size();
  for(vector<FIRFilter>::iterator it(x.begin()); it!=x.end(); ++it)
    avg += (float)(*it).getAverage();
  avg /= n;

  for(vector<FIRFilter>::iterator it(x.begin()); it!=x.end(); ++it) {
    float d = (float)(*it).getAverage() - avg;
    v += d * d;
  }
  v /= n - 1;
  return (int)v;
}
*/

void
CentralizedClockFrequencyManager::adjustFrequencies(vector<int>& d) {
  int freqs[] = {1,1,1,1};

//  so far haven't seen any help from checking the variance...
//  if(variance(myRollbackFilters) > 50) {
  vector<FIRFilter<int> >::iterator itmin = min_element(myRollbackFilters.begin(), myRollbackFilters.end(), compfir());
  vector<FIRFilter<int> >::iterator itmax = max_element(myRollbackFilters.begin(), myRollbackFilters.end(), compfir());
  freqs[itmin - myRollbackFilters.begin()] = 0;
  freqs[itmax - myRollbackFilters.begin()] = 2;
//  }

  cout << "rollbacks:";
  for(int i = 0; i < myRollbackFilters.size(); ++i) {
    ostringstream path;
    path << "rollbacks_lp" << i << ".csv";

    ofstream fp(path.str().c_str(), ios_base::app);
    if(fp.is_open()) {
      fp << d[i] << endl;
      fp.close();
    }

    cout << " [" << myRollbackFilters[i].getData() << "]";
  }
  cout << endl;
  
//  for(int i = 0; i < myRollbackFilters.size(); ++i)
//    setCPUFrequency(i, myAvailableFreqs[freqs[i]].c_str());

}
