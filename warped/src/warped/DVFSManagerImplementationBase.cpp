// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "DVFSManagerImplementationBase.h"
#include "TimeWarpSimulationManager.h"
#include <set>
#include <sched.h>
#include <cstdio>
#include <utils/Debug.h>

using namespace std;

DVFSManagerImplementationBase::DVFSManagerImplementationBase(
    TimeWarpSimulationManager* simMgr,
    int measurementPeriod,
    int firsize,
    bool dummy,
    bool debug,
    OptimizationGoal og,
    UsefulWorkMetric uwm)
  :mySimulationManager(simMgr)
  ,myCommunicationManager(simMgr->getCommunicationManager())
  ,mySimulationManagerID(simMgr->getSimulationManagerID())
  ,myNumSimulationManagers(simMgr->getNumberOfSimulationManagers())
  ,myCPU(0)
  ,myFIRSize(firsize)
  ,myUtilFilters(myNumSimulationManagers, myFIRSize)
  ,myAvailableFreqs(0)
  ,myFrequencyIdxs(0)
  ,myMeasurementPeriod(measurementPeriod)
  ,myMeasurementCounter(0)
  ,myMaxFreqIdx(0)
  ,myIsDummy(dummy) 
  ,myDebugPrint(debug)
  ,myUWM(uwm)
  ,myOG(og)
  ,myLastRollbacks(0)
  ,myLastEventsRolledBack(0)
  ,myLastEventsExecuted(0)
{
  if(isMaster())
    myStopwatch.start();
}

DVFSManagerImplementationBase::~DVFSManagerImplementationBase() {
}

void
DVFSManagerImplementationBase::poll() {
  cerr << "DVFSManagerImplementationBase::poll() called !!"
       << endl;
  abort();
}

bool
DVFSManagerImplementationBase::checkMeasurementPeriod() {
  if(isMaster() && myStopwatch.elapsed() > myMeasurementPeriod) {
    myStopwatch.reset();
    myStopwatch.start();
    return true;
  }
  return false;
}

void
DVFSManagerImplementationBase::writeCSVRow(int node,
                                                     double util,
                                                     int freq) {
  ostringstream path;
  path << "lp" << node << ".csv";

  ofstream fp(path.str().c_str(), ios_base::app);
  if(fp)
    fp << util << "," << freq << endl;
} 

void
DVFSManagerImplementationBase::configure(
        SimulationConfiguration &configuration) {
  registerWithCommunicationManager();
#ifdef HAVE_SCHED_GETCPU
  myCPU = sched_getcpu();
#else
  cerr << "sched_getcpu() required to use DVFS" << endl;
  abort();
#endif
  populateAvailableFrequencies();
}

