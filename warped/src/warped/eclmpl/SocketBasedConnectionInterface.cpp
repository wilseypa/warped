#include "SocketBasedConnectionInterface.h"

using std::cerr;
using std::endl;

// Only here for debugging... remove when done.
void __eclmpl__printCharStr(const char * const str, const unsigned int &strLen) {
  cerr << "\"";
  for (unsigned int i = 0; i < strLen; i++) {
    if (str[i] == '\0')
      cerr << "'\\0'";
    else if (str[i] == '\n')
      cerr << "'\\n'";
    else if (str[i] >= 32 && str[i] <= 126)
      cerr << str[i];
    else 
      cerr << "'" << (int)str[i] << "'";
  }
  cerr << "\"";
} 

// Precondition: char msgHeader[msgHeaderSize] = "00000";
void __eclmpl__serializeMessageHeader(char *msgHeader, const unsigned int &size) {
  int a = 0;
  unsigned int len = size;
  bool start = false;
  for (unsigned int i = 10000; i > 0; i /= 10) {
    if (len/i > 0) {
      start = true;
    }
    if (start) {
      msgHeader[a] = 48+len/i;
      len = len % i;
      a++;
    }
  }
  msgHeader[a] = '\0';
} // End of __eclmpl__serializeMessageHeader(...).

const int sendSocketBufferSize = 65536;
const int recvSocketBufferSize = 65536;
const int maxBuf = 65535;
 
// For stream-based sockets we send a header prepended to each message, containing
// the size in bytes of the message. We need at most 5 bytes (+ '\0') to store
// the serialized length of any message (we'll never send a message with a size
// greater than 99999 bytes).
const unsigned int msgHeaderSize = 6;

SocketBasedConnectionInterface::SocketBasedConnectionInterface() {
  //Ignoring SIGPIPE so that read and send can handle through EPIPE.
  signal(SIGPIPE, SIG_IGN);
} // End of default constructor.

SocketBasedConnectionInterface::SocketBasedConnectionInterface(const unsigned int &mtuSize) {
  mtu = mtuSize;
} // End of constructor.

SocketBasedConnectionInterface::~SocketBasedConnectionInterface() {
} // End of destructor.

// The connection scheme is as follows:
// ----------------------------------------------
// b - bind, l - listen, a - accept, f - fork
// c - connect, r - receive, s - send
// fd - socket file descriptor,
// fup - find unused port number
// Port Px is used for communicating with x.
// 0 <= x <= n
// ----------------------------------------------
// 0. fup            --> P0 
//    b(P0) l(P0) 
//    f(1) a(P0)     --> fd[1]            (P0 is added to 1 <= x's argv <= n)     
//    r(fd[1], "P1") --> P1 is known
//    f(2) a(P0)     --> fd[2]
//    r(fd[2], "P2") --> P2 is known
//    ...
//    f(n) a(P0)     --> fd[n]
//    r(fd[n], "Pn") --> Pn is known
//    s(fd[1 <= x <= n], ConnFileTable)  (All portnumbers are added to table)
//    r(fd[1 <= x <= n], "done connecting with peer slaves") (can be received in arbitrary order)
//    s(fd[1 <= x <= n], "synchronize")                      (sent in order)
// 1. fup            --> P1
//    b(P1) l(P1) 
//    c(P0)          --> fd[0]
//    s(fd[0], P1)
//    r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
//    c(P2)          --> fd[2]
//    c(P3)          --> fd[3]
//    ...
//    c(Pn)          --> fd[n]
//    s(fd[2], "go ahead and connect")
//    s(fd[0], "done connecting with peer slaves")
//    r(fd[0], "synchronize")
// 2. fup            --> P2
//    b(P2) l(P2) 
//    c(P0)          --> fd[0]
//    s(fd[0], P2)
//    r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
//    a(P2)          --> fd[1]
//    r(fd[1], "go ahead and connect")
//    c(P3)          --> fd[3]
//    ...
//    c(Pn)          --> fd[n]
//    s(fd[3], "go ahead and connect")
//    s(fd[0], "done connecting with peer slaves")
//    r(fd[0], "synchronize")
// 3. fup            --> P3
//    b(P3) l(P3) 
//    c(P0)          --> fd[0]
//    s(fd[0], P3)
//    r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
//    a(P3)          --> fd[1]
//    a(P3)          --> fd[2]
//    r(fd[2], "go ahead and connect")
//    c(P4)          --> fd[4]
//    ...
//    c(Pn)          --> fd[n]
//    s(fd[4], "go ahead and connect")
//    s(fd[0], "done connecting with peer slaves")
//    r(fd[0], "synchronize")
// ......
// n-1.
//    fup            --> Pn-1
//    b(Pn-1) l(Pn-1) 
//    c(P0)          --> fd[0]
//    s(fd[0], Pn-1)
//    r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
//    a(Pn-1)          --> fd[1]
//    a(Pn-1)          --> fd[2]
//    ...
//    a(Pn-1)          -->  fd[n-2]
//    r(fd[n-2], "go ahead and connect")
//    c(Pn)          --> fd[n]
//    s(fd[0], "done connecting with peer slaves")
//    r(fd[0], "synchronize")
// n. fup            --> Pn
//    b(Pn) l(Pn) 
//    c(P0)          --> fd[0]
//    s(fd[0], Pn)
//    r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
//    a(Pn)          --> fd[1]
//    a(Pn)          --> fd[2]
//    ...
//    a(Pn)          --> fd[n-1]
//    s(fd[0], "done connecting with peer slaves")
//    r(fd[0], "synchronize")
// ----------------------------------------------
// => Everyone is connected to everyone and everyone is synchronized, 
//    so they are ready to go at about the same time.

