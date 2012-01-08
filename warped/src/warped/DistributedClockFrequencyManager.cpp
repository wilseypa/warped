// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "warped.h"
#include "DistributedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackVectorMessage.h"
#include "CommunicationManager.h"

const int DistributedClockFrequencyManager::myAvailableDelays[] = {20,25,30};

DistributedClockFrequencyManager::DistributedClockFrequencyManager(TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int numCPUs, int firsize, bool dummy)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs, firsize, dummy)
{
  myNumAvailableDelays = sizeof(myAvailableDelays) / sizeof(int);
}

void
DistributedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    if(isMaster()) {
      resetMeasurementCounter();
      int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
      CFRollbackVectorMessage* msg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
      myCommunicationManager->sendMessage(msg, dest);
    }
  }
}

void
DistributedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(CFRollbackVectorMessage::dataType(), this);
}

void
DistributedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  myRound++;

  CFRollbackVectorMessage* msg = dynamic_cast<CFRollbackVectorMessage*>(kMsg);
  ASSERT(msg);

  std::vector<int> dat;
  msg->getData(dat);
  if(myRound == 1) {
    int r = mySimulationManager->getRollbacks();
    dat[mySimulationManagerID] = r - myLastRollbacks;
    //cout << "stuffing dat[" << mySimulationManagerID << "] with " << r - myLastRollbacks << endl;
    myLastRollbacks = r;
  }
  else if(myRound == 2) {
    for(int i = 0; i < dat.size(); ++i)
      myRollbackFilters[i].update(dat[i]);
    adjustFrequency(dat);
    myRound = 0;
  }

  if(!(isMaster() && myRound == 0)) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    CFRollbackVectorMessage* newMsg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}

void
DistributedClockFrequencyManager::configure(SimulationConfiguration &configuration) {
  ClockFrequencyManagerImplementationBase::configure(configuration);
  populateAvailableFrequencies();
  setCPUFrequency(myCPU, myAvailableFreqs[0]);
}

void
DistributedClockFrequencyManager::adjustFrequency(vector<int>& rb) {
  int min = myRollbackFilters[0].getData();
  int max = min;
  float avg = (float)min / (float)myNumSimulationManagers;
  for(int i = 1; i < myNumSimulationManagers; ++i) {
    int dat = myRollbackFilters[i].getData();
    min = MIN_FUNC(min,dat);
    max = MAX_FUNC(max,dat);
    avg += (float)dat / (float)myNumSimulationManagers;
  }
  int rollbacks = myRollbackFilters[mySimulationManagerID].getData();
  int delay = getNominalDelay();
  if(!myIsDummy){
    if(rollbacks == min) {
  //    cout << ". speeding up...";
  //    if(!myIsDummy)
  //      mySimulationManager->speedup();
      cout << ". speeding up";
      delay = myAvailableDelays[0];
    }
    else if(rollbacks == max) {
  //    cout << ". slowing down...";
  //    if(!myIsDummy)
  //      mySimulationManager->slowdown();
      cout << ". slowing down";
      delay = myAvailableDelays[myNumAvailableDelays - 1];
    }
    mySimulationManager->setDelayUs(delay);
  }
  writeCSVRow(mySimulationManagerID, rollbacks, rb[mySimulationManagerID], delay);
}

string
DistributedClockFrequencyManager::toString() {
  ostringstream out;
  out << "Distributed CFM, Period = " << getPeriod();
  return out.str();
}
