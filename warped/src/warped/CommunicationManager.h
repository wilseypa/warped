#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "CommunicatingEntity.h"
#include "eclmpl/PhysicalCommunicationLayer.h"
#include "Configurable.h"

#include<tr1/unordered_map>
using std::tr1::unordered_map;

class KernelMessage;
class SimulationObject;
class TimeWarpSimulationManager;

/** The CommunicationManager abstract base class.

 This is the abstract base class for all communication manager
 implementations in the kernel. Different communication manager
 implementations MUST be derived from this abstract base class.

 To accomodate communication related optimizations in the the
 simulation kernel, we define the CommunicationManager abstract
 interface class.  This class provides the basic communication
 facilities and makes certain functions pure virtual so that
 different communication managers may optimize these functions.
 Currently, two communication manager implementations are
 supported: (a) the default communication manager (without any
 optimizations); and (b) the message aggregating communication
 manager that aggregates messages before sending them across the
 network.

 */
class CommunicationManager: virtual public Configurable {
public:

	/**@name Public Class Methods of CommunicationManager. */
	//@{

	/** Default constructor.

	 @param physicalLayer A handle to the physical comm layer.
	 */
	CommunicationManager(PhysicalCommunicationLayer *physicalLayer,
			TimeWarpSimulationManager *initSimulationManager);

	/// Virtual destructor
	virtual ~CommunicationManager();

	/// initialize the communication Manager
	virtual void initializeCommunicationManager() = 0;

	/** Method to request the sending of a KernelMessage to another
	 SimulationObject.  This is generally housekeeping messages like GVT
	 updates, or wrapped events.

	 This is a pure virtual function and has to be overridden by the user.

	 @param msg Message to send.
	 @pararm dest Destination SimulationManager.

	 Note that the msg will be deleted by the kernel when it's delivered,
	 so no handle should be retained by the caller.
	 */
	virtual void sendMessage(KernelMessage *msg, unsigned int dest) = 0;

	/** Route this message to the right guy.

	 This is a pure virtual function and has to be overridden by the user.

	 @param msg Message to be routed.
	 */
	virtual void routeMessage(KernelMessage *msg) = 0;

	/** Grab a message from the network layer.

	 This is a pure virtual function and has to be overridden by the user.

	 @return Retrieved message.
	 */
	virtual const SerializedInstance *retrieveMessageFromPhysicalLayer() = 0;

	/** Poll to see if any messages have arrived.

	 This is a pure virtual function and has to be overridden by the user.

	 @param int Max number of messages to receive (default = 1).
	 @return Number of received messages.
	 */
	virtual unsigned int checkPhysicalLayerForMessages(int = 1) = 0;

	void initializePhysicalCommunicationLayer(
			SimulationConfiguration &configuration) {
		myPhysicalCommunicationLayer->physicalInit(configuration);
	}

	/** Get the id of this communication manager.

	 @return Id of communication manager.
	 */
	unsigned int getId() {
		return myPhysicalCommunicationLayer->physicalGetId();
	}

	/** Register all message types the comm manager needs to know about.

	 @param messageType Type of message.
	 @param entity Entity associated with the message.
	 */
	void registerMessageType(const string &messageType,
			CommunicatingEntity *entity);

	/** Wait for init messages to set up the distributed simulation.

	 Assume N simulation objects. At initialization, we will
	 wait for numExpected = N-1 initialization messages.

	 This is a pure virtual function and has to be overridden by the user.

	 @param numExpected Number of expected init messages.
	 */
	virtual void waitForInitialization(unsigned int numExpected) = 0;

	/** send start messages to start the distributed simulation

	 Assume N simulation objects. Simulation Manager 0 sends start
	 messages to N - 1 simulation manager.

	 @param myID Id of the sending simulation manager.
	 @param numManagers Number of simulation managers.
	 */
	virtual void sendStartMessage(unsigned int myID) = 0;

	/** wait for start message

	 The starting simulation manager (ID = 0) sends simulation start
	 messages and only after the receipt of these messages should
	 the simulation manager start simulation activity.

	 */
	virtual void waitForStart() = 0;

	/// Set true when recovering from a catastrophic rollback during
	/// optimimistic fossil collection.
	void setRecoveringFromCheckpoint(bool inRec) {
		recoveringFromCkpt = inRec;
	}

	// Returns true when the Simulation is in recovery from a catastrophic rollback
	bool getRecoveringFromCheckpoint() {
		return recoveringFromCkpt;
	}

	/// Increments the number of recoveries from catastrophic rollbacks.
	void incrementNumRecoveries() {
		numCatastrophicRollbacks++;
	}

	virtual void finalize();

	void configure(SimulationConfiguration &configuration);

	//@} // End of Public Class Methods of CommunicationManager

protected:

	/**@name Protected Class Attributes of CommunicationManager. */
	//@{
	/// A list of receivers.

	//Define a type typeCommMap with a string key, containing communicatingEntities
	typedef unordered_map<string, CommunicatingEntity *> typeCommMap;
	typeCommMap listOfReceivers;

	/// Handle to physical communication layer.
	PhysicalCommunicationLayer * const myPhysicalCommunicationLayer;

	/// True when recovering from a catastrophic rollback during
	/// optimimistic fossil collection.
	bool recoveringFromCkpt;

	/// The number of catastrophic rollbacks that have occurred when
	/// using optimistic fossil collection.
	unsigned int numCatastrophicRollbacks;

	//@} // End of Protected Class Attributes of CommunicationManager.

};

#endif
