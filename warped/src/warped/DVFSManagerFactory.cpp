// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "DVFSManagerFactory.h"
#include "RealDVFSManager.h"
#include "SimulatedDVFSManager.h"
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
  string opt = "PERFORMANCE";
  int p = 0;
  int firsize = 16;
  configuration.getDVFSIntOption("Period", p);
  configuration.getDVFSIntOption("FIRSize", firsize);
  configuration.getDVFSStringOption("UsefulWorkMetric", metric);
  configuration.getDVFSStringOption("OptimizeFor", opt);

  DVFSManager::UsefulWorkMetric uwm;
  if(metric == "ROLLBACKS")
    uwm = DVFSManager::ROLLBACKS;
  else if(metric == "ROLLBACKFRACTION")
    uwm = DVFSManager::ROLLBACK_FRACTION;
  else if(metric == "EFFECTIVEUTILIZATION")
    uwm = DVFSManager::EFFECTIVE_UTILIZATION;
  else {
    stringstream err;
    err << "DVFSManager: UsefulWorkMetric " << metric << " is invalid. "
        << "Valid metrics are Rollbacks | EffectiveUtilization "
        << "| RollbackFraction. Aborting simulation." << endl;
    mySimulationManager->shutdown(err.str());
  }

  DVFSManager::OptimizationGoal og;
  if(opt == "PERFORMANCE")
    og = DVFSManager::PERFORMANCE;
  else if(opt == "POWER")
    og = DVFSManager::POWER;
  else if(opt == "HYBRID")
    og = DVFSManager::HYBRID;
  else {
    stringstream err;
    err << "DVFSManager: OptimizeFor " << opt << " is invalid. "
        << "Valid options are Performance | Power | Hybrid. "
        << "Aborting simulation." << endl;
    mySimulationManager->shutdown(err.str());
  }

  const char* trueFalseOptions[] = {"Fixed", "DebugPrint"};
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

  if( geteuid() !=  0 ) {
    stringstream err;
    err << "DVFSManager: To use DVFS, WARPED "
        << " must be run as root. Aborting simulation." << endl;
    mySimulationManager->shutdown(err.str());
  }

  if(type == "REAL" || type == "SIMULATED") {
    cout << "("
         << mySimulationManager->getSimulationManagerID()
         << ") configured a " << type << " DVFS Manager" << endl
         << "Fixed: " << (trueFalseValues[0] ? "True" : "False") << endl
         << "Period: " << p << endl
         << "FIR Size: " << firsize << endl
         << "Optimize for: " << opt << endl
         << "Useful work metric: " << metric << endl;
         if(trueFalseValues[1])
           cout <<  "Writing trace to csv" << endl;

    if(type == "REAL")
      return new RealDVFSManager(mySimulationManager,
                                 p,
                                 firsize,
                                 trueFalseValues[0],
                                 trueFalseValues[1],
                                 og,
                                 uwm);

    return new SimulatedDVFSManager(mySimulationManager,
                                    p,
                                    firsize,
                                    trueFalseValues[0],
                                    trueFalseValues[1],
                                    og,
                                    uwm);
  }
  else {
    stringstream err;
    cerr << "DVFSManager: invalid type '" << type << "'." << endl
         << "Valid types are None | Real | Simulated. Aborting simulation.";
    mySimulationManager->shutdown(err.str());
  }
}

const DVFSManagerFactory *
DVFSManagerFactory::instance(){
  static DVFSManagerFactory *singleton = new DVFSManagerFactory();
  return singleton;
}
