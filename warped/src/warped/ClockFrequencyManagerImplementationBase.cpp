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
  ,myAmMaster(false)
  ,myNumCPUs(numCPUs) {

  ASSERT(mySimulationManager);
  myCommunicationManager = mySimulationManager->getCommunicationManager();
  ASSERT(myCommunicationManager);

  mySimulationManagerID = mySimulationManager->getSimulationManagerID();
  myAmMaster = (mySimulationManagerID == 0);
  myNumSimulationManagers = mySimulationManager->getNumberOfSimulationManagers();
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
  if (++myMeasurementCounter == myMeasurementPeriod)
    return true;
  return false;
}

void
ClockFrequencyManagerImplementationBase::configure(SimulationConfiguration &configuration){
  registerWithCommunicationManager();
  myCPU = sched_getcpu();
  if(myCPU != mySimulationManagerID)
    cout << "WARNING: simulation manager id (" << mySimulationManagerID << ") differs from CPU id (" << myCPU << ")" << endl;
  setGovernorMode("userspace");
}

void
ClockFrequencyManagerImplementationBase::setGovernorMode(const char* governor) {
  if(!isMaster()) {
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
  ostringstream path;
  path << "/sys/devices/system/cpu/cpu" << cpu_idx << "/cpufreq/scaling_setspeed";

  ofstream fp(path.str().c_str());
  if(fp.is_open()) {
    fp << freq;
    fp.close();

    cout << "(" << mySimulationManagerID << ") setting frequency of cpu "
      << cpu_idx << " to " << freq << endl;
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
    } else {
      cerr << "Unable to get available frequencies: " << path.str() << " does not exist" << endl;
      abort();
    }
  }

  if(isMaster()) {
    std::vector<string>::iterator it(myAvailableFreqs.begin()); 
    std::cout << "frequencies:";
    for(; it != myAvailableFreqs.end(); ++it)
      std::cout << " [" << *it << "]";
    std::cout << std::endl;
  }
}

