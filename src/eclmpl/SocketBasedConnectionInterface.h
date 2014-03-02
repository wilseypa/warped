#ifndef SOCKET_BASED_CONNECTION_INTERFACE
#define SOCKET_BASED_CONNECTION_INTERFACE

#include <vector>

#include "eclmplCommonInclude.h"
#include "eclmplSocket.h"
#include "eclmplConnectionInterfaceImplementationBase.h"

class SocketBasedConnectionInterface : public eclmplConnectionInterfaceImplementationBase,
    public eclmplSocket {
public:
    /// Simulation manager file descriptors for sends and receives.
    std::vector<eclmplSocket*> socket;

    SocketBasedConnectionInterface();
    SocketBasedConnectionInterface(const unsigned int& mtuSize);
    virtual ~SocketBasedConnectionInterface();

    virtual bool establishConnections(const int* const argc,
                                      const char* const* const* const argv);
    virtual void tearDownConnections();

    virtual void send(const unsigned int& msgSize, const void* const msg,
                      const unsigned int& destinationId);
    virtual bool recv(unsigned int& msgSize, char* const msg, unsigned int& sourceId);

protected:
    virtual void establishConnections(const int* const argc,
                                      const char* const* const* const argv,
                                      const eclmplConfigFileTable& connTable);
    virtual void establishConnections(const slaveStartupInfo& info);
    void createSocketPtrVector();
    void createNewSocket(const unsigned int& id, const int type);
    virtual void setDefaultSocketOptions(const unsigned int& id);
    int obtainAndBindUnusedPort(const unsigned int& id);
    virtual void synchronizeWithSlaves();
    virtual void synchronizeWithMaster();
    virtual void establishConnectionsWithPeerSlaves(const eclmplConfigFileTable& connTable);
    virtual void distributeConfigFileTable(const eclmplConfigFileTable& connTable);

    std::string hostName;
};

#endif