// This is just here to keep the compiler happy... (g++ 2.95.2 that is).
bool 
SocketBasedConnectionInterface::establishConnections(const int * const argc, 
						     const char * const * const * const argv) {
  return eclmplConnectionInterfaceImplementationBase::establishConnections(argc, argv);
} // End of establishConnections(...).

// Master establishes connection with slaves after forking them off.
void 
SocketBasedConnectionInterface::establishConnections(const int * const argc, 
						     const char * const * const * const argv,
						     const eclmplConfigFileTable &connTable) {
  
  ASSERT(connectionId == 0); // Only master executes this method.
  ASSERT(connTable.getNumberOfEntries() == numberOfConnections);
  vector<string> masterEntry = connTable.getEntry(0);
  ASSERT(masterEntry.size() == 2);

  // Create a vector of numberOfConnections number of socket pointers.
  createSocketPtrVector();
  
  // Set seed for random number generator.
  srand(93);

  // Create master socket and find an unused portnumber for it, bind and listen to it.
  createNewSocket(connectionId, SOCK_STREAM); // SOCK_STREAM --> TCP
  setDefaultSocketOptions(connectionId);
  string myPort = std::to_string(obtainAndBindUnusedPort(connectionId));

  eclmplConfigFileTable localConnTable = connTable;
  // Add portnumber info to localConnTable.
  localConnTable.addToEntry(myPort, (int)connectionId);

  // Start slaves and distribute updated connTable.
  eclmplContactInfo myContactInfo;
  myContactInfo.setProvidedBy(connectionId);
  vector<string> contactInfo;
  hostName = masterEntry[0];
  contactInfo.push_back(masterEntry[0]);
  contactInfo.push_back(myPort);
  myContactInfo.setContactArguments(contactInfo);
#if 0
  for (int i =0; i<connTable.getNumberOfEntries();i++)
  {
	  for (int j=0; j<connTable.getEntry(i).size(); j++)
	  {
		  std::cout << " " << connTable.getEntry(i)[j];
	  }
	  std::cout << std::endl;
  }
  std::cout << "NumberCons: " << numberOfConnections << std::endl;
#endif
  for (unsigned int i = 1; i < numberOfConnections; i++) {
    forkOffSlave(argc, argv, connTable, myContactInfo, i);
    int newSocketFd;
    struct sockaddr_in addr;
    int addrLen = sizeof(addr);
    ASSERT(socket[0]->wAccept(newSocketFd, (struct sockaddr *)&addr, &addrLen) != -1);
    ASSERT( (socket[i] = new eclmplSocket(newSocketFd, addr, (int)addrLen)) != 0);
    setDefaultSocketOptions(i);
  
#ifdef DEBUG_SOCKET_CONN_START
    cerr << "0: Forked off slave " << i << ". Now waiting for its portnumber." << endl;
#endif
    // Receive portnumber where i is listening.
    unsigned int msgSize = mtu;
    char msg[mtu];
    while (SocketBasedConnectionInterface::recv(msgSize, msg, i) == false) {
      msgSize = mtu; // Max allowed recv size.
    }
    string slavePortNr = std::to_string(atoi(msg));
#ifdef DEBUG_SOCKET_CONN_START
    cerr << "0: Received portnumber " << slavePortNr << "from slave " << i << "." << endl;
#endif

    // Add portnumber info to localConnTable.
    localConnTable.addToEntry(slavePortNr, (int)i);
  }

#ifdef DEBUG_SOCKET_CONN_START
  cerr << "0: Done forking off slaves. Now about to distributeConfigFileTable" << endl;
#endif
  // Send serializedConnTable to slaves.
  distributeConfigFileTable(localConnTable);
  
#ifdef DEBUG_SOCKET_CONN_START
  cerr << "0: Done with distributeConfigFileTable. Now about to synchronize with slaves." << endl;
#endif
  // Synchronize with slaves.
  synchronizeWithSlaves();

#ifdef DEBUG_SOCKET_CONN_START
  cerr << "0: Done synchronizing with slaves. Now ready to start communicating." << endl;
#endif
  // We're now ready to be used by some application...
} // End of establishConnections(...).

