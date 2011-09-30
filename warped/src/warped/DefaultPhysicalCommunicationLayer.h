#ifndef DEFAULT_PHYSICAL_COMMUNICATION_LAYER_H
#define DEFAULT_PHYSICAL_COMMUNICATION_LAYER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "eclmpl/PhysicalCommunicationLayer.h"

/** The DefaultPhysicalCommunicationLayer class.

    This is a default physical communication layer, derived from the
    PhysicalCommunicationLayer abstract interface class. This
    communication layer assumes that a uniprocessor is used and that
    no physical communication layer is actually used. Thus, none of
    the methods in the class have any work to do.
 */
class DefaultPhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
  /**@name Public Class Methods of DefaultPhysicalCommunicationLayer. */
  //@{

  /// Default Constructor.
  DefaultPhysicalCommunicationLayer();

  /// Destructor.
  ~DefaultPhysicalCommunicationLayer();

  /** Initialize the physical communication layer.

      @param configuration The SimulationConfiguration to use for this run.
  */
  void physicalInit( SimulationConfiguration &configuration );

  /** Get the Id of the simulation manager.

      @return Id of the simulation manager.
  */
  int physicalGetId() const;

  /** Send buffer.

      @param buffer Char buffer to send.
      @param size Size of the buffer to send.
      @param dest destination simulation manager
  */
  void physicalSend( const SerializedInstance *toSend, unsigned int dest);


  /** Check the probe to see if there are message to retrieve.

      @return The retrieved message (NULL if no message).
  */
  SerializedInstance *physicalProbeRecv();

//   /** Retrieve message into a buffer.

//       @param buffer Buffer to which we save the message.
//       @param size Size of the buffer.
//       @param sizeStatus Was the size of retr. msg > size?
//       @return True/False, Was any message retrieved?
//   */
//   bool physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus);

  /// Clean up.
  void physicalFinalize();

  //@} // End of Public Class Methods of DefaultPhysicalCommunicationLayer.
};

#endif
