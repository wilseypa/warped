// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "ClockFrequencyManagerFactory.h"
#include "CentralizedClockFrequencyManager.h"
#include "DistributedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"

using std::cout;
using std::cerr;
using std::endl;

ClockFrequencyManagerFactory::ClockFrequencyManagerFactory(){}

ClockFrequencyManagerFactory::~ClockFrequencyManagerFactory(){}

// allocate a new manager of the type given in the configuration file
Configurable *
ClockFrequencyManagerFactory::allocate( SimulationConfiguration &configuration,
				Configurable *parent ) const {

  TimeWarpSimulationManager *mySimulationManager = 
    dynamic_cast<TimeWarpSimulationManager *>( parent );
  ASSERT(mySimulationManager);

  string type;
  int p = 0;
  int confCPUs = 0;
  int firsize = 16;
  configuration.getClockFreqManagerType(type);
  configuration.getClockFreqManagerPeriod(p);
  configuration.getClockFreqManagerCPUs(confCPUs);
  configuration.getClockFreqManagerFIRSize(firsize);
  bool dummy = configuration.getClockFreqManagerDummy();

  if(type == "NONE") {
    return NULL;
  }
  else if(confCPUs <= 1) {
    cout << "ClockFrequencyManager: To use clock frequency modulation, you must "
        << "specify 'NumCPUs: n' s.t. n > 1." << endl
        << "Disabling ClockFrequencyManager" << endl;
    return NULL;
  }

  if( geteuid() !=  0 ) {
      cout << "ClockFrequencyManager: To use clock frequency modulation, WARPED " 
        << " must be run as root" << endl;
    return NULL;
  }

  int systemCPUs = CPUCount();
  if(confCPUs > systemCPUs) {
    std::ostringstream err;
    err << "ClockFrequencyManager: " << confCPUs << " CPUs specified in configuration, "
        << "but only " << systemCPUs << " CPU(s) found in /proc/cpuinfo!" << endl
        << "shutting down...";
    mySimulationManager->shutdown(err.str());
  }

  if(type == "CENTRALIZED") {
    cout << "("
        << mySimulationManager->getSimulationManagerID()
        << ") configured a Centralized Clock Frequency Manager with a period = "
        << p << " and FIR size of " << firsize << " using " << confCPUs << " CPUs" << std::endl;

    return new CentralizedClockFrequencyManager(mySimulationManager, p, confCPUs, firsize, dummy);
  }
  else if(type == "DISTRIBUTED") {

    cout << "("
        << mySimulationManager->getSimulationManagerID()
        << ") configured a Distributed Clock Frequency Manager with period = "
        << p << " and FIR size of " << firsize << " using " << confCPUs << " CPUs" << std::endl;

    return new DistributedClockFrequencyManager(mySimulationManager, p, confCPUs, firsize, dummy);
  }
  else {
    std::ostringstream err;
    cerr << "ClockFrequencyManager: invalid type '" << type << "'." << endl
        << "Valid types are: None, Centralized, Distributed";
    mySimulationManager->shutdown(err.str());
  }
  return NULL;
}

const ClockFrequencyManagerFactory *
ClockFrequencyManagerFactory::instance(){
  static ClockFrequencyManagerFactory *singleton = new ClockFrequencyManagerFactory();
  return singleton;
}

int ClockFrequencyManagerFactory::CPUCount() {
    FILE *fp;
    char res[128];
    memset ( res, 0, sizeof ( res ) );
    fp = popen ( "/bin/cat /proc/cpuinfo | grep -c '^processor'", "r" );
    fread ( res, 1, sizeof ( res ) - 1, fp );
    fclose ( fp );

    for ( int i = 0; i < 128; ++i ) {
        if ( res[i] < '0' || res[i] > '9' ) {
            res[i] = ( char ) 0;
            break;
        }
    }

    return atoi ( res );
}
