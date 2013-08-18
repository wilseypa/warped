
#include "SharedDVFSManager.h"
#include "TimeWarpSimulationManager.h"
#include "UsefulWorkMessage.h"
#include "CommunicationManager.h"
#include <Debug/Debug.h>

using namespace std;

SharedDVFSManager::SharedDVFSManager(TimeWarpSimulationManager* simMgr,
                                 int measurementPeriod,
                                 int firsize,
                                 Algorithm alg,
                                 bool debug,
                                 UsefulWorkMetric uwm,
                                 double threshold)

  :DVFSManagerImplementationBase(simMgr,
                                 measurementPeriod,
                                 firsize,
                                 alg,
                                 debug,
                                 uwm,
                                 threshold)
  ,myCPUs(myNumSimulationManagers)
{}

SharedDVFSManager::~SharedDVFSManager() {
  //setGovernorMode(myCPU, "ondemand");
}

void
SharedDVFSManager::poll() {
  if(checkMeasurementPeriod()) {
    // initiate the measurement cycle
    if(isMaster()) {
      int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
      UsefulWorkMessage* msg = new UsefulWorkMessage(mySimulationManagerID,
                                                     dest,
                                                     myNumSimulationManagers,
                                                     UsefulWorkMessage::COLLECT);
      myCommunicationManager->sendMessage(msg, dest);
    }
  }
}

void
SharedDVFSManager::registerWithCommunicationManager() {
  myCommunicationManager->registerMessageType(UsefulWorkMessage::dataType(), this);
}

void
SharedDVFSManager::configure(SimulationConfiguration &config) {
  // populate available frequencies and our CPU id, set userspace governor
  DVFSManagerImplementationBase::configure(config);

  // initialize the frequency index array now that we know how many
  // frequencies are available
  int maxidx = myAvailableFreqs.size() - 1;

  // build the frequency index array based on the number of P states available
  initializeFrequencyIdxs(maxidx);

  // initialize my frequency to the median frequency
  int freq = myAvailableFreqs[maxidx / 2];
  cout << "(" << mySimulationManagerID << "): bound to PE " << myCPU
       << "; initializing freq to " << freq << endl;
  setGovernorMode(myCPU, "userspace");
  setCPUFrequency(myCPU, freq);

  if(isMaster()) {
    myCPUs[0] = myCPU;
    UsefulWorkMessage* uwm = new UsefulWorkMessage(0,
                                                   1,
                                                   myNumSimulationManagers,
                                                   UsefulWorkMessage::CIRCULATECPU);
    myCommunicationManager->sendMessage(uwm, 1);
  }
}

void
SharedDVFSManager::receiveKernelMessage(KernelMessage* kMsg) {
  UsefulWorkMessage* msg = dynamic_cast<UsefulWorkMessage*>(kMsg);
  ASSERT(msg);

  std::vector<double> dat;
  msg->getData(dat);

  if(msg->getRound() == UsefulWorkMessage::CIRCULATECPU) {
    if(isMaster()) {
      for(int i = 1; i < myCPUs.size(); i++)
          myCPUs[i] = static_cast<int>(dat[i]);
    }
    else {
      dat[mySimulationManagerID] = static_cast<double>(myCPU);
      int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
      UsefulWorkMessage* newMsg = new UsefulWorkMessage(mySimulationManagerID,
                                                        dest,
                                                        myNumSimulationManagers,
                                                        UsefulWorkMessage::CIRCULATECPU);
      newMsg->setData(dat);
      myCommunicationManager->sendMessage(newMsg, dest);
    }
    delete msg;
    return;
  }

  // add our useful work index to the array
  fillUsefulWork(dat);
  if(isMaster()) {

    // update FIR filters with data. if we're the master, then we know
    // dat has all the current useful work indexes of all other LPs
    for(int i = 0; i < dat.size(); ++i)
      myUtilFilters[i].update(dat[i]);

    // if we're not staying at a fixed frequency, and the frequency indexes 
    // have changed, then set the new cpu frequencies
    if(!isDummy() && updateFrequencyIdxs())
      for(int i=0; i < myFrequencyIdxs.size(); i++)
        setCPUFrequency(myCPUs[i], myAvailableFreqs[myFrequencyIdxs[i]]);

    // write trace to csv
    if(debugPrint())
      for(int i=0; i < myFrequencyIdxs.size(); i++)
        writeCSVRow(i, 
                    myUtilFilters[i].getData(), 
                    myAvailableFreqs[myFrequencyIdxs[i]]);

  }
  else {

    // send the useful work array on to the next simulation manager
    int dest = (mySimulationManagerID + 1) % myNumSimulationManagers;
    UsefulWorkMessage* newMsg = new UsefulWorkMessage(mySimulationManagerID,
                                                        dest,
                                                        myNumSimulationManagers,
                                                        UsefulWorkMessage::COLLECT);
    newMsg->setData(dat);
    myCommunicationManager->sendMessage(newMsg, dest);
  }

  delete kMsg;
}

string
SharedDVFSManager::toString() {
  return "Shared DVFS, " + DVFSManagerImplementationBase::toString();
}
