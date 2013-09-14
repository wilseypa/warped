#ifndef ECLMPL_RELIABLE_PHYSICAL_COMMUNICATION_LAYER_H
#define ECLMPL_RELIABLE_PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmplCommonInclude.h"
#include "eclmplUnreliableNetworkMessage.h"
#include "PhysicalCommunicationLayer.h"
#include "eclmplTimer.h"
#include "eclmplConnectionInterface.h"

using std::priority_queue;
using std::vector;
using std::deque;

/** The eclmplReliablePhysicalCommunicationLayer class.

*/
class eclmplReliablePhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
  /**@name Public Class Methods of eclmplReliablePhysicalCommunicationLayer. */
  //@{

  /// Default Constructor.
  eclmplReliablePhysicalCommunicationLayer();

  /// Destructor.
  virtual ~eclmplReliablePhysicalCommunicationLayer();
  
  /** Init physical layer.
  */
  virtual void physicalInit();

  /** Get the Id of the simulation manager.

      @return Id of the simulation manager.
  */
  int physicalGetId() const;

  /** Send buffer.

      @param buffer Char buffer to send.
      @param size Size of the buffer to send.
  */
  virtual void physicalSend( const SerializedInstance *toSend, unsigned int dest );

  /** Probe to see if there are messages to retrieve.

      @return The retrieved message (NULL if no message).
  */
  virtual SerializedInstance *physicalProbeRecv();

//   /** Retrieve message into a buffer.

//       @param buffer Buffer to which we save the message.
//       @param size Size of the buffer.
//       @param sizeStatus Was the size of retr. msg > size?
//       @return True/False, Was any message retrieved?
//   */
//   virtual bool physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus);

  /// Clean up and call MPI_Finalize.
  virtual void physicalFinalize();
  
  /** Return how many processes are involved in the communicator.

      @return The number of processes involved in the communicator.
  */
  virtual int physicalGetSize() const;

  //@} // End of Public Class Methods of eclmplReliablePhysicalCommunicationLayer.

protected:
  /**@name Protected Class Methods of eclmplReliablePhysicalCommunicationLayer. */
  //@{
  virtual void send(eclmplUnreliableNetworkMessage * const nwMsg, const unsigned int &dest);
  virtual void probeNetwork() = 0;
  virtual bool insertInOrderMessage(eclmplUnreliableNetworkMessage *nwMsg);
  virtual void processNwMsgInfo(eclmplUnreliableNetworkMessage *nwMsg);
  virtual void updateOutOfOrderMessageQs(const unsigned int &source, 
				     const bool &sendRetransmissionReq);
  virtual unsigned int checkRetransmissionTimeout();
  virtual SerializedInstance *getNextInOrder();
  virtual int peekNextInOrderSize();
  virtual void sendAck(unsigned int dest);
  virtual void initializeCommunicationLayerAttributes() = 0;
  //@} // End of Protected Class Attributes of eclmplReliablePhysicalCommunicationLayer.

  /**@name Protected Class Attributes of eclmplReliablePhysicalCommunicationLayer. */
  //@{

  // Id of the simulation manager.
  //int mySimulationManagerID;

  /// Id of this communicator.
  unsigned int physicalId;

  /// Number of simulation managers involved in the communicator.
  unsigned int physicalSize;

  /// Simulation manager file descriptors for sends and receives.
  //eclmplSocket *recvSocket;
  //eclmplSocket *sendSocket;
  eclmplConnectionInterface *connInterface;


  /// nextSendSequenceNumber[physicalSize].
  SequenceNumber *nextSendSequenceNumber;

  /// acknowledgedSequenceNumberReceived[[hysicalSize].
  SequenceNumber *acknowledgedSequenceNumberReceived;
  // i.e. what is the highest sequence number that we have
  // sent on a link that has been acknowledged.

  /// acknowledgedSequenceNumber[physicalSize].
  SequenceNumber *acknowledgedSequenceNumberSent;
  // highest acknowledged sequence number sent.

  /// highestInSequenceNumberRecived[physicalSize].
  SequenceNumber *highestInSequenceNumberReceived; 
  // i.e. highest sequence number that may be acknowlegded.

  SequenceNumber *highestInSequenceNumberProcessed;
  // i.e. highest sequence number given to the application.

  SequenceNumber *endOfHole;

  bool outOfOrderMessageQsAreEmpty;
  // outOfOrderMessageQs[physicalSize]
  vector<priority_queue<eclmplUnreliableNetworkMessage *, vector<eclmplUnreliableNetworkMessage *>, 
                        lessPriority<eclmplUnreliableNetworkMessage *> > > outOfOrderMessageQs;
  deque<eclmplUnreliableNetworkMessage *> inOrderMessageQ;
  // synchBuffer[physicalSize]
  vector<deque<eclmplUnreliableNetworkMessage *> > synchBuffer;
  // sendQs[physicalSize]
  vector<deque<eclmplUnreliableNetworkMessage *> > sendQs;

  //unsigned int windowSize;

  unsigned int maxNrUnackedMsgs;

  eclmplTimer *retransmissionTimer;

  //@} // End of Protected Class Attributes of eclmplReliablePhysicalCommunicationLayer.
};

#endif
