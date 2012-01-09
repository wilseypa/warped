// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "warped.h"
#include "DistributedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackVectorMessage.h"
#include "CommunicationManager.h"

const int DistributedClockFrequencyManager::myAvailableDelays[] = {24,25,26};

DistributedClockFrequencyManager::DistributedClockFrequencyManager(TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int numCPUs, int firsize, bool dummy)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs, firsize, dummy)
{
  myNumAvailableDelays = sizeof(myAvailableDelays) / sizeof(int);
  myDelay = 25000;
}

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
    //if(rollbacks == min) {
    if(rollbacks < avg) {
      //delay = myAvailableDelays[0];
      myDelay -= 1000;
      if(myDelay < 0)
        myDelay = 0;
    }
    //else if(rollbacks == max) {
    else if(rollbacks > avg) {
      //delay = myAvailableDelays[myNumAvailableDelays - 1];
      myDelay += 1000;
    }
    //mySimulationManager->setDelayUs(delay);
    mySimulationManager->setDelayUs(myDelay);
  }
  //writeCSVRow(mySimulationManagerID, rollbacks, rb[mySimulationManagerID], delay);
  writeCSVRow(mySimulationManagerID, rollbacks, rb[mySimulationManagerID], myDelay);
}

string
DistributedClockFrequencyManager::toString() {
  ostringstream out;
  if(myIsDummy)
    out << "Dummy ";
  out << "Distributed CFM, Period = " << getPeriod();
  return out.str();
}
