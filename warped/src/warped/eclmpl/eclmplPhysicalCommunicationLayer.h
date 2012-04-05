#ifndef ECLMPL_PHYSICAL_COMMUNICATION_LAYER_H
#define ECLMPL_PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmplCommonInclude.h"
#include "NetworkMessage.h"
#include "PhysicalCommunicationLayer.h"
#include "eclmplConnectionInterface.h"

using std::deque;

/** The eclmplPhysicalCommunicationLayer class.

*/
class eclmplPhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
  /**@name Public Class Methods of eclmplPhysicalCommunicationLayer. */
  //@{

  /// Default Constructor.
  eclmplPhysicalCommunicationLayer();

  /// Destructor.
  virtual ~eclmplPhysicalCommunicationLayer();
  
  /** Init physical layer.

      @param argc Number of command line argument.
      @param argv Command line arguments.
  */
  virtual void physicalInit( SimulationConfiguration &configuration );

  /** Get the Id of the simulation manager.

      @return Id of the simulation manager.
  */
  int physicalGetId() const;

  /** Send buffer.

      @param buffer Char buffer to send.
      @param size Size of the buffer to send.
  */
  virtual void physicalSend( const SerializedInstance *toSend, 
			     unsigned int dest);

  /** Probe to see if there are messages to retrieve.

      @return The retrieved message (NULL if no message).
  */
  virtual SerializedInstance *physicalProbeRecv();

  /// Clean up and call MPI_Finalize.
  virtual void physicalFinalize();
  
  /** Return how many processes are involved in the communicator.

      @return The number of processes involved in the communicator.
  */
  virtual int physicalGetSize() const;

  //@} // End of Public Class Methods of eclmplPhysicalCommunicationLayer.

protected:
  /**@name Protected Class Methods of eclmplPhysicalCommunicationLayer. */
  //@{
  virtual void probeNetwork() = 0;
  virtual SerializedInstance *getNextInSequence();
  virtual int peekNextInSequenceSize();
  virtual void initializeCommunicationLayerAttributes() = 0;
  //@} // End of Protected Class Attributes of eclmplPhysicalCommunicationLayer.

  /**@name Protected Class Attributes of eclmplPhysicalCommunicationLayer. */
  //@{

  /// Id of this communicator.
  unsigned int physicalId;

  /// Number of simulation managers involved in the communicator.
  unsigned int physicalSize;

  eclmplConnectionInterface *connInterface;

  deque<NetworkMessage *> inOrderMessageQ;

  //@} // End of Protected Class Attributes of eclmplPhysicalCommunicationLayer.
};

#endif
