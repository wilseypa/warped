#include "UDPConnectionInterface.h"

#include <vector>
#include <string>


const std::string configFile = "procgroup";
const int maxBuf = 65535;
const int sendSocketBufferSize = 65535;
const int recvSocketBufferSize = 65535;

UDPConnectionInterface::UDPConnectionInterface(const unsigned int &mtuSize) {
  mtu = mtuSize;
} // End of constructor.

bool 
UDPConnectionInterface::establishConnections(const int * const argc, 
					     const char * const * const * const argv) {
  ASSERT(*argc > 0);

  // We have to check the last argument on the command line to be able
  // to figure out if we are the first communicator started, i.e. the
  // master, or if we have been forked off, i.e. a slave.
  std::string finalParameter((*argv)[*argc - 1]);
  if (finalParameter == "amslave") {
    slaveStartupInfo startupInfo;
    startupInfo = parseCommandLineArguments(argc, argv);
    SocketBasedConnectionInterface::establishConnections(startupInfo);
  }
  else {
    eclmplConfigFileTable *connTable = scanConfigFile(configFile);
    SocketBasedConnectionInterface::establishConnections(argc, argv, *connTable);
    delete connTable;
  }
  eclmplConfigFileTable udpConnectionTable;
  createAndDistributeRecvSocketVector(udpConnectionTable);
  createAndReceiveSendSocketVector(udpConnectionTable);
  closeTcpSockets();

  connected = true;
  return connected;
} // End of establishConnections(...).

void 
UDPConnectionInterface::createAndDistributeRecvSocketVector(eclmplConfigFileTable &udpConnectionTable) {
  // First create the socket vector.
  createSocketPtrVector(recvSocket);

  // Then find and bind unused port numbers.
  obtainAndBindUnusedPorts(recvSocket);

#if 0
  cerr << connectionId << ": hostName=" << hostName << endl;
  cerr << connectionId << ": recvSocket=";
  for (unsigned int i = 0; i < numberOfConnections; i++) {
    cerr << recvSocket[i]->getPortNumber() << " ";
  }
  cerr << endl;
#endif

  // Distribute port number information.
  if (connectionId == 0) {
    // Add entry for master first.
    std::vector<std::string> portInfoStr;
    std::string tmpStr;
    portInfoStr.push_back(hostName);
    for (unsigned int id = 0; id < numberOfConnections; id++) {
      if (id == connectionId) {
	tmpStr = std::to_string(0);
      }
      else {
	tmpStr = std::to_string(recvSocket[id]->getPortNumber());
      }
      portInfoStr.push_back(tmpStr);
    }
    udpConnectionTable.addEntry(portInfoStr);

    // The master will receive information from all slaves.
    char portInfo[maxBuf];
    unsigned int portInfoLen;
    unsigned int sourceId;
    for (unsigned int id = 1; id < numberOfConnections; id++) {
      eclmplConfigFileTable tmpPortInfoTable;
      portInfoLen = maxBuf;
      sourceId = id;
      SocketBasedConnectionInterface::recv(portInfoLen, portInfo, sourceId);
      tmpPortInfoTable.deserialize(portInfo);
      udpConnectionTable.addEntry(tmpPortInfoTable.getEntry(0));
    }
  }
  else {
    // Each slave will send port number information to master.
    eclmplConfigFileTable tmpPortInfoTable;
    std::vector<std::string> portInfoStr;
    std::string tmpStr;
    portInfoStr.push_back(hostName);
    for (unsigned int id = 0; id < numberOfConnections; id++) {
      if (id == connectionId) {
	       tmpStr = std::to_string(0);
      }
      else {
	       tmpStr = std::to_string(recvSocket[id]->getPortNumber());
      }
      portInfoStr.push_back(tmpStr);
    }
    tmpPortInfoTable.addEntry(portInfoStr);
    char srlzdPortInfo[maxBuf];
    unsigned int srlzdPortInfoLen = maxBuf;
    tmpPortInfoTable.serialize(srlzdPortInfo, srlzdPortInfoLen);
    unsigned int masterId = 0;
    SocketBasedConnectionInterface::send(srlzdPortInfoLen, srlzdPortInfo, masterId);
  }
} // End of createAndDistributeRecvSocketVector(...).

