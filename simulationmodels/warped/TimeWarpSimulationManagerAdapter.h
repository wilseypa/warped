#ifndef TIMEWARP_SIMULATION_MANAGER_ADAPTER_H
#define TIMEWARP_SIMULATION_MANAGER_ADAPTER_H

#include "TimeWarpSimulationManager.h"
#include "ObjectID.h"
#include "IntVTime.h"

class TimeWarpSimulationManagerAdapter : public TimeWarpSimulationManager {
public:
  TimeWarpSimulationManagerAdapter();
   
  virtual ~TimeWarpSimulationManagerAdapter();

  void initialize(){}

  void simulate ( const VTime &simulateUntil ){}

  void finalize(){}
   
  void registerSimulationObjects(){}
   
  vector<string> *getSimulationObjectNames(){ return 0; }

  void registerSimulationObjectProxies( const vector<string> *arrayOfObjectProxies,
					unsigned int sourceSimulationManagerID,
					unsigned int destSimulationManagerID ){}

  const Event *getEvent( SimulationObject *object ){ return 0; }
  const Event *peekEvent( SimulationObject *object ){ return 0; }

  void receiveEvent( Event *event, SimulationObject *sender,
		     SimulationObject *receiver){}

  CommunicationManager *getCommunicationManager(){ return 0; }

  SchedulingManager *getSchedulingManager(){ return 0; }

  GVTManager *getGVTManager(){ return 0; }

  StateManager *getStateManager(){ return 0; }

  TimeWarpEventSet *getEventSetManager(){ return 0; }
  OutputManager *getOutputManager(){ return 0; }

  SchedulingData *getSchedulingData(){ return 0; }

  SimulationObject *getObjectHandle( const string &object ) const { return 0; }

  OBJECT_ID &getObjectId( const string &objectName ){ 
    static ObjectID retval( 0, 0 ); 
    return retval;
  }

  bool contains( const string &object ) const { return false; }

  SimulationObject *getObjectHandle( const OBJECT_ID &objectID ) const { return 0; }

  void registerWithCommunicationManager(){}

  void receiveKernelMessage(KernelMessage *msg){}

  unsigned int getSimulationManagerID() const { return 0; }

  const VTime &getSimulationTime(){ return IntVTime::getIntVTimeZero(); }
   
  bool checkIdleStatus(){ return false; }

  bool checkSuppressMessageFlag(){ return false; }

  void setSuppressMessageFlag(MsgSuppression flag){}

  MsgSuppression getSuppressMessageFlag(){ return MsgSuppression(0); }
   
  bool checkMessageAggregationFlag(){ return false; }

  void setMessageAggregationFlag(bool flag){}

  SimulationStream *getIFStream( const string &filename,
				 SimulationObject *object){ return 0; }
   
  SimulationStream *getOFStream( const string &filename,
				 SimulationObject *object,
				 ios::openmode mode ){ return 0; }
   
  SimulationStream *getIOFStream( const string &filename,
				  SimulationObject *object ){ return 0; }

  void configure( SimulationConfiguration &configuration ){}
   

protected:
  std::unordered_map<string, SimulationObject *> *createMapOfObjects(){ return 0; }

  void fossilCollect(const VTime& fossilCollectTime){}

  void rollback( Event *event, 
		 SimulationObject *object,
		 EventMessageType type ){}
   
  void coastForward( const VTime& coastForwardFromTime,
		     const VTime& rollbackToTime,
		     SimulationObject *object ){}
   
  void displayGlobalObjectMap( std::ostream &out ){}

  void calculateSimulationTime(){}


private:
  /**
     Returns true if the simulation is complete, false otherwise.
  */
  bool simulationComplete( const VTime &simulateUntil ){ return false; }

  /**
     Talks to the physical layer to get messages.
  */
  void getMessages(){}

  /**
     Executes the local objects.  Returns true if there was an event to
     execute, false otherwise.
  */
  bool executeObjects(){ return false; }

};

#endif

