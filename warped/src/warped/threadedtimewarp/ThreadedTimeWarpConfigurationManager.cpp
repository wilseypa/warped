// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpConfigurationManager.h"
#include "TimeWarpConfigurationManager.h"
#include "ThreadedTimeWarpSimulationManagerFactory.h"
#include "SchedulingData.h"
#include "Application.h"
#include "SimulationConfiguration.h"
#include "Spinner.h"

using std::cout;
using std::endl;

ThreadedTimeWarpConfigurationManager::ThreadedTimeWarpConfigurationManager( const vector<string> &arguments,
							    Application *initApplication )
	: TimeWarpConfigurationManager(arguments, initApplication) {}

ThreadedTimeWarpConfigurationManager::~ThreadedTimeWarpConfigurationManager()
{}

void
ThreadedTimeWarpConfigurationManager::configure( SimulationConfiguration &configuration ){
  cout << "Initializing threaded simulation...\n";

  const ThreadedTimeWarpSimulationManagerFactory *twSimMgrFactory =
		  ThreadedTimeWarpSimulationManagerFactory::instance();

  mySimulationManager = 
    dynamic_cast<ThreadedTimeWarpSimulationManager *>(twSimMgrFactory->allocate( configuration,
									 myApplication ) );
  mySimulationManager->configure( configuration );
  
  Spinner::spinIfRequested( "SpinBeforeSimulationStart",  configuration );
}
