
#include "Simulation.h"
#include "SequentialConfigurationManager.h"
#include "TimeWarpConfigurationManager.h"
#include "SimulationConfiguration.h"
#include "Application.h"
#include "WarpedDebug.h"
#include <utils/ConfigurationScope.h>
#include <utils/ConfigurationChoice.h>
#include <utils/ConfigurationValue.h>

#include <iostream>

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
Simulation::configure(SimulationConfiguration& configuration) {
    // Decide which Simulation manager to use based on the configuraiton

    if (configuration.simulationTypeIs("Sequential")) {
        myConfigurationManager = new SequentialConfigurationManager(myApplication);
        debug::debugout << "Configured a SequentialSimulationManager" << std::endl;
    } else if (configuration.simulationTypeIs("TimeWarp")
               || configuration.simulationTypeIs("ThreadedTimeWarp")) {
        myConfigurationManager = new TimeWarpConfigurationManager(configuration.getArguments(),
                                                                  myApplication);
        debug::debugout << "Configured a TimeWarpSimulationManager" << std::endl;
    } else {
        std::cerr << "Unknown Simulation type \"" << configuration.getSimulationType() << "\"" << std::endl;
        exit(-1);
    }

    myConfigurationManager->configure(configuration);
}

SimulationManager*
Simulation::getSimulationManager() {
    ASSERT(myConfigurationManager != 0);
    return myConfigurationManager->getSimulationManager();
}


// report an error condition depending on the severity level
void 
Simulation::reportError(const string& msg, const SEVERITY level){
  switch(level){
  case NOTE:
    std::cout << "Severity Level: NOTE" << std::endl;
    std::cout << msg << std::endl;
    break;
  case WARNING:
    std::cout << "Severity Level: WARNING" << std::endl;
    std::cout << msg << std::endl;
    break;
  case ERROR:
    std::cout << "Severity Level: ERROR" << std::endl;
    std::cout << msg << std::endl;
    exit(-1);
    break;
  case ABORT:
    std::cout << "Severity Level: ABORT" << std::endl;
    std::cout << msg << std::endl;
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
