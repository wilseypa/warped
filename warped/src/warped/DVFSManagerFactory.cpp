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

  string type;
  configuration.getDVFSStringOption("TYPE", type);
  if(type == "NONE") {
    return NULL;
  }

  string metric = "ROLLBACKS";
  int p = 0;
  int firsize = 16;
  configuration.getDVFSIntOption("PERIOD", p);
  configuration.getDVFSIntOption("FIRSIZE", firsize);
  configuration.getDVFSStringOption("USEFULWORKMETRIC", metric);

  UsefulWorkMetric uwm;
  uwm = metric == "ROLLBACKS" ?             UWM_ROLLBACKS :
        metric == "EFFECTIVE_UTILIZATION" ? UWM_EFFECTIVE_UTILIZATION :
        metric == "EFFICIENCY" ?            UWM_EFFICIENCY :
                                            UWM_NONE;
  if(uwm == UWM_NONE) {
    stringstream err;
    err << "DVFSManager: UsefulWorkMetric " << metric << " is invalid. "
        << "Valid metrics are Rollbacks | EffectiveUtilization "
        << "| Efficiency. Aborting simulation." << endl;
    mySimulationManager->shutdown(err.str());
  }

  const char* trueFalseOptions[] = {"DUMMY", "POWERSAVE"};
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
         << ") configured a " << type << " DVFS Manager, "
         << "Dummy: " << (trueFalseValues[0] ? "True" : "False") << "; "
         << "Period: " << p << "; "
         << "FIR Size: " << firsize << "; "
         << "Power Save: " << (trueFalseValues[1] ? "True" : "False")
         << endl;

    if(type == "REAL")
      return new RealDVFSManager(mySimulationManager,
                                 p,
                                 firsize,
                                 trueFalseValues[0],
                                 trueFalseValues[1],
                                 uwm);

    return new SimulatedDVFSManager(mySimulationManager,
                                    p,
                                    firsize,
                                    trueFalseValues[0],
                                    trueFalseValues[1],
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