// Slaves establish connections with master and other slaves.
void 
SocketBasedConnectionInterface::establishConnections(const slaveStartupInfo &info) {
  ASSERT(connectionId != 0); // Only slaves executes this method.
#if 0
  cerr << "info.configTableEntry.size()=" << info.configTableEntry.size() << endl;
#endif
  ASSERT(info.configTableEntry.size() == 2);
  hostName = info.configTableEntry[0];

  // Create a vector of numberOfConnections number of socket pointers.
  createSocketPtrVector();
  
  // Set seed for random number generator.
  srand(93);

  // Create master socket and find an unused portnumber for it, bind and listen to it.
  createNewSocket(connectionId, SOCK_STREAM); // SOCK_STREAM --> TCP
  setDefaultSocketOptions(connectionId);
  string myPort = std::to_string(obtainAndBindUnusedPort(connectionId));

  // Establish connection with master.
  ASSERT(info.masterContactInfo.nrOfContactArguments() == 2);
  vector<string> args = info.masterContactInfo.contactArguments();
  // args[0]==hostname, args[1]==portNumber
  string masterHost = args[0];
  int masterPort = std::stoi(args[1]);
  unsigned int masterId = 0;
  createNewSocket(masterId, SOCK_STREAM); // SOCK_STREAM --> TCP
  setDefaultSocketOptions(masterId);
#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Connecting to master " << masterHost << " " << masterPort << endl;
#endif
  ASSERT(socket[masterId]->wConnect(masterHost.c_str(), masterPort) != -1);

#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Done connecting to master. Now sending our portnumber (" << myPort << ") to master." << endl;
  cerr << connectionId << ": _____________ mtu="<<mtu<<", myPortLen="<<(strlen(myPort.c_str())+1)<<"_____________" << endl;
#endif
  // Send a message to master containing our portnumber.
  unsigned int msgSize = mtu;
  char msg[mtu];
  ASSERT(mtu > static_cast<unsigned int>(strlen(myPort.c_str())+1));
  strcpy(msg, myPort.c_str());
  msgSize = static_cast<unsigned int>(strlen(myPort.c_str())+1);
  SocketBasedConnectionInterface::send(msgSize, msg, masterId);

#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Done sending portnumber to master. Now waiting to receive configFileTable." << endl;
#endif
  eclmplConfigFileTable connTable;
  // Receive serializedConnTable from master.
  msgSize = mtu;
  while (SocketBasedConnectionInterface::recv(msgSize, msg, masterId) == false) {
    msgSize = mtu; // Max allowed recv size.
  }
  ASSERT(masterId == 0); // masterId is a return parameter from recv.
  eclmplConfigFileTable localConnTable;
  localConnTable.deserialize(msg);

#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Done receiving configFileTable=";
  __eclmpl__printCharStr(msg, msgSize);
  cerr << endl;
#endif
#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Deserialized configFileTable=\n" << localConnTable << endl << endl;
#endif
#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Now going to establish connections with other slaves." << endl;
#endif
  // Establish connections with other slaves.
  establishConnectionsWithPeerSlaves(localConnTable);

#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Done connecting to slaves. Now waiting to synchronize with master.." << endl;
#endif
  // Let master know we're done with connection establishment and Wait
  // for synchronize message from master.
  synchronizeWithMaster();
#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": Done synchronizing with master. Now ready to start communicating." << endl;
#endif

#if 0
  cerr << connectionId << ": DOOOOOOOOOONNNNNNNNEEEEEEEE__ ";
  for (unsigned int i = 0; i < numberOfConnections; i++) {
    cerr << socket[i]->getPortNumber() << " ";
  }
  cerr << endl;
#endif
} // End of establishConnections(...).

