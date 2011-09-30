#include "TCPConnectionInterface.h"

TCPConnectionInterface::TCPConnectionInterface(const unsigned int &mtuSize) {
  mtu = mtuSize;
  socketFd = 0;
} // End of constructor.

bool 
TCPConnectionInterface::establishConnections(const int * const argc, 
					     const char * const * const * const argv) {
  return SocketBasedConnectionInterface::establishConnections(argc, argv);
} // End of establishConnections(...).

void
TCPConnectionInterface::disableNagle(const unsigned int &id) {
  int val = 1;
  // Disable Nagle's algorithm. See p. 179, p. 202 Stevens, Vol. 1.
  ASSERT(socket[id]->wSetsockopt(IPPROTO_TCP, TCP_NODELAY, (int *)&val, sizeof(val)) != -1);
} // End of disableNagle(...).

void
TCPConnectionInterface::disableNagle() {
  int val;
  // Disable Nagle's algorithm. See p. 179, p. 202 Stevens, Vol. 1.
  for (unsigned int id = 0; id < numberOfConnections; id++) {
    val = 1;
    ASSERT(socket[id]->wSetsockopt(IPPROTO_TCP, TCP_NODELAY, (int *)&val, sizeof(val)) != -1);
  }
} // End of disableNagle().
