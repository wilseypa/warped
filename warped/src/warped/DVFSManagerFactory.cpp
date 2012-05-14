// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "DVFSManagerFactory.h"
#include "SharedDVFSManager.h"
#include "DistributedDVFSManager.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"

using std::cout;
using std::cerr;
using std::endl;

DVFSManagerFactory::DVFSManagerFactory(){}

DVFSManagerFactory::~DVFSManagerFactory(){}

// allocate a new manager of the type given in the configuration file
Configurable *
DVFSManagerFactory::allocate( SimulationConfiguration &configuration,
				Configurable *parent ) const {

  TimeWarpSimulationManager *mySimulationManager = 
    dynamic_cast<TimeWarpSimulationManager *>( parent );
  ASSERT(mySimulationManager);

  string type = "NONE";
  configuration.getDVFSStringOption("Type", type);
  if(type == "NONE") {
    return NULL;
  }

  string metric = "ROLLBACKS";
  string al = "FIXED";
  int p = 0;
  int firsize = 16;
  double threshold = 0.1;
  configuration.getDVFSIntOption("Period", p);
  configuration.getDVFSIntOption("FIRSize", firsize);
  configuration.getDVFSDoubleOption("Threshold", threshold);
  configuration.getDVFSStringOption("UsefulWorkMetric", metric);
  configuration.getDVFSStringOption("Algorithm", al);

  DVFSManager::UsefulWorkMetric uwm;
  if(metric == "ROLLBACKS")
    uwm = DVFSManager::ROLLBACKS;
  else if(metric == "ROLLBACKFRACTION")
    uwm = DVFSManager::ROLLBACK_FRACTION;
  //else if(metric == "EFFECTIVEUTILIZATION")
  //  uwm = DVFSManager::EFFECTIVE_UTILIZATION;
  else {
    stringstream err;
    err << "DVFSManager: UsefulWorkMetric " << metric << " is invalid. "
        << "Valid metrics are Rollbacks | RollbackFraction. Aborting simulation."
        << endl;
    mySimulationManager->shutdown(err.str());
  }

  DVFSManager::Algorithm alg;
  if(al == "FIXED")
    alg = DVFSManager::FIXED;
  else if(al == "PERFORMANCE")
    alg = DVFSManager::PERFORMANCE;
  else if(al == "POWER")
    alg = DVFSManager::POWER;
  else if(al == "HYBRID")
    alg = DVFSManager::HYBRID;
  else {
    stringstream err;
    err << "DVFSManager: Algorithm " << al << " is invalid. "
        << "Valid options are Fixed | Performance | Power | Hybrid. "
        << "Aborting simulation." << endl;
    mySimulationManager->shutdown(err.str());
  }

  const char* trueFalseOptions[] = {"DebugPrint"};
  int numTrueFalse = sizeof(trueFalseOptions) / sizeof(const char*);
  bool trueFalseValues[numTrueFalse];
  for(int i=0; i < numTrueFalse; i++) {
    string val = "FALSE";
    configuration.getDVFSStringOption(trueFalseOptions[i], val);
    if(val != "TRUE" && val != "FALSE") {
      stringstream err;
      err << "DVFSManager: option " << trueFalseOptions[i]
          << " is invalid. Expected True / False.  Aborting simulation."
          << endl;
      mySimulationManager->shutdown(err.str());
    }
    trueFalseValues[i] = val == "TRUE";
  }

  if(type == "SHARED" || type == "DISTRIBUTED") {
    cout << "("
         << mySimulationManager->getSimulationManagerID()
         << ") configured a " << type << " DVFS Manager" << endl
         << "Algorithm: " << al << endl
         << "Period: " << p << endl
         << "FIR Size: " << firsize << endl
         << "Useful work metric: " << metric << endl
         << "Threshold: " << threshold << endl;
         if(trueFalseValues[0])
           cout <<  "Writing trace to csv" << endl;

    if(type == "SHARED")
      return new SharedDVFSManager(mySimulationManager,
                                   p,
                                   firsize,
                                   alg,
                                   trueFalseValues[0],
                                   uwm,
                                   threshold);

    return new DistributedDVFSManager(mySimulationManager,
                                      p,
                                      firsize,
                                      alg,
                                      trueFalseValues[0],
                                      uwm,
                                      threshold);
  }
  else {
    stringstream err;
    cerr << "DVFSManager: invalid type '" << type << "'." << endl
         << "Valid types are None | Shared | Distributed. Aborting simulation.";
    mySimulationManager->shutdown(err.str());
  }
}

const DVFSManagerFactory *
DVFSManagerFactory::instance(){
  static DVFSManagerFactory *singleton = new DVFSManagerFactory();
  return singleton;
}
