// See copyright notice in file Copyright in the root directory of this archive.

#include "ClockFrequencyManagerImplementationBase.h"
#include "TimeWarpSimulationManager.h"
#include <set>
#include <sched.h>
#include <stdio.h>

using namespace std;

ClockFrequencyManagerImplementationBase::ClockFrequencyManagerImplementationBase(TimeWarpSimulationManager* simMgr,
    int measurementPeriod, int numCPUs)
  :mySimulationManager(simMgr)
  ,myMeasurementPeriod(measurementPeriod)
  ,myMeasurementCounter(0)
  ,myNumCPUs(numCPUs) {

  ASSERT(mySimulationManager);
  myCommunicationManager = mySimulationManager->getCommunicationManager();
  ASSERT(myCommunicationManager);

  mySimulationManagerID = mySimulationManager->getSimulationManagerID();
  myNumSimulationManagers = mySimulationManager->getNumberOfSimulationManagers();

  myCPU = mySimulationManagerID % myNumCPUs;
}

ClockFrequencyManagerImplementationBase::~ClockFrequencyManagerImplementationBase() {
  setGovernorMode("ondemand");
}

void
ClockFrequencyManagerImplementationBase::poll() {
  cerr << "ClockFrequencyManagerImplementationBase::poll() called !!"
       << endl;
  abort();
}

bool
ClockFrequencyManagerImplementationBase::checkMeasurementPeriod() {
  // only the master may initiate the measurement process
  if (!iAmMaster())
    return false;

  if (++myMeasurementCounter == myMeasurementPeriod)
    myMeasurementCounter = 0;

  return (myMeasurementCounter == 0);
}

void
ClockFrequencyManagerImplementationBase::configure(SimulationConfiguration &configuration){
  registerWithCommunicationManager();
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(myCPU, &mask);
  sched_setaffinity((pid_t)getpid(), sizeof(cpu_set_t), &mask);

  setGovernorMode("userspace");
}

void
ClockFrequencyManagerImplementationBase::setGovernorMode(const char* governor) {
  if(!iAmMaster()) {
    return;
  }

  for ( int i = 0; i < myNumCPUs; i++ ) {
    ostringstream path;
    path << "/sys/devices/system/cpu/cpu" << i << "/cpufreq/scaling_governor";

    ofstream fp(path.str().c_str());
    if(fp.is_open()) {
      fp << governor;
      fp.close();
    }
    else {
      cerr << "Unable to set governor: " << path.str() << " does not exist" << endl;
      abort();
    }
  }
}

void
ClockFrequencyManagerImplementationBase::setCPUFrequency(int cpu_idx, const char* freq) {
  if(cpu_idx != myCPU && !iAmMaster()) {
    utils::debug << "Non-master node " << mySimulationManagerID << " on CPU "
        << myCPU << " trying to set frequency of CPU " << cpu_idx << endl;
    return;
  }

  ostringstream path;
  path << "/sys/devices/system/cpu/cpu" << cpu_idx << "/cpufreq/scaling_setspeed";

  ofstream fp(path.str().c_str());
  if(fp.is_open()) {
    fp << freq;
    fp.close();
  }
  else {
    cerr << "Unable to set frequency: " << path.str() << " does not exist" << endl;
    abort();
  }
}

void
ClockFrequencyManagerImplementationBase::populateAvailableFrequencies() {
  for ( int i = 0; i < myNumCPUs; i++ ) {
    ostringstream path;
    path << "/sys/devices/system/cpu/cpu" << i << "/cpufreq/scaling_available_frequencies";

    ifstream fp(path.str().c_str());
    string freq;
    if(fp.is_open()) {
      vector<string> f1;
      vector<string> intersect;

      while(fp >> freq)
          f1.push_back(freq.c_str());

      if(i == 0)
        myAvailableFreqs = f1;

      // should be the same for each cpu, but just in case
      set_intersection(f1.begin(), f1.end(), myAvailableFreqs.begin(),
          myAvailableFreqs.end(), back_inserter(intersect));
      myAvailableFreqs = intersect;

      fp.close();
    }
    else {
      cerr << "Unable to get available frequencies: " << path.str() << " does not exist" << endl;
      abort();
    }
  }
}

