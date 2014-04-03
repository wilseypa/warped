#ifndef TCP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H
#define TCP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H

#include <sys/select.h>                 // for fd_set

#include "NetworkMessage.h"
#include "TCPConnectionInterface.h"
#include "eclmplCommonInclude.h"
#include "eclmplPhysicalCommunicationLayer.h"

/** The TCPSelectPhysicalCommunicationLayer class.

*/
class TCPSelectPhysicalCommunicationLayer : public eclmplPhysicalCommunicationLayer {
public:
    /**@name Public Class Methods of TCPSelectPhysicalCommunicationLayer. */
    //@{

    /// Default Constructor.
    TCPSelectPhysicalCommunicationLayer();

    /// Destructor.
    ~TCPSelectPhysicalCommunicationLayer();

    //@} // End of Public Class Methods of TCPSelectPhysicalCommunicationLayer.

protected:
    /**@name Protected Class Methods of TCPSelectPhysicalCommunicationLayer. */
    //@{

    void probeNetwork();
    void initializeCommunicationLayerAttributes();

    //@} // End of Protected Class Attributes of TCPSelectPhysicalCommunicationLayer.

    /**@name Protected Class Attributes of TCPSelectPhysicalCommunicationLayer. */
    //@{

    // For calls to select.
    fd_set selectListenSet, fdSet;
    int maxFd;
    char* recvBuf;

    //@} // End of Protected Class Attributes of TCPSelectPhysicalCommunicationLayer.
};

#endif
