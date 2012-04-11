// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationObjectQueueFactory.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSimulationObjectQueue.h"
#include "SimulationConfiguration.h"
#include "SchedulingData.h"
using std::cerr;
using std::endl;

Configurable *
TimeWarpSimulationObjectQueueFactory::allocate( SimulationConfiguration &configuration,
				   Configurable *parent ) const {
  TimeWarpSimulationObjectQueue *retval = 0;
/*
#ifdef USE_TIMEWARP
  TimeWarpSimulationManager *mySimulationManager =
    dynamic_cast<TimeWarpSimulationManager *>(parent);

  ASSERT( mySimulationManager != 0 );

	if( configuration.objectQueueTypeIs( "CalendarQueue" ) ){
		//The default is 5 buckets, width=1 if nothing is specified
		unsigned int numberOfBuckets = 5;
		unsigned int bucketWidth = 1;
		//These functions return false if numberOfBuckets/bucketWidth was not changed
		configuration.getObjectQueueNumberOfBuckets(numberOfBuckets);
		configuration.getObjectQueueBucketWidth(bucketWidth);
		//std::cout << "Buckets: " << numberOfBuckets << std::endl;
		//std::cout << "Width: " << bucketWidth << std::endl;
		retval = new TimeWarpSimulationObjectCalendarQueue( mySimulationManager, numberOfBuckets, bucketWidth);
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
		<< ") configured a Calendar Queue as the object queue" << endl;
	}
	else if( configuration.objectQueueTypeIs( "LockedQueue" ) ){
		retval = new TimeWarpSimulationObjectLockedQueue( mySimulationManager );
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured a locked object queue" << endl;
	}
	else{
		retval = new TimeWarpSimulationObjectLockedQueue( mySimulationManager );
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured a default locked object queue" << endl;
	}

#endif
*/

  return retval;
}

const TimeWarpSimulationObjectQueueFactory *
TimeWarpSimulationObjectQueueFactory::instance(){
  static const TimeWarpSimulationObjectQueueFactory *retval = new TimeWarpSimulationObjectQueueFactory();
  return retval;
}
