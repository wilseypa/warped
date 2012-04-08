// See copyright notice in file Copyright in the root directory of this archive.

#include "ClockFrequencyManagerImplementationBase.h"
#include "TimeWarpSimulationManager.h"
#include <set>
#include <sched.h>
#include <cstdio>
#include <utils/Debug.h>

using namespace std;

ClockFrequencyManagerImplementationBase::ClockFrequencyManagerImplementationBase(TimeWarpSimulationManager* simMgr, int measurementPeriod, int firsize, bool dummy)
  :mySimulationManager(simMgr)
  ,myCommunicationManager(simMgr->getCommunicationManager())
  ,mySimulationManagerID(simMgr->getSimulationManagerID())
  ,myNumSimulationManagers(simMgr->getNumberOfSimulationManagers())
  ,myCPU(0)
  ,myFIRSize(firsize)
  ,myIsDummy(dummy) 
  ,myUtilFilters(myNumSimulationManagers, myFIRSize)
  ,myAvailableFreqs(0)
  ,myMeasurementPeriod(measurementPeriod)
  ,myMeasurementCounter(0)
  ,myAmMaster(mySimulationManagerID == 0)
{
  if(isMaster())
    myStopwatch.start();
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
/*  if (isMaster() && ++myMeasurementCounter == myMeasurementPeriod) {
    myMeasurementCounter = 0;
    return true;
  }
  return false;
  */
  if(isMaster() && myStopwatch.elapsed() > myMeasurementPeriod) {
    myStopwatch.reset();
    myStopwatch.start();
    return true;
  }
  return false;

}

void
ClockFrequencyManagerImplementationBase::writeCSVRow(int node,
                                                     double util,
                                                     int freq) {
  ostringstream path;
  path << "lp" << node << ".csv";

  ofstream fp(path.str().c_str(), ios_base::app);
  if(fp)
    fp << util << "," << freq << endl;
} 

void
ClockFrequencyManagerImplementationBase::configure(
        SimulationConfiguration &configuration) {
  registerWithCommunicationManager();
  myCPU = sched_getcpu();
  if(myCPU != mySimulationManagerID)
    cout << "WARNING: simulation manager id (" << mySimulationManagerID
         << ") differs from CPU id (" << myCPU << ")" << endl;
  setGovernorMode("userspace");

  populateAvailableFrequencies();
  setCPUFrequency(myCPU, myAvailableFreqs[0]);
}

void
ClockFrequencyManagerImplementationBase::setGovernorMode(const char* governor) {
  ostringstream path;
  path << "/sys/devices/system/cpu/cpu" << myCPU
       << "/cpufreq/scaling_governor";

  ofstream fp(path.str().c_str());
  if(fp.is_open()) {
    fp << governor;
    fp.close();
  }
  else {
    cerr << "Unable to set governor: " << path.str()
         << " does not exist" << endl;
    abort();
  }
}

void
ClockFrequencyManagerImplementationBase::setCPUFrequency(int cpu_idx, int freq) {
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
  ostringstream path;
  path << "/sys/devices/system/cpu/cpu" << myCPU
       << "/cpufreq/scaling_available_frequencies";

  ifstream fp(path.str().c_str());
  int freq;
  if(fp.is_open()) {
    while(fp >> freq)
      myAvailableFreqs.push_back(freq);
    fp.close();
  } else {
    cerr << "Unable to get available frequencies: " << path.str() << " does not exist" << endl;
    abort();
  }

  if(isMaster()) {
    std::vector<int>::iterator it(myAvailableFreqs.begin()); 
    std::cout << "frequencies:";
    for(; it != myAvailableFreqs.end(); ++it)
      std::cout << " [" << *it << "]";
    std::cout << std::endl;
  }
}

ostream& operator<<(ostream& out, ClockFrequencyManager& cfm) {
  out << cfm.toString();
  return out;
}