void
SocketBasedConnectionInterface::tearDownConnections(){

  unsigned int msgSize = mtu;
  char msg[mtu];

  // RK_NOTE_7-4-10: This is not working properly. In some cases, the recv function
  // gets the teardown message and tries to deserialize it, causing problems. I am not
  // sure of a good way to ensure that the master is the last one to close down.
  /*
  // The master waits until all slaves have closed connections.
  if(connectionId == 0){
  
    // Wait for the arrival of a "teardown" message from each slave before
    // shutting down all of the connections. This ensures that the master is the
    // last one to finish.
    unsigned int in = 1;
    while(in < numberOfConnections) {
      msgSize = mtu;
      while (SocketBasedConnectionInterface::recv(msgSize, msg, in) == false) {
        msgSize = mtu; // Max allowed recv size.
      }
    
      // Only increment when a teardown message has been received.
      string recvMsg(msg);
      if(recvMsg == "teardown"){
        in++;
      }
    }

    for (unsigned int i = 0; i < socket.size(); i++) {
      debug::debugout << "ShuttingDown: " << i << " ... ";
      int ret = shutdown(socket[i]->getSocketFd(),2);
      debug::debugout << ret << std::endl;
    }
  }
  else{
    for (unsigned int i = 0; i < socket.size(); i++) {
      if(i == 0){
        // Send a tear down message to the master to let it know that we are finished.
        ASSERT(mtu > strlen("teardown"));
        strcpy(msg, "teardown");
        msgSize = (unsigned int)strlen(msg)+1;
        SocketBasedConnectionInterface::send(msgSize, msg, 0);
      }
   
      debug::debugout << "ShuttingDown: " << i << " ... ";
      int ret = shutdown(socket[i]->getSocketFd(),2);
      debug::debugout << ret << std::endl;
    }
  }*/
  debug::debugout << "Begin tearing down connections for: " << connectionId;
  for (unsigned int i = 0; i < socket.size(); i++) {
    debug::debugout << " ... " << i;
    int ret = shutdown(socket[i]->getSocketFd(),2);
  }
  debug::debugout << endl << "Finished tearing down connections for: " << connectionId << endl;
}

void
SocketBasedConnectionInterface::createSocketPtrVector() {
  ASSERT(numberOfConnections > 0);
  socket.resize(numberOfConnections);
  for (unsigned int i = 0; i < numberOfConnections; i++) {
    socket[i] = NULL;
  }
} // End of createSocketVector()

void
SocketBasedConnectionInterface::createNewSocket(const unsigned int &id, const int type) {
  int protocol = 0;
  ASSERT( (socket[id] = new eclmplSocket(type, protocol)) != 0);
} // End of createNewSocket(...).

