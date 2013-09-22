#ifndef CONTROL_MANAGER_H
#define CONTROL_MANAGER_H


#include <fstream>
#include "warped.h"
#include "controlkit/Sensor.h"
#include "controlkit/Actuator.h"
#include "SimulationManager.h"
#include "TimeWarpSimulationManager.h"
#include "CommunicatingEntity.h"
#include "Configurable.h"

/** The ControlManager class.

    The ControlManager class represents the central component of the
    monitoring and instrumentation framework built around the
    simulation manager. This class contains three subcomponents that
    jointly perform the monitoring and control tasks: a sensor
    database (indexed by the unique name of the sensor), an actuator
    database (indexed by the unique name of the actuator), and the
    control logic that manipulates the actuators to steer the
    simulation.

    In addition, the ControlManager presents two interfaces to
    entities that interact with it; the first interface is with the
    simulation manager. The simulation manager sees an interface
    within the ControlManager that is strictly used for monitoring and
    controlling aspects of the simulation kernel. The second interface
    is for the visualization framework. The visualization framework
    can query the control manager for information.
    
*/
class ControlManager : virtual public CommunicatingEntity, 
		       virtual public Configurable {
public:

  /**@name Public Class Methods of ControlManager */
  //@{

  /// Default constructor.
  ControlManager(TimeWarpSimulationManager *simMgr);

  /// Default Destructor.
  ~ControlManager();
   
  /// register sensor with the control manager
  void registerSensor(string &name, Sensor *sensor);

  /// register actuator with the control manager
  void registerActuator(string &name, Actuator *actuator);

  /// Register a particular event type with the communication manager.
  void registerWithCommunicationManager();

  /// Method the communication mgr will call to deliver a message.
  void receiveKernelMessage(KernelMessage *msg);

  /// initiate the control action (if any)
  void initiateControlAction();

  /// configure this control manager
  void configure( SimulationConfiguration &configuration );
   
  //@} // End of Public Class Methods of ControlManager

private:

  /**@name Private Class Attributes of ControlManager */
  //@{

  /// the set of sensors in the system
  std::unordered_map<string, Sensor *> sensorDataBase;

  /// the set of actuators in the system
  std::unordered_map<string, Actuator *> actuatorDataBase;
  
  /// handle to the local simulation manager
  TimeWarpSimulationManager *mySimulationManager;

  /// the file stream to which sensor information is dumped
  std::ostream *infoStream;
   
  //@} // End of Private Class Attributes of ControlManager

   
  /**@name Private Class Methods of ControlManager */
  //@{

  /// initiate the parameter adjustment phase
  void initiateParameterAdjustment();
   
  //@} // End of Public Class Methods of ControlManager

};

#endif
