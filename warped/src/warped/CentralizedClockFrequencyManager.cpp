// See copyright notice in file Copyright in the root directory of this archive.

#include "CentralizedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackVectorMessage.h"
#include "CommunicationManager.h"
#include <utils/Debug.h>

using namespace std;

CentralizedClockFrequencyManager::CentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int numCPUs)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs) {}

void
CentralizedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    // initiate the measurement cycle
    CFRollbackVectorMessage* msg = new CFRollbackVectorMessage(0, 1, myNumSimulationManagers);
    myCommunicationManager->sendMessage(msg, 1);
  }
}

void
CentralizedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(CFRollbackVectorMessage::dataType(), this);
}

void
CentralizedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  CFRollbackVectorMessage* msg = dynamic_cast<CFRollbackVectorMessage*>(kMsg);
  ASSERT(msg);

  std::vector<int> data;
  msg->getData(data);
  data[mySimulationManagerID] = mySimulationManager->getRollbacks();

  if(iAmMaster()) {
    adjustFrequencies(data);
  }
  else {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    CFRollbackVectorMessage* newMsg = new CFRollbackVectorMessage(mySimulationManagerID, dest, myNumSimulationManagers);
    newMsg->setData(data);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}


void
CentralizedClockFrequencyManager::configure(SimulationConfiguration &configuration) {
  ClockFrequencyManagerImplementationBase::configure(configuration);
  if(iAmMaster())
    populateAvailableFrequencies();
}



void
CentralizedClockFrequencyManager::adjustFrequencies(std::vector<int>& lpRollbacks) {
  vector<Rollback> rollbacks(myNumCPUs);

  // if multiple LPs share a CPU, use average rollbacks
  for(int i = 0; i < rollbacks.size(); ++i) {
    rollbacks[i].CPU = i;
    rollbacks[i].average = 0.f;
    int k = 0;
    for(int j = 0; j < lpRollbacks.size(); j += myNumCPUs) {
      rollbacks[i].average += lpRollbacks[j];
      k++;
    }
    rollbacks[i].average /= (double)k;
  }

  sort(rollbacks.begin(), rollbacks.end());
  for(int i = 0; i < rollbacks.size(); ++i) {
    int maxfreqidx = myAvailableFreqs.size() - 1;
    int freqidx = MIN_FUNC(i, maxfreqidx);

    utils::debug << "(" << mySimulationManagerID << ") setting frequency of cpu "
        << rollbacks[i].CPU << " to " << myAvailableFreqs[freqidx] << endl;

    setCPUFrequency(rollbacks[i].CPU, myAvailableFreqs[freqidx].c_str());
  }
}