void
DVFSManagerImplementationBase::setGovernorMode(int cpu, const char* governor) {
  ostringstream path;
  path << "/sys/devices/system/cpu/cpu" << cpu
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
DVFSManagerImplementationBase::setCPUFrequency(int cpu_idx, int freq) {
  ostringstream path;
  path << "/sys/devices/system/cpu/cpu" << cpu_idx
       << "/cpufreq/scaling_setspeed";

  ofstream fp(path.str().c_str());
  if(fp.is_open()) {
    fp << freq;
    fp.close();
  }
  else {
    cerr << "Unable to set frequency: " << path.str()
         << " does not exist" << endl;
    abort();
  }
}

struct Utilization {
  int node;
  double util;
  bool operator()(const Utilization& a, const Utilization& b) const {
      return a.util < b.util;
  }
};

bool
DVFSManagerImplementationBase::updateFrequencyIdxs() {
  float dist;

  vector<Utilization> utils(0);
  double avg = 0.;
  int i = 0;
  int n = myUtilFilters.size();
  for(; i < n; i++) {
    Utilization u;
    u.node = i;
    u.util = myUtilFilters[i].getData();
    avg += u.util;
    utils.push_back(u);
  }
  avg /= n;

  if(myUWM == ROLLBACKS) {
    dist = avg * .1;
    sort(utils.rbegin(), utils.rend(), Utilization());
  }
  else {
    dist = 0.1;
    sort(utils.begin(), utils.end(), Utilization());
  }

  i = 0;
  int high = n;
  int low = -1;
  for(; i < n; i++) {
    if(myUWM == ROLLBACKS) {
      if(utils[i].util > avg + dist)
        low = i;
      else if(utils[i].util < avg - dist)
        break;
    }
    else {
      if(utils[i].util < avg - dist)
        low = i;
      else if(utils[i].util > avg + dist)
        break;
    }
  }
  high = i;

  bool changed = false;
  int l = 0;
  int h = n - 1;
  // low is the index of the highest useful work that is BELOW average
  // high is the index of the lowest useful work that is ABOVE average
  // starting with the nodes with the absolute lowest and highest useful 
  // work indexes, move towards the center, matching pairs as we go
  while(l <= low && h >= high) {

    // skip over any nodes that are already at max/min frequency
    while(l <= low && myFrequencyIdxs[utils[l].node] == myMaxFreqIdx)
      l++;
    while(h >= high && myFrequencyIdxs[utils[h].node] == 0)
      h--;

    // adjust frequency indexes depending on optimization goal
    if(myOG == PERFORMANCE) {
      if(l <= low && h >= high) {
        myFrequencyIdxs[utils[l++].node]++;
        myFrequencyIdxs[utils[h--].node]--;
        changed = true;
      }
    }
    else if(l <= low) {
      myFrequencyIdxs[utils[l++].node]++;
      if(h >= high && myOG != POWER)
        myFrequencyIdxs[utils[h--].node]--;
      changed = true;
    }
  }

  return changed;
}

void
DVFSManagerImplementationBase::fillUsefulWork(vector<double>& v) {
  v[mySimulationManagerID] =
     myUWM == ROLLBACKS ? getRollbacksForPeriod() :
     myUWM == ROLLBACK_FRACTION ? getRollbackFractionForPeriod() :
     myUWM == EFFECTIVE_UTILIZATION ? 0 : 0;
}

double
DVFSManagerImplementationBase::getRollbacksForPeriod() {
  int temp = myLastRollbacks;
  myLastRollbacks = mySimulationManager->getRollbacks();
  return static_cast<double>(myLastRollbacks - temp);
}

double
DVFSManagerImplementationBase::getRollbackFractionForPeriod() {
  int temp1 = myLastEventsRolledBack;
  int temp2 = myLastEventsExecuted;
  myLastEventsRolledBack = mySimulationManager->getNumEventsRolledBack();
  myLastEventsExecuted = mySimulationManager->getNumEventsExecuted();
  return 1 - (static_cast<double>(myLastEventsRolledBack - temp1) / 
            (myLastEventsExecuted - temp2));
}

void 
DVFSManagerImplementationBase::initializeFrequencyIdxs(int maxidx) {
  myMaxFreqIdx = maxidx;
  for(int i=0; i < myNumSimulationManagers; i++)
    myFrequencyIdxs.push_back(maxidx / 2);
}

void
DVFSManagerImplementationBase::populateAvailableFrequencies() {
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
    cerr << "Unable to get available frequencies: "
         << path.str() << " does not exist" << endl;
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

string
DVFSManagerImplementationBase::toString() {
  stringstream ss;
  ss << "Period: " << myMeasurementPeriod << ", "
     << "Fixed: " << (myIsDummy ? "True" : "False") << ", "
     << "FIRSize: " << myFIRSize << ", "
     << "UsefulWorkMetric: " << uwmStrings[myUWM] << ", "
     << "Optimize for: " << optimizationStrings[myOG];
  return ss.str();
}

ostream& operator<<(ostream& out, DVFSManager& dvfsm) {
  out << dvfsm.toString();
  return out;
}

const char* DVFSManagerImplementationBase::optimizationStrings[] = 
  {"Performance", "Power", "Hybrid"};

const char* DVFSManagerImplementationBase::uwmStrings[] = 
  {"Rollbacks", "RollbackFraction", "EffectiveUtilization"};
