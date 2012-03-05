// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "warped.h"
#include "DistributedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackVectorMessage.h"
#include "CommunicationManager.h"
#include <cmath>

struct compfir {
	int operator()(FIRFilter<int>& a, FIRFilter<int>& b) const {
		return a.getData() < b.getData();
	}
};

DistributedClockFrequencyManager::DistributedClockFrequencyManager(TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int numCPUs, int firsize, bool dummy)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs, firsize, dummy)
  ,myNominalDelay(5 * 1000)
  ,myDelayStep(3 * 1000)
  ,myDelay(myNominalDelay)
{}

void
DistributedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    CFRollbackVectorMessage* msg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
    myCommunicationManager->sendMessage(msg, dest);
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

struct Rollback {
  int cpu;
  int rollbacks;
	bool operator()(const Rollback& a, const Rollback& b) const {
		return a.rollbacks < b.rollbacks;
	}
};


void
DistributedClockFrequencyManager::adjustFrequency(vector<int>& rb) {
  /*
  vector<Rollback> tmp(4);
  tmp[0].cpu = 0;
  tmp[0].rollbacks = myRollbackFilters[0].getData();
  tmp[1].cpu = 1;
  tmp[1].rollbacks = myRollbackFilters[1].getData();
  tmp[2].cpu = 2;
  tmp[2].rollbacks = myRollbackFilters[2].getData();
  tmp[3].cpu = 3;
  tmp[3].rollbacks = myRollbackFilters[3].getData();
  sort(tmp.begin(), tmp.end(), Rollback());
  vector<Rollback>::iterator it = tmp.begin();
  int delay = myNominalDelay;
  for(; it != tmp.end(); ++it) {
    if((*it).cpu == mySimulationManagerID)
      break;
  }
  if(it - tmp.begin() > 1)
    delay = myAvailableDelays[2];
  else
    delay = myAvailableDelays[0];
  */

  int hystlow;
  int hysthigh;
  int rollbacks = myRollbackFilters[mySimulationManagerID].getData();

  if(!myIsDummy) {
    float min = myRollbackFilters[0].getData();
    float max = min;
    float avg = (float)min / (float)myNumSimulationManagers;
    for(int i = 1; i < myNumSimulationManagers; ++i) {
      float dat = myRollbackFilters[i].getData();
      min = MIN_FUNC(min,dat);
      max = MAX_FUNC(max,dat);
      avg += (float)dat / (float)myNumSimulationManagers;
    }

    float hyst = (max-min)*.5;
    hystlow = avg - hyst/2;
    hysthigh = hystlow + hyst;

    bool adjustDown = true;
    int lastRb = myRollbackFilters[0].getData();
    for(int i = 1; i < myNumSimulationManagers; ++i) {
      if(myRollbackFilters[i].getData() != lastRb)
        adjustDown = false;
    }

    if(rollbacks > hysthigh)
      myDelay = myNominalDelay + myDelayStep;
    else if(rollbacks < hystlow)
      myDelay = myNominalDelay - myDelayStep;
    else if(adjustDown)
      myDelay = myNominalDelay;
  }

  mySimulationManager->setDelayUs(myDelay);
  writeCSVRow(mySimulationManagerID, rollbacks, rb[mySimulationManagerID], myDelay, hystlow, hysthigh);
  //writeCSVRow(mySimulationManagerID, rollbacks, rb[mySimulationManagerID], myDelay);
}

string
DistributedClockFrequencyManager::toString() {
  ostringstream out;
  if(myIsDummy)
    out << "Dummy ";
  out << "Distributed CFM, Period = " << getPeriod() << ", FIR size = " << myFIRSize;
  return out.str();
}