void
SocketBasedConnectionInterface::setDefaultSocketOptions(const unsigned int &id) {
  //int val = 1; // => SO_REUSEADDR on (0 => off).
  // For now, let's assume that only one guy can bind a port number at a time.
  //ASSERT(socket[id]->wSetsockopt(SOL_SOCKET, SO_REUSEADDR, (int *)&val, sizeof(val)) != -1);

  // Make sure that close(socketFd) return immediately when called. We can't do anything about 
  // messages in transit at that time anyway. The application should make sure all necessary
  // messages are transmitted and received when time comes to closing sockets. See Stevens,
  // p. 187.
  struct linger lingerOptions;
  lingerOptions.l_onoff = 0; // off.
  lingerOptions.l_linger = 0; // linger time... since it's off it doesn't really matter.
  ASSERT(socket[id]->wSetsockopt(SOL_SOCKET, SO_LINGER, (struct linger *)&lingerOptions, 
				 sizeof(struct linger)) != -1);
  
  // Set the socket buffer sizes.
  ASSERT(sendSocketBufferSize >= (int)mtu && recvSocketBufferSize >= (int)mtu);
  ASSERT(socket[id]->wSetsockopt(SOL_SOCKET, SO_SNDBUF, (int *)&sendSocketBufferSize, sizeof(int)) != -1);
  ASSERT(socket[id]->wSetsockopt(SOL_SOCKET, SO_RCVBUF, (int *)&recvSocketBufferSize, sizeof(int)) != -1);
} // End of setDefaultSocketOptions(...).


int
SocketBasedConnectionInterface::obtainAndBindUnusedPort(const unsigned int &id) {
  // Registered ports range from 1024 to 49151.
  // Ephemeral ports (or private ports) range from 49152 to 65536, see Stevens p. 42.
  // All of these are available for us to use (if not already taken by someone else).
  // We could let the kernel choose an unused port for us, but then we would be 
  // limited to the ephemeral ports only (see Stevens p. 92).
  int myPort = (rand() % 64512)+ 1024;
  while (socket[id]->wBind(myPort) < 0) {
    myPort = (rand() % 64512)+ 1024;
  }
  ASSERT(socket[id]->wListen(2) != -1);
  return myPort;
} // End of obtainUnusedPort(...).

void
SocketBasedConnectionInterface::synchronizeWithSlaves() {
  unsigned int msgSize = mtu;
  char msg[mtu];
#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": ___________SYNCHRONIZING WITH SLAVES_____________" << endl;
#endif
  // Just wait and receive one message from each slave.
  // The arrival of a message means that the slave is done connecting with its peers.
  // The contents of the message doesn't really matter and we don't bother to check it.
  for (unsigned int i = 1; i < numberOfConnections; i++) {
    msgSize = mtu;
    while (SocketBasedConnectionInterface::recv(msgSize, msg, i) == false) {
      msgSize = mtu; // Max allowed recv size.
    }
#ifdef DEBUG_SOCKET_CONN_START
  cerr << connectionId << ": ___________RECEIVED:";
  __eclmpl__printCharStr(msg, msgSize);
  cerr << endl;
#endif
  }

  ASSERT(mtu > strlen("synchronize"));
  strcpy(msg, "synchronize");
  // Now send a synchronize message, meaning that everyone is connected to 
  // everyone else, and everyone is ready to start sending and receiving
  // messages for an application.
  msgSize = (unsigned int)strlen(msg)+1;
  for (unsigned int i = 1; i < numberOfConnections; i++) {
    SocketBasedConnectionInterface::send(msgSize, msg, i);
#ifdef DEBUG_SOCKET_CONN_START
    cerr << connectionId << ": ___________SENT:";
    __eclmpl__printCharStr(msg, msgSize);
    cerr << " to " << i << endl;
#endif
  }
} // End of synchronizeWithSlaves().

void
SocketBasedConnectionInterface::synchronizeWithMaster() {
  unsigned int msgSize = mtu;
  char msg[mtu];
  // Send a message to master saying that we're done with
  // connecting to peer slaves.
  ASSERT(mtu > strlen("done connecting with peer slaves"));
  strcpy(msg, "done connecting with peer slaves");
  msgSize = (unsigned int)strlen(msg)+1;
  unsigned int masterId = 0;
  SocketBasedConnectionInterface::send(msgSize, msg, masterId);

  msgSize = mtu;
  // Receive a "synchronize" message from the master.
  while (SocketBasedConnectionInterface::recv(msgSize, msg, masterId) == false) {
    msgSize = mtu; // Max allowed recv size.
  }
} // End of synchronizeWithMaster().

