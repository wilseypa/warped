#ifndef TCP_CONNECTION_INTERFACE_H
#define TCP_CONNECTION_INTERFACE_H

#include "SocketBasedConnectionInterface.h"
#include "eclmplCommonInclude.h"

class TCPConnectionInterface : public SocketBasedConnectionInterface {
public:
    TCPConnectionInterface() {}
    TCPConnectionInterface(const unsigned int& mtuSize);
    virtual ~TCPConnectionInterface() {}
    virtual bool establishConnections(const int* const argc,
                                      const char* const* const* const argv);
    void disableNagle(const unsigned int& id);
    void disableNagle();
protected:
};

#endif
