// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Debug.h>
#include "ClockFrequencyManagerFactory.h"
#include "CentralizedClockFrequencyManager.h"
#include "DecentralizedClockFrequencyManager.h"
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
  int firsize = 16;
  configuration.getClockFreqManagerType(type);
  configuration.getClockFreqManagerPeriod(p);
  configuration.getClockFreqManagerFIRSize(firsize);
  bool dummy = configuration.getClockFreqManagerDummy();

  if(type == "NONE") {
    return NULL;
  }

  if( geteuid() !=  0 ) {
      cout << "ClockFrequencyManager: To use clock frequency modulation, WARPED " 
        << " must be run as root" << endl;
    return NULL;
  }

  if(type == "CENTRALIZED") {
    cout << "("
        << mySimulationManager->getSimulationManagerID()
        << ") configured a Centralized Clock Frequency Manager with a period = "
        << p << " and FIR size of " << firsize << std::endl;

    return new CentralizedClockFrequencyManager(mySimulationManager, p, firsize, dummy);
  }
  else if(type == "DECENTRALIZED") {

    cout << "("
        << mySimulationManager->getSimulationManagerID()
        << ") configured a Decentralized Clock Frequency Manager with period = "
        << p << " and FIR size of " << firsize << std::endl;

    return new DecentralizedClockFrequencyManager(mySimulationManager, p, firsize, dummy);
  }
  else {
    std::ostringstream err;
    cerr << "ClockFrequencyManager: invalid type '" << type << "'." << endl
        << "Valid types are: None, Centralized, Decentralized";
    mySimulationManager->shutdown(err.str());
  }
  return NULL;
}

const ClockFrequencyManagerFactory *
ClockFrequencyManagerFactory::instance(){
  static ClockFrequencyManagerFactory *singleton = new ClockFrequencyManagerFactory();
  return singleton;
}
