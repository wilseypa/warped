#ifndef COMMUNICATION_MANAGER_IMPLEMENTATION_BASE_H
#define COMMUNICATION_MANAGER_IMPLEMENTATION_BASE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "CommunicationManager.h"
class TimeWarpSimulationManager;

/** The CommunicationManagerImplementationBase class.

    This is the default implementation base class for different types
    of communication managers. There are no optimizations built into
    this manager.  This class contains methods and data common to the
    different communication manager implementations.

*/
class CommunicationManagerImplementationBase : public CommunicationManager {

public:
  /**@name Public Class Methods of CommunicationManagerImplementationBase. */
  //@{
  
  /** Constructor.
      @param physicalLayer Handle to the PhysicalLayer.
      @param simMgr Handle to the simulation manager.
  */
  CommunicationManagerImplementationBase( PhysicalCommunicationLayer *physicalLayer, 
					  TimeWarpSimulationManager *simMgr);

  /// Destructor.
  virtual ~CommunicationManagerImplementationBase(){};

  /// initialize the communication manager
  virtual void initializeCommunicationManager();
   
  /** Send message.
       
  @param msg Message to send.
  */
  virtual void sendMessage( KernelMessage *msg, unsigned int dest );

  /** Route message to the correct receiver.

  @param msg Message to route.
  */
  virtual void routeMessage(KernelMessage *msg);
   
  /** Retrieve a message from the physical comm. layer.
       
  @return Retrieved message.
  */
  virtual SerializedInstance *retrieveMessageFromPhysicalLayer();
   
  /** Poll to see if any messages have arrived.
       
  @param int Max number of messages to receive (default = 1).
  @return Number of received messages.
  */
  virtual unsigned int checkPhysicalLayerForMessages( int = 1 );
   
  /** Wait for init messages to set up the distributed simulation.
       
  Assume N simulation objects. At initialization, we will 
  wait for numExpected = N-1 initialization messages.
       
  @param numExpected Number of expected init messages.       
  */
  virtual void waitForInitialization(unsigned int numExpected);
   
  /** send start messages to start the distributed simulation
       
  Assume N simulation objects. Simulation Manager 0 sends start
  messages to N - 1 simulation manager.
       
  @param myID Id of the sending simulation manager.
  */
  virtual void sendStartMessage(unsigned int myID);
   
  /** wait for start message
       
  The starting simulation manager (ID = 0) sends simulation start
  messages and only after the receipt of these messages should
  the simulation manager start simulation activity.
       
  */
  virtual void waitForStart();

  /** Get the message type for a InitializationMessage */
  static const string &getInitializationMessageType();

  /** Get the message type for a CirculateInitializationMessage */
  static const string &getCirculateInitializationMessageType();

  /** Get the message type for a CheckIdleMessage */
  static const string &getCheckIdleMessageType();

  //@} // End of Public Class Methods of CommunicationManagerImplBase

protected:
  /// a handle to the simulation manager
  TimeWarpSimulationManager *const mySimulationManager;

private:
   //@} // End of Private Class Attributes of CommunicationManagerImplBase

  /**@name Private Class Methods of CommunicationManagerImplementationBase. */
  //@{
   
  //@} // End of Private Class Methods of CommunicationManagerImplBase
   
};

#endif
