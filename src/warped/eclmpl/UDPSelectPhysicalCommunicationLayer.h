#ifndef UDP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H
#define UDP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmplSocket.h"
#include "eclmplCommonInclude.h"
#include "eclmplUnreliableNetworkMessage.h"
#include "eclmplReliablePhysicalCommunicationLayer.h"
#include "eclmplTimer.h"
#include "UDPConnectionInterface.h"

/** The UDPSelectPhysicalCommunicationLayer class.

*/
class UDPSelectPhysicalCommunicationLayer : public eclmplReliablePhysicalCommunicationLayer {
public:
  /**@name Public Class Methods of UDPSelectPhysicalCommunicationLayer. */
  //@{

  /// Default Constructor.
  UDPSelectPhysicalCommunicationLayer();

  /// Destructor.
  ~UDPSelectPhysicalCommunicationLayer();
  
  //@} // End of Public Class Methods of UDPSelectPhysicalCommunicationLayer.

protected:
  /**@name Protected Class Methods of UDPSelectPhysicalCommunicationLayer. */
  //@{
  void probeNetwork();
  void initializeCommunicationLayerAttributes();

  //bool UDP_SynchronizeMaster(const eclmplUnreliableNetworkMessageType &msgType);
  //bool UDP_SynchronizeSlave(const eclmplUnreliableNetworkMessageType &msgType);
  //void UDP_Finalize();

  //@} // End of Protected Class Attributes of UDPSelectPhysicalCommunicationLayer.

  /**@name Protected Class Attributes of UDPSelectPhysicalCommunicationLayer. */
  //@{

  // UDP_SELECT:
  fd_set selectListenSet, fdSet;
  int maxFd;

  //@} // End of Protected Class Attributes of UDPSelectPhysicalCommunicationLayer.
};

#endif
