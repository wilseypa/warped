// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "DistributedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "CFRollbackAverageMessage.h"
#include "CommunicationManager.h"

DistributedClockFrequencyManager::DistributedClockFrequencyManager(TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int numCPUs)
  :ClockFrequencyManagerImplementationBase(simMgr, measurementPeriod, numCPUs),
   myRollbacks(0),
   myLastRollbacks(0),
   myRba(0),
   myFreqidx(1) {}

void
DistributedClockFrequencyManager::poll() {
  if(checkMeasurementPeriod()) {
    resetMeasurementCounter();
    unsigned int rollbacks = mySimulationManager->getRollbacks();
    unsigned int r = rollbacks - myLastRollbacks;
    myLastRollbacks = rollbacks;
//    std::cout << "rollbacks for cpu " << myCPU << ": " << r << std::endl;
    myRollbacks.push_front(r);
    float size = (float) myRollbacks.size();
    if (size > 3)
      myRollbacks.pop_back();

    double avg = 0.f;
    std::deque<unsigned int>::iterator it(myRollbacks.begin());
    for(; it != myRollbacks.end(); ++it)
      avg += (double)*it / size;
      
    unsigned int a = (unsigned int)avg;
    if(myRba == 0)
      myRba = a;
    else {
      // if rollbacks increased, slow down the core
      int tmp;
      if(a > myRba)
        tmp = MIN_FUNC((int)myAvailableFreqs.size()-2, myFreqidx+1);
      // if rollbacks decreased, speed up the core
      else if(a < myRba)
        tmp = MAX_FUNC(0, myFreqidx-1);
      else
        return;

      if(myFreqidx != tmp) {
        myFreqidx = tmp;
        setCPUFrequency(myCPU, myAvailableFreqs[myFreqidx].c_str());
//      std::cout << "cpu " << myCPU << ": rba went from " <<
//        myRba << " to " << a << ". freqidx = " << myFreqidx
//        << std::endl;
      }
      myRba = a;
    }


    // initiate round 0
//    CFRollbackAverageMessage* msg = new CFRollbackAverageMessage(0, 1, 0, 0.f, r, r);
//    msg->update(r, myNumSimulationManagers);
//    myCommunicationManager->sendMessage(msg, 1);
  }
}

void
DistributedClockFrequencyManager::registerWithCommunicationManager() {
  //myCommunicationManager->registerMessageType(CFRollbackAverageMessage::dataType(), this);
}

void
DistributedClockFrequencyManager::receiveKernelMessage(KernelMessage* kMsg) {
  CFRollbackAverageMessage* msg = dynamic_cast<CFRollbackAverageMessage*>(kMsg);
  ASSERT(msg);

  int round = msg->getRound();
  double avg = msg->getRollbackAverage();
  unsigned int min = msg->getRollbackMin();
  unsigned int max = msg->getRollbackMax();
  bool master = isMaster();

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
  setCPUFrequency(myCPU, myAvailableFreqs[myFreqidx].c_str());
}

void
DistributedClockFrequencyManager::adjustFrequency(double avgRollbacks, unsigned int min, unsigned int max) {
  // TODO: compute and set new frequencies
  utils::debug << "(" << mySimulationManagerID << ") Average rollbacks: " << avgRollbacks;
  utils::debug << " My rollbacks: " << mySimulationManager->getRollbacks();
  utils::debug << std::endl;
}
