
#include "TimeWarpSimMgrWithVisualization.h"
#include "DefaultVisualizationManager.h"
#include "CommunicationManager.h"
#include "SchedulingManager.h"
#include "Event.h"
#include <StopWatch.h>
#include <sstream>

using std::fstream;
using std::istringstream;
using std::ostringstream;
using std::endl;

TimeWarpSimMgrWithVisualization::TimeWarpSimMgrWithVisualization( unsigned int numProcessorsRequested,
								  Application *initApplication ) :
  TimeWarpSimulationManager( numProcessorsRequested, initApplication ) {
  //  myVisualizationManager = new DefaultVisualizationManager( this, configurationMap );
}

TimeWarpSimMgrWithVisualization::~TimeWarpSimMgrWithVisualization(){
}

void
TimeWarpSimMgrWithVisualization::registerSimulationObjects(){
  // dont forget to call the base class registerSimulationObjects() ...
  TimeWarpSimulationManager::registerSimulationObjects();
  
  // Okay, the base class is done with its job. Now, let's walk the list
  // of local objects in this SimulationManager and pass on the information
  // to the visualization server.
  
  vector<SimulationObject *> *objects = getElementVector(localArrayOfSimObjPtrs);
  for( unsigned int i = 0; i < objects->size(); i++ ){
     ostringstream tempStream;
     SimulationObject *object = (*objects)[i];
     
     tempStream << "LP#" << getSimulationManagerID() << ", has object id="
		<< object->getObjectID();
     
     string data = tempStream.str();
     
     myVisualizationManager->receiveData(const_cast<char *>(data.c_str()));
     
     tempStream.seekp(0);
   }
  delete objects;
}

void
TimeWarpSimMgrWithVisualization::simulate(const VTime& simulateUntil){

  abort();
   
   StopWatch stopwatch;
   stopwatch.start();

   cerr << "SimulationManager(" << getSimulationManagerID() << "): Initializing Objects" << endl;

   initialize();

   int terminateCycleCounter = 0;

   if(numberOfSimulationManagers > 1){
     if(getSimulationManagerID() == 0){
	// send out simulation start messages
         getCommunicationManager()->sendStartMessage(getSimulationManagerID());
      }
      else {
         // else wait for the start message
         getCommunicationManager()->waitForStart();
      }
   }
   
   cerr << "SimulationManager(" << getSimulationManagerID()
        << "): Starting simulation - End time: " << simulateUntil << ")\n";

   while( getGVTManager()->getGVT() < simulateUntil && !simulationComplete() ){
     if(numberOfSimulationManagers > 1){
       getCommunicationManager()->checkPhysicalLayerForMessages(1000);
     }

      ASSERT(getSchedulingManager() != NULL);
      const Event *nextEvent = getSchedulingManager()->peekNextEvent();

      if( nextEvent != NULL ){
	SimulationObject *object = getObjectHandle( nextEvent->getReceiver() );

         ASSERT(object != NULL);
	 setSimulationTime( nextEvent->getReceiveTime() );
         // schedule a process for execution
	 object->executeProcess();
      
         // lets save this state
         getStateManager()->saveState( getSimulationTime(), object );

         terminateCycleCounter = 0;
         if(getGVTManager()->checkGVTPeriod()){
            getGVTManager()->calculateGVT();
            fossilCollect(getGVTManager()->getGVT());
         }

      }
      else {
         // check if the simulation is complete
         if(++terminateCycleCounter > 5000){
//             simulationFinished =
//                getCommunicationManager()->isSimulationComplete();
            terminateCycleCounter = 0;
         }
      }

   }
   
   finalize();

   stopwatch.stop();

   cerr << "Simulation complete ("
        << stopwatch.elapsed() << " secs)" << endl;
}

const Event *
TimeWarpSimMgrWithVisualization::getEvent(SimulationObject *object){
   ASSERT(getEventSetManager() != NULL);
   const Event *retval;
   int id = object->getObjectID()->getSimulationObjectID();

   if ( getMessageSuppression(id) == COASTFORWARD) {
     retval = getEventSetManager()->getEvent( object, getCoastForwardTime() );
   }
   else {
     retval = getEventSetManager()->getEvent(object);
   }

   return retval;
}

void
TimeWarpSimMgrWithVisualization::receiveKernelMessage(KernelMessage *msg){
   // dont forget to call the base class receiveKernelMessage() ...
   TimeWarpSimulationManager::receiveKernelMessage(msg);
}
