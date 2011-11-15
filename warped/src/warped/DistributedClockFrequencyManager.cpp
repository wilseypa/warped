// See copyright notice in file Copyright in the root directory of this archive.

#include "DistributedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackAverageMessage.h"
#include "CommunicationManager.h"
#include <utils/Debug.h>

DistributedClockFrequencyManager::DistributedClockFrequencyManager(TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int numCPUs)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs) {}

void
DistributedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    unsigned int r = mySimulationManager->getRollbacks();

    // initiate round 0
    CFRollbackAverageMessage* msg = new CFRollbackAverageMessage(0, 1, 0, 0.f, r, r);
    msg->update(r, myNumSimulationManagers);
    myCommunicationManager->sendMessage(msg, 1);
  }
}

void
DistributedClockFrequencyManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(CFRollbackAverageMessage::dataType(), this);
}

void
DistributedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  CFRollbackAverageMessage* msg = dynamic_cast<CFRollbackAverageMessage*>(kMsg);
  ASSERT(msg);

  int round = msg->getRound();
  double avg = msg->getRollbackAverage();
  unsigned int min = msg->getRollbackMin();
  unsigned int max = msg->getRollbackMax();
  bool master = iAmMaster();

  if(master && round == 0) {
    adjustFrequency(avg, min, max);
  }
  else if(!master) {
    if (round == 0)
      avg += (double)mySimulationManager->getRollbacks() / (double)myNumSimulationManagers;
    else
      adjustFrequency(avg, min, max);
  }

  if(!(master && round == 1)) {
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    int nextround = master ? round + 1 : round;
    CFRollbackAverageMessage* newMsg = NULL;
    newMsg = new CFRollbackAverageMessage(mySimulationManagerID, dest, nextround, avg, min, max);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}

void
DistributedClockFrequencyManager::configure(SimulationConfiguration &configuration) {
  ClockFrequencyManagerImplementationBase::configure(configuration);
  populateAvailableFrequencies();
}

void
DistributedClockFrequencyManager::adjustFrequency(double avgRollbacks, unsigned int min, unsigned int max) {
  // TODO: compute and set new frequencies
  utils::debug << "(" << mySimulationManagerID << ") Average rollbacks: " << avgRollbacks;
  utils::debug << " My rollbacks: " << mySimulationManager->getRollbacks();
  utils::debug << std::endl;
}
