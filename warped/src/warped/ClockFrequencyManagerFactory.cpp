// See copyright notice in file Copyright in the root directory of this archive.

#include "ClockFrequencyManagerFactory.h"
#include "CentralizedClockFrequencyManager.h"
#include "DistributedClockFrequencyManager.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include <utils/Debug.h>

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
  configuration.getClockFreqManagerType(type);
  configuration.getClockFreqManagerPeriod(p);
  configuration.getClockFreqManagerCPUs(confCPUs);

  if(type == "NONE") {
    return NULL;
  }
  else if(confCPUs <= 1) {
    utils::debug << "ClockFrequencyManager: To use clock frequency modulation, you must "
        << "specify 'NumCPUs: n' s.t. n > 1." << endl
        << "Disabling ClockFrequencyManager" << endl;
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
    utils::debug << "("
        << mySimulationManager->getSimulationManagerID()
        << ") configured a Centralized Clock Frequency Manager with period = "
        << p << " using " << confCPUs << " CPUs" << std::endl;

    return new CentralizedClockFrequencyManager(mySimulationManager, p, confCPUs);
  }
  else if(type == "DISTRIBUTED") {

    utils::debug << "("
        << mySimulationManager->getSimulationManagerID()
        << ") configured a Distributed Clock Frequency Manager with period = "
        << p << " using " << confCPUs << " CPUs" << std::endl;

    return new DistributedClockFrequencyManager(mySimulationManager, p, confCPUs);
  }
  else {
    std::ostringstream err;
    err << "ClockFrequencyManager: invalid type '" << type << "'." << endl
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