void 
UDPConnectionInterface::createAndReceiveSendSocketVector(eclmplConfigFileTable &udpConnectionTable) {
  // First create the socket vector.
  createSocketPtrVector(sendSocket);

  // Distribute port number information.
  if (connectionId == 0) {
    char srlzdPortInfo[maxBuf];
    unsigned int srlzdPortInfoLen = maxBuf;
    udpConnectionTable.serialize(srlzdPortInfo, srlzdPortInfoLen);
    for (unsigned int id = 1; id < numberOfConnections; id++) {
      SocketBasedConnectionInterface::send(srlzdPortInfoLen, srlzdPortInfo, id);
    }
  }
  else {
    char portInfo[maxBuf];
    unsigned int portInfoLen;
    unsigned int sourceId;
    portInfoLen = maxBuf;
    sourceId = 0;
    SocketBasedConnectionInterface::recv(portInfoLen, portInfo, sourceId);
    udpConnectionTable.deserialize(portInfo);
  }

#if 0
  cerr << connectionId << ": UDP CONNECTION TABLE=\n" << udpConnectionTable << endl << endl;
#endif

  // Connect UDP sockets. This will not actually establish a connection, but
  // will do a lookup for the remote hostname so that we will save time later on.
  for (unsigned int id = 0; id < numberOfConnections; id++) {
    if (id != connectionId) {
      std::vector<std::string> entry = udpConnectionTable.getEntry(id);
      string host = entry[0];
      // each entry looks like: hostname listenPort0 ... listenPortN, 
      // so Port_i is accessed through entry[i+1]
      int portNr = std::stoi(entry[connectionId+1]);
      ASSERT(sendSocket[id]->wConnect(host.c_str(), portNr) != -1);
    }
  }
} // End of createAndReceiveSendSocketVector(...).

void
UDPConnectionInterface::closeTcpSockets() {
  for (unsigned int i = 0; i < numberOfConnections; i++) {
    ASSERT(close(socket[i]->getSocketFd()) != -1);
  }
} // End of closeTcpSockets().

// Just to keep the compiler happy...
void
UDPConnectionInterface::createSocketPtrVector() {
  SocketBasedConnectionInterface::createSocketPtrVector();
} // End of createSocketPtrVector().

void
UDPConnectionInterface::createSocketPtrVector(std::vector<eclmplSocket *> &socketVector) {
  ASSERT(numberOfConnections > 0);
  socketVector.resize(numberOfConnections);

  int protocol = 0;
  int type = SOCK_DGRAM; // UDP socket.
  ASSERT(sendSocketBufferSize >= (int)mtu && recvSocketBufferSize >= (int)mtu);
  for (unsigned int i = 0; i < numberOfConnections; i++) {
    ASSERT( (socketVector[i] = new eclmplSocket(type, protocol)) != 0);

    // Set the socket buffer sizes.
    ASSERT(socketVector[i]->wSetsockopt(SOL_SOCKET, SO_SNDBUF, (int *)&sendSocketBufferSize, sizeof(int)) != -1);
    ASSERT(socketVector[i]->wSetsockopt(SOL_SOCKET, SO_RCVBUF, (int *)&recvSocketBufferSize, sizeof(int)) != -1);
  }
} // End of createSocketVector()

void
UDPConnectionInterface::obtainAndBindUnusedPorts(std::vector<eclmplSocket *> &socketVector) {
  // Registered ports range from 1024 to 49151.
  // Ephemeral ports (or private ports) range from 49152 to 65536, see Stevens p. 42.
  // All of these are available for us to use (if not already taken by someone else).
  // We could let the kernel choose an unused port for us, but then we would be 
  // limited to the ephemeral ports only (see Stevens p. 92).
  int myPort;
  for (unsigned int id = 0; id < numberOfConnections; id++) {
    myPort = (rand() % 64512)+ 1024;
    while (socketVector[id]->wBind(myPort) < 0) {
      myPort = (rand() % 64512)+ 1024;
    }
  }
} // End of obtainUnusedPort(...).

void 
UDPConnectionInterface::send(const unsigned int &msgSize,  const char * const msg,
				     const unsigned int &destinationId) {
  ASSERT(msgSize <= mtu); // we do not provide fragmentation of messages.
  ASSERT(destinationId < numberOfConnections);
  ASSERT(destinationId != connectionId);

  int flags = 0;
  ASSERT(sendSocket[destinationId]->wSend(msg, msgSize, flags) != -1);
} // End of send(...).

bool 
UDPConnectionInterface::recv(unsigned int &msgSize, char * const msg, 
				     unsigned int &sourceId) {
  ASSERT(msgSize >= mtu); // msg needs to be able to receive a whole message.
  int flags = 0;
  ASSERT( (msgSize = recvSocket[sourceId]->wRecv(msg, mtu, flags)) > 0);
  return true;
} // End of recv(...).
