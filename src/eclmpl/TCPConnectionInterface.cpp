#include "TCPConnectionInterface.h"

TCPConnectionInterface::TCPConnectionInterface() : SocketBasedConnectionInterface() {
} // End of Default Constructor

TCPConnectionInterface::TCPConnectionInterface(const unsigned int& mtuSize) : SocketBasedConnectionInterface(mtuSize) {
} // End of Initialization Constructor.

TCPConnectionInterface::~TCPConnectionInterface() {
} // End of Destructor

void TCPConnectionInterface::disableNagle(const unsigned int& id) {
    int val = 1;
    // Disable Nagle's algorithm. See p. 179, p. 202 Stevens, Vol. 1.
    ASSERT(socket[id]->wSetsockopt(IPPROTO_TCP, TCP_NODELAY, (int*)&val, sizeof(val)) != -1);
} // End of disableNagle(...).

void
TCPConnectionInterface::disableNagle() {
    int val;
    // Disable Nagle's algorithm. See p. 179, p. 202 Stevens, Vol. 1.
    for (unsigned int id = 0; id < numberOfConnections; id++) {
        val = 1;
        ASSERT(socket[id]->wSetsockopt(IPPROTO_TCP, TCP_NODELAY, (int*)&val, sizeof(val)) != -1);
    }
} // End of disableNagle().
