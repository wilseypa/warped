#ifndef UDP_CONNECTION_INTERFACE_H
#define UDP_CONNECTION_INTERFACE_H

#include <vector>                       // for vector

#include "SocketBasedConnectionInterface.h"
#include "eclmplCommonInclude.h"

class eclmplConfigFileTable;
class eclmplSocket;

class UDPConnectionInterface : public SocketBasedConnectionInterface {
public:
    UDPConnectionInterface() {}
    UDPConnectionInterface(const unsigned int& mtuSize);
    virtual ~UDPConnectionInterface() {}
    virtual bool establishConnections(const int* const argc,
                                      const char* const* const* const argv);
    void send(const unsigned int& msgSize,  const char* const msg, const unsigned int& destinationId);
    bool recv(unsigned int& msgSize, char* const msg,unsigned int& sourceId);

    std::vector<eclmplSocket*> sendSocket;
    std::vector<eclmplSocket*> recvSocket;
protected:
    void createAndDistributeRecvSocketVector(eclmplConfigFileTable& udpConnectionTable);
    void createAndReceiveSendSocketVector(eclmplConfigFileTable& udpConnectionTable);
    void closeTcpSockets();
    void createSocketPtrVector();
    void createSocketPtrVector(std::vector<eclmplSocket*>& socketVector);
    void obtainAndBindUnusedPorts(std::vector<eclmplSocket*>& socketVector);
};

#endif
