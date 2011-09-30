// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpSimulationManagerFactory.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"

using std::cerr;
using std::endl;

ThreadedTimeWarpSimulationManagerFactory::ThreadedTimeWarpSimulationManagerFactory(){}

ThreadedTimeWarpSimulationManagerFactory::~ThreadedTimeWarpSimulationManagerFactory(){
  // myTimeWarpSimulationManager will be deleted by the end user - the
  // ThreadedTimeWarpSimulationManager
}

Configurable *
ThreadedTimeWarpSimulationManagerFactory::allocate( SimulationConfiguration &configuration,
					    Configurable *parent ) const {
  ThreadedTimeWarpSimulationManager *retval = 0;

  //Count the number of simulation managers
  unsigned int numberOfSimulationManagers = 0;
  if( !configuration.getNumberOfSimulationManagers( numberOfSimulationManagers ) ){
    cerr << "Could not find number of simulation managers!" << endl;
    abort();
  }

  //Count the number of threads, if none specified try reading the proc file
  unsigned int numberOfWorkerThreads = 0;
  if( !configuration.getWorkerThreadCount( numberOfWorkerThreads ) ){
	  numberOfWorkerThreads = GetProcCount();
  }

  retval = new ThreadedTimeWarpSimulationManager(numberOfSimulationManagers,
												  numberOfWorkerThreads,
												  (Application *)parent );
  return retval;
}

const ThreadedTimeWarpSimulationManagerFactory *
ThreadedTimeWarpSimulationManagerFactory::instance(){
  static ThreadedTimeWarpSimulationManagerFactory *singleton = new ThreadedTimeWarpSimulationManagerFactory();

  return singleton;
}

unsigned int ThreadedTimeWarpSimulationManagerFactory::GetProcCount() const
{
	fstream cpuFile("/proc/cpuinfo", ios::in);
	//If /proc/cpuinfo exists then
	//Count each processor - 1 as available
	//The -1 is so the simulationManager gets its own processor
	//Else return just 1 available processor
	int numberAvailableProcessors=0;
	if (cpuFile.is_open())
	{
		string line = "";
		while (getline(cpuFile,line))
		{
			if (line.substr(0,9)=="processor")
			{
				numberAvailableProcessors++;
			}
		}
		//Subtract the simulationManager's processor from the available list
		//If their is at least 1 extra
		if (numberAvailableProcessors>=1)
		{
			numberAvailableProcessors--;
		}
		cpuFile.close();
	}
	return numberAvailableProcessors;
}
