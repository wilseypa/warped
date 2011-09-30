#include "TCPSelectPhysicalCommunicationLayer.h"

using std::cerr;
using std::endl;

const unsigned int mtu = ECLMPL_MTU; // No messages with greater size may be received or sent.
                                     // This number itself can be changed of course...
const int maxBuf = 65535;

TCPSelectPhysicalCommunicationLayer::TCPSelectPhysicalCommunicationLayer() {
  connInterface = new TCPConnectionInterface(mtu);
  maxFd = -1;
  FD_ZERO(&selectListenSet);
  FD_ZERO(&fdSet);
  recvBuf = new char[maxBuf];
} // End of default constructor.

TCPSelectPhysicalCommunicationLayer::~TCPSelectPhysicalCommunicationLayer()
{
  delete connInterface;
  delete []recvBuf;
} // End of desctructor.

void
TCPSelectPhysicalCommunicationLayer::probeNetwork() {
  unsigned int size = 0;
  NetworkMessage *nwMsg;

  // Probe to see if any messages are available.
  timeval timeout;

  // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
  // The effect of this timer will greatly impact performance. The effect is sometimes
  // not predictable. However, the effect seems to be machine (read Linux Pentium or
  // Solaris Sun) dependent. Try values of tv_sec = 0, and tv_usec = 0, 1, 100 for 
  // starters.
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE

  selectListenSet = fdSet;
  select(maxFd, &selectListenSet, NULL, NULL, &timeout);
  for (unsigned int i = 0; i < physicalSize; i++) {
    if (static_cast<TCPConnectionInterface *>(connInterface)->socket[i]->wFD_ISSET(&selectListenSet) == true) {
      size = maxBuf;
      connInterface->recv(size, recvBuf, i);
      if (size > 0) {
		char *newBuf = new char[size];
		memcpy(newBuf, recvBuf, size);
		nwMsg = new NetworkMessage((int)size, newBuf);
		inOrderMessageQ.push_back(nwMsg);
      }
      else {
    	 // These are commented out because I believe messages of size=0 should be allowed
    	 // cerr << physicalId << ": Error! TCP Input buffer corrupted. Aborting." << endl;
    	 // exit(1);
      }
    } // End of if (recvSocket[i].wFD_ISSET(&selectListenSet) == true).
  } // End of for (int i = 0; i < physicalSize; i++).
} // End of TCP_probeNetwork().

void
TCPSelectPhysicalCommunicationLayer::initializeCommunicationLayerAttributes() {
  static_cast<TCPConnectionInterface *>(connInterface)->disableNagle();
  maxFd = -1;
  for (unsigned int i = 0; i < physicalSize; i++) {
    if (static_cast<TCPConnectionInterface *>(connInterface)->socket[i]->getSocketFd() > maxFd) {
      maxFd = static_cast<TCPConnectionInterface *>(connInterface)->socket[i]->getSocketFd();
    }
    FD_SET(static_cast<TCPConnectionInterface *>(connInterface)->socket[i]->getSocketFd(), &selectListenSet);
  }
  fdSet = selectListenSet;
  maxFd += 1; // Needed for call to select.
} // End of setupAttributes(...).
