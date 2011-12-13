// See copyright notice in file Copyright in the root directory of this archive.

#include "CentralizedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackVectorMessage.h"
#include "CommunicationManager.h"
#include <utils/Debug.h>

using namespace std;

CentralizedClockFrequencyManager::CentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int numCPUs)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs)
  ,myLastRollbacks(myNumSimulationManagers)
  ,myRollbackFIR(myNumSimulationManagers)
  ,myAverageRollbacks(myNumSimulationManagers)
  ,myRollbacks(myNumSimulationManagers)
  ,myFirstTime(true)
  ,myStartedRound(false)
  ,myDoAdjust(false)
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

void
CentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  resetMeasurementCounter();
  myRound++;

//  if(!(myRound == 2 && myStartedRound)) {
  CFRollbackVectorMessage* msg = dynamic_cast<CFRollbackVectorMessage*>(kMsg);
  ASSERT(msg);

  std::vector<int> data;
  msg->getData(data);
//    if(myRound == 1 && myStartedRound)
//      data[mySimulationManagerID] = mySimulationManager->getRollbacks();
//
//    if(data[mySimulationManagerID]) {
//      updateRollbacks(data);
//      if(myStartedRound && !myFirstTime) {
//        adjustFrequencies();
//        setMaster(false);
//      }
//      
//      if(myFirstTime)
//        myFirstTime = false;
//      else {
//        setMaster(mySimulationManagerID == mySlowestCPU);
//        if(isMaster())
//          cout << "set master to " << mySimulationManagerID << endl;
//      }
//    }
//    else {
//      data[mySimulationManagerID] = mySimulationManager->getRollbacks();
//    }

  data[mySimulationManagerID] = mySimulationManager->getRollbacks();
  if(isMaster()) {
    updateRollbacks(data);
    adjustFrequencies();
  }
  else {

    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    CFRollbackVectorMessage* newMsg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
    newMsg->setData(data);
    myCommunicationManager->sendMessage(newMsg, dest);
  }
//  else {
//    myStartedRound = false;
//  }

  delete kMsg;
}


void
CentralizedClockFrequencyManager::configure(SimulationConfiguration &configuration) {
  ClockFrequencyManagerImplementationBase::configure(configuration);
  populateAvailableFrequencies();
  if(isMaster()) {
    for(int i=0; i < myNumSimulationManagers; ++i)
      setCPUFrequency(i, myAvailableFreqs[1].c_str());
  }
}


//bool
//CentralizedClockFrequencyManager::checkMeasurementPeriod() {
//  // only the master may initiate the measurement process
//  if (!isMaster())
//    return false;
//  return ClockFrequencyManagerImplementationBase::checkMeasurementPeriod();
//}

int
CentralizedClockFrequencyManager::variance(vector<int>& x) {
  float avg = 0.f;
  float v = 0.f;
  float n = (float)x.size();
  for(vector<int>::iterator it(x.begin()); it!=x.end(); ++it)
    avg += (float)*it;
  avg /= n;

  for(vector<int>::iterator it(x.begin()); it!=x.end(); ++it) {
    float d = (float)*it - avg;
    v += d * d;
  }
  v /= n - 1;
  return (int)v;
}

int
CentralizedClockFrequencyManager::averageRollbacks(int lp) {
  double avg = 0.f;
  float size = (float)myRollbackFIR[lp].size();
  std::deque<int>::iterator it(myRollbackFIR[lp].begin());
  for(; it != myRollbackFIR[lp].end(); ++it)
    avg += (double)*it / size;
  return (int)avg;
}

void
CentralizedClockFrequencyManager::updateRollbacks(std::vector<int>& r) {
  for(int i = 0; i < r.size(); ++i) {
    myRollbackFIR[i].push_front(r[i] - myLastRollbacks[i]);
    if(myRollbackFIR[i].size() > 3)
      myRollbackFIR[i].pop_back();
    myAverageRollbacks[i] = averageRollbacks(i);
    myRollbacks[i].CPU = i;
    myRollbacks[i].average = myAverageRollbacks[i];
  }
  myLastRollbacks = r;
  sort(myRollbacks.begin(), myRollbacks.end());
}

void
CentralizedClockFrequencyManager::adjustFrequencies() {
  int freqs[] = {1,1,1,1};

  if(variance(myAverageRollbacks) > 50) {
    freqs[myRollbacks[0].CPU] = 0;
    freqs[myRollbacks[1].CPU] = 1;
    freqs[myRollbacks[2].CPU] = 1;
    freqs[myRollbacks[3].CPU] = 2;
  }

  cout << "rollbacks:";
  for(int i = 0; i < myAverageRollbacks.size(); ++i) {
    cout << " [" << myAverageRollbacks[i] << "]";
  }
  cout << endl;
  
  for(int i = 0; i < myAverageRollbacks.size(); ++i)
    setCPUFrequency(i, myAvailableFreqs[freqs[i]].c_str());

//  cout << "rollbacks:";
//  // if multiple LPs share a CPU, use average rollbacks
//  for(int i = 0; i < lpRollbacks.size(); ++i) {
//    rollbacks[i].CPU = i;
//    rollbacks[i].average = myCurrentRollbacks[i] - myLastRollbacks[i];
//    cout << " [" << rollbacks[i].average << "]";
//    rollbacks[i].average = 0.f;
//    int k = 0;
//    for(int j = 0; j < lpRollbacks.size(); j += myNumCPUs) {
//      rollbacks[i].average += lpRollbacks[j];
//      k++;
//    }
//    rollbacks[i].average /= (double)k;
//  }
//  cout << endl;


//  sort(rollbacks.begin(), rollbacks.end());
//  for(int i = 0; i < rollbacks.size(); ++i) {
////    int maxfreqidx = myAvailableFreqs.size() - 1;
////    int freqidx = MIN_FUNC(i, maxfreqidx);
//    int freqidx = i == 0 ? 0 :
//      i == 3 ? 2 : 1;
//
//    cout << "(" << mySimulationManagerID << ") setting frequency of cpu "
//        << rollbacks[i].CPU << " to " << myAvailableFreqs[freqidx] << endl;
//
//    setCPUFrequency(rollbacks[i].CPU, myAvailableFreqs[freqidx].c_str());
//  }
}