//inline void ???
void
SocketBasedConnectionInterface::send(const unsigned int &msgSize,  const void * const msg,
				     const unsigned int &destinationId) {
  ASSERT(msgSize <= mtu); // we do not provide fragmentation of messages.
                          // note that msgHeader and msg are actually sent separately, 
                          // although we send them with one call to writev.
  char msgHeader[msgHeaderSize] = "00000";
  __eclmpl__serializeMessageHeader(msgHeader, msgSize);

  struct iovec sendVec[2];
  sendVec[0].iov_base = (char *)msgHeader;
  sendVec[0].iov_len = msgHeaderSize;
  // Remove const for msg.
  // We take take it upon us to assure that msg is not modified during the call to writev.
  sendVec[1].iov_base = const_cast<void *>(msg);
  sendVec[1].iov_len = msgSize;
  
  unsigned int ret = (unsigned int)writev(socket[destinationId]->getSocketFd(), sendVec, 2);
  //ASSERT(ret == msgHeaderSize+msgSize);
} // End of send(...).

//inline bool  ???
bool
SocketBasedConnectionInterface::recv(unsigned int &msgSize, char * const msg, 
				     unsigned int &sourceId) {
#if 0
  cerr << connectionId << ": _____recv________ mtu="<<mtu<<", msgSize="<<msgSize<<", rcvFr=" <<sourceId<<"_____________" << endl;
#endif
  ASSERT(msgSize >= mtu); // msg needs to be able to receive a whole message.
  //ASSERT( (msgSize = socket[sourceId]->readn(msg, msgHeaderSize)) == msgHeaderSize);
  msgSize = socket[sourceId]->readn(msg, msgHeaderSize);
  unsigned int len = static_cast<unsigned int>(atoi(msg));
  ASSERT( (msgSize = socket[sourceId]->readn(msg, (int)len)) == len);
  return true;
} // End of recv(...).

void
SocketBasedConnectionInterface::establishConnectionsWithPeerSlaves(const eclmplConfigFileTable &connTable) {
  for (unsigned int i = 1; i < numberOfConnections; i++) {
    if (i < connectionId) {
      int newSocketFd;
      struct sockaddr_in addr;
      int addrLen = sizeof(addr);
      ASSERT(socket[connectionId]->wAccept(newSocketFd, (struct sockaddr *)&addr, &addrLen) != -1);
      
      // Need to make sure that the socket referred to by i in socket[i] actually
      // corresponds to the correct simulation manager with id i.
      int id = 1;
      while(id < connectionId){
        struct hostent *net = gethostbyname(((connTable.getEntry(id))[0]).c_str());
        struct in_addr entryAddr;
        entryAddr.s_addr = *((long*)(net->h_addr_list[0]));

        // Compare the address of the current entry with the address of the
        // connection that was just received. If it matches, then use the current id.
        if(entryAddr.s_addr == addr.sin_addr.s_addr){
          break;
        }
        else{
          id++;
        }
      }  

      ASSERT( (socket[id] = new eclmplSocket(newSocketFd, addr, (int)addrLen)) != 0);
      setDefaultSocketOptions(id);
    }
    else if (i > connectionId) {
      createNewSocket(i, SOCK_STREAM); // SOCK_STREAM --> TCP
      setDefaultSocketOptions(i);

      // Establish connection with slave i.
      vector<string> args = connTable.getEntry(i);
      ASSERT(args.size() == 3);
      // args[0]==hostname, args[1]==executable, args[2]==portNumber
      string slaveHost = args[0];
      int slavePort = std::stoi(args[2]);
#ifdef DEBUG_SOCKET_CONN_START
      cerr << connectionId << ": Trying to connect to slave " << slaveHost << slavePort << endl;
#endif
      ASSERT(socket[i]->wConnect(slaveHost.c_str(), slavePort) != -1);
#ifdef DEBUG_SOCKET_CONN_START
      cerr << connectionId << ": Connected to slave " << slaveHost << slavePort << endl;
#endif
    }
  }
} // End of establishConnectionsWithPeerSlaves(...).

void 
SocketBasedConnectionInterface::distributeConfigFileTable(const eclmplConfigFileTable &connTable) {
  unsigned int serializedConnTableSize = maxBuf; // Value return parameter.
  char serializedConnTable[maxBuf];
  connTable.serialize(serializedConnTable, serializedConnTableSize);
  ASSERT(serializedConnTableSize <= mtu);

  for (unsigned int i = 1; i < numberOfConnections; i++) {
    SocketBasedConnectionInterface::send(serializedConnTableSize, serializedConnTable, i);
  }
} // End of distributeConfigFileTable(...).
