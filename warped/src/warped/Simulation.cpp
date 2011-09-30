// See copyright notice in file Copyright in the root directory of this archive.

#include "Simulation.h"
#include "SimulationConfiguration.h"
#include "ConfigurationManagerFactory.h"
#include "Application.h"
#include <utils/ConfigurationScope.h>
#include <utils/ConfigurationChoice.h>
#include <utils/ConfigurationValue.h>
using std::cout;

Simulation::Simulation( Application *initApplication ) : myApplication( initApplication ){
}

Simulation*
Simulation::instance( SimulationConfiguration *configuration,
		      Application *userApplication ){
  // global access point to this singleton class
  static Simulation *singleton = 0;
  if( singleton == 0 ){
    singleton = new Simulation( userApplication );
    if( configuration == 0 ){
      configuration = getDefaultConfiguration();
    }
    singleton->configure( *configuration );
  }
  return singleton;
}

void 
Simulation::configure( SimulationConfiguration &configuration ){
  const ConfigurationManagerFactory *cfgMgrFactory = ConfigurationManagerFactory::instance();

  myConfigurationManager = 
    dynamic_cast<ConfigurationManager *>( cfgMgrFactory->allocate( configuration,
								   myApplication));
  ASSERT( myConfigurationManager != 0 );
  myConfigurationManager->configure( configuration );
}

SimulationManager *
Simulation::getSimulationManager(){
  ASSERT( myConfigurationManager != 0 );
  return myConfigurationManager->getSimulationManager();
}


// report an error condition depending on the severity level
void 
Simulation::reportError(const string& msg, const SEVERITY level){
  switch(level){
  case NOTE:
    cout << "Severity Level: NOTE" << endl;
    cout << msg << endl;
    break;
  case WARNING:
    cout << "Severity Level: WARNING" << endl;
    cout << msg << endl;
    break;
  case ERROR:
    cout << "Severity Level: ERROR" << endl;
    cout << msg << endl;
    exit(-1);
    break;
  case ABORT:
    cout << "Severity Level: ABORT" << endl;
    cout << msg << endl;
    abort();
    break;
  default:
    break;
  };
}

SimulationConfiguration *
Simulation::getDefaultConfiguration(){
  ConfigurationScope *outerScope = new ConfigurationScope( "" );
  vector<string> emptyVector;
  SimulationConfiguration *retval = new SimulationConfiguration( outerScope, emptyVector );

  // Make the default a sequential simulation
  ConfigurationChoice *simulationChoice = new ConfigurationChoice( "Simulation" );
  simulationChoice->setConfigurationValue( new StringConfigurationValue("Sequential") );
  outerScope->addChoice( simulationChoice );

  // Make the default event list a splay tree.
  ConfigurationScope *eventListScope = new ConfigurationScope( "EventList" );
  ConfigurationChoice *eventListChoice = new ConfigurationChoice( "Type" );
  eventListChoice->setConfigurationValue( new StringConfigurationValue("SplayTree") );
  eventListScope->addChoice( eventListChoice );
  outerScope->addScope( eventListScope );
  
  return retval;
}

void
Simulation::initialize(){
  getSimulationManager()->initialize();
}

void
Simulation::simulate( const VTime &simulateUntil ){ 
  getSimulationManager()->simulate( simulateUntil );
}

void
Simulation::finalize(){
  getSimulationManager()->finalize();
  cleanUp();
}

const VTime &
Simulation::getCommittedTime(){
  return getSimulationManager()->getCommittedTime();
}

const VTime &
Simulation::getNextEventTime(){
  return getSimulationManager()->getNextEventTime();
}

bool
Simulation::simulationComplete(){
  return getSimulationManager()->simulationComplete();
}

extern "C" {
  /**
     Simply here to have something to search for from configure scripts.
  */
  char isWarped(){ return 0; }
}
