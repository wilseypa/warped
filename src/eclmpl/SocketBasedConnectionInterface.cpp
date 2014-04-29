#include "SocketBasedConnectionInterface.h"

#define SEND_SOCKET_BUFFER_SIZE 65536
#define RECV_SOCKET_BUFFER_SIZE 65536

// For stream-based sockets we send a header prepended to each message, containing
// the size in bytes of the message. We need at most 5 bytes (+ '\0') to store
// the serialized length of any message (we'll never send a message with a size
// greater than 99999 bytes).
#define MSG_HEADER_SIZE 6

SocketBasedConnectionInterface::SocketBasedConnectionInterface() : connectionId(0), numberOfConnections(0), mtu(0), connected(false) {
    hostName = std::string();
    rshCmd = std::string();
    
    //Ignoring SIGPIPE so that read and send can handle through EPIPE.
    signal(SIGPIPE, SIG_IGN);
} // End of Default Constructor.

SocketBasedConnectionInterface::SocketBasedConnectionInterface(const unsigned int& mtuSize) : connectionId(0), numberOfConnections(0), mtu(mtuSize), connected(false) {
    hostName = std::string();
    
    char* cmd;
    cmd = getenv("SSH_COMMAND");
    if (cmd != NULL) {
        rshCmd = std::string(cmd);
    }
    else {
#ifdef SSH_COMMAND
        rshCmd = std::string(__local__add_quotes(SSH_COMMAND));
#else
        std::cerr << "Could not set command for starting remote shells." << std::endl
                  << "Since you did not specify a location of a remote shell command" << std::endl
                  << "at configure-time (e.g. ./configure --with-ssh=/usr/bin/ssh)," << std::endl
                  << "you must set the environment variable SSH_COMMAND to a correct" << std::endl
                  << "location of rsh or ssh (e.g. setenv SSH_COMMAND /usr/bin/ssh)" << std::endl;
        abort();
#endif
    }
} // End of Initialization Constructor.

SocketBasedConnectionInterface::~SocketBasedConnectionInterface() {
    for (unsigned int i = 0; i < socket.size(); i++) {
        delete socket[i];
    }
} // End of destructor.

/* The connection scheme is as follows:
   ----------------------------------------------
   b - bind, l - listen, a - accept, f - fork
   c - connect, r - receive, s - send
   fd - socket file descriptor,
   fup - find unused port number
   Port Px is used for communicating with x.
   0 <= x <= n
   ----------------------------------------------
   0. fup            --> P0
      b(P0) l(P0)
      f(1) a(P0)     --> fd[1]            (P0 is added to 1 <= x's argv <= n)
      r(fd[1], "P1") --> P1 is known
      f(2) a(P0)     --> fd[2]
      r(fd[2], "P2") --> P2 is known
      ...
      f(n) a(P0)     --> fd[n]
      r(fd[n], "Pn") --> Pn is known
      s(fd[1 <= x <= n], ConnFileTable)  (All portnumbers are added to table)
      r(fd[1 <= x <= n], "done connecting with peer slaves") (can be received in arbitrary order)
      s(fd[1 <= x <= n], "synchronize")                      (sent in order)
   1. fup            --> P1
      b(P1) l(P1)
      c(P0)          --> fd[0]
      s(fd[0], P1)
      r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
      c(P2)          --> fd[2]
      c(P3)          --> fd[3]
      ...
      c(Pn)          --> fd[n]
      s(fd[2], "go ahead and connect")
      s(fd[0], "done connecting with peer slaves")
      r(fd[0], "synchronize")
   2. fup            --> P2
      b(P2) l(P2)
      c(P0)          --> fd[0]
      s(fd[0], P2)
      r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
      a(P2)          --> fd[1]
      r(fd[1], "go ahead and connect")
      c(P3)          --> fd[3]
      ...
      c(Pn)          --> fd[n]
      s(fd[3], "go ahead and connect")
      s(fd[0], "done connecting with peer slaves")
      r(fd[0], "synchronize")
   3. fup            --> P3
      b(P3) l(P3)
      c(P0)          --> fd[0]
      s(fd[0], P3)
      r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
      a(P3)          --> fd[1]
      a(P3)          --> fd[2]
      r(fd[2], "go ahead and connect")
      c(P4)          --> fd[4]
      ...
      c(Pn)          --> fd[n]
      s(fd[4], "go ahead and connect")
      s(fd[0], "done connecting with peer slaves")
      r(fd[0], "synchronize")
   ......
   n-1.
      fup            --> Pn-1
      b(Pn-1) l(Pn-1)
      c(P0)          --> fd[0]
      s(fd[0], Pn-1)
      r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
      a(Pn-1)          --> fd[1]
      a(Pn-1)          --> fd[2]
      ...
      a(Pn-1)          -->  fd[n-2]
      r(fd[n-2], "go ahead and connect")
      c(Pn)          --> fd[n]
      s(fd[0], "done connecting with peer slaves")
      r(fd[0], "synchronize")
   n. fup            --> Pn
      b(Pn) l(Pn)
      c(P0)          --> fd[0]
      s(fd[0], Pn)
      r(fd[0], ConnFileTable)            (All portnumbers and conn-info of peers are known)
      a(Pn)          --> fd[1]
      a(Pn)          --> fd[2]
      ...
      a(Pn)          --> fd[n-1]
      s(fd[0], "done connecting with peer slaves")
      r(fd[0], "synchronize")
   ----------------------------------------------
   => Everyone is connected to everyone and everyone is synchronized,
      so they are ready to go at about the same time. */

// This is just here to keep the compiler happy... (g++ 2.95.2 that is).
bool SocketBasedConnectionInterface::establishConnections(std::vector<std::string>& argv) {
    ASSERT(argv.size() > 0);

    // We have to check the last argument on the command line to be able
    // to figure out if we are the first communicator started, i.e. the
    // master, or if we have been forked off, i.e. a slave.
    std::string finalParameter(argv[argv.size()-1]);
    if (finalParameter.compare("amslave") == 0) {
        slaveStartupInfo startupInfo;
        startupInfo = parseCommandLineArguments(argv);
        establishConnectionsFromSlaves(startupInfo);
    }
    else {
        eclmplConfigFileTable* connTable = new eclmplConfigFileTable();
        scanConfigFile(std::string("procgroup"), *connTable, 2);
        establishConnectionsMasterToSlaves(argv, *connTable);
        delete connTable;
    }
    connected = true;
    return connected;
} // End of establishConnections(...).

// Master establishes connection with slaves after forking them off.
void SocketBasedConnectionInterface::establishConnectionsMasterToSlaves(std::vector<std::string>& argv, const eclmplConfigFileTable& connTable) {
    ASSERT(connectionId == 0); // Only master executes this method.
    ASSERT(connTable.getNumberOfEntries() == numberOfConnections);
    std::vector<std::string> masterEntry = connTable.getEntry(0);
    ASSERT(masterEntry.size() == 2);

    // Create a vector of numberOfConnections number of socket pointers.
    createSocketPtrVector();

    // Set seed for random number generator.
    srand(93);

    // Create master socket and find an unused portnumber for it, bind and listen to it.
    createNewSocket(connectionId, SOCK_STREAM); // SOCK_STREAM --> TCP
    setDefaultSocketOptions(connectionId);
    std::string myPort = std::to_string(obtainAndBindUnusedPort(connectionId));

    eclmplConfigFileTable localConnTable = connTable;
    // Add portnumber info to localConnTable.
    localConnTable.addToEntry(myPort, (int)connectionId);

    // Start slaves and distribute updated connTable.
    eclmplContactInfo myContactInfo;
    myContactInfo.setProvidedBy(connectionId);
    std::vector<std::string> contactInfo;
    hostName = masterEntry[0];
    contactInfo.push_back(masterEntry[0]);
    contactInfo.push_back(myPort);
    myContactInfo.setContactArguments(contactInfo);
    
    char* msg = new char[mtu];
    for (unsigned int i = 1; i < numberOfConnections; i++) {
        // ******************  modify this function call
        forkOffSlave(argv, connTable, myContactInfo, i);
        int newSocketFd;
        struct sockaddr_in addr;
        int addrLen = sizeof(addr);
        ASSERT(socket[0]->wAccept(newSocketFd, (struct sockaddr*)&addr, &addrLen) != -1);
        ASSERT((socket[i] = new eclmplSocket(newSocketFd, addr, (int)addrLen)) != 0);
        setDefaultSocketOptions(i);
        
        // Receive portnumber where i is listening.
        unsigned int msgSize = mtu;
        while (recv(msgSize, msg, i) == false) {
            msgSize = mtu; // Max allowed recv size.
        }
        std::string slavePortNr = std::to_string(atoi(msg));
        
        // Add portnumber info to localConnTable.
        localConnTable.addToEntry(slavePortNr, (int)i);
    }
    delete[] msg;
    
    // Send serializedConnTable to slaves.
    distributeConfigFileTable(localConnTable);
    
    // Synchronize with slaves.
    synchronizeWithSlaves();
    
    // We're now ready to be used by some application...
} // End of establishConnectionsMasterToSlaves(...).

// Slaves establish connections with master and other slaves.
void SocketBasedConnectionInterface::establishConnectionsFromSlaves(const slaveStartupInfo& info) {
    ASSERT(connectionId != 0); // Only slaves executes this method.
    
    ASSERT(info.configTableEntry.size() == 2);
    hostName = info.configTableEntry[0];

    // Create a vector of numberOfConnections number of socket pointers.
    createSocketPtrVector();

    // Set seed for random number generator.
    srand(93);

    // Create master socket and find an unused portnumber for it, bind and listen to it.
    createNewSocket(connectionId, SOCK_STREAM); // SOCK_STREAM --> TCP
    setDefaultSocketOptions(connectionId);
    std::string myPort = std::to_string(obtainAndBindUnusedPort(connectionId));

    // Establish connection with master.
    ASSERT(info.masterContactInfo.nrOfContactArguments() == 2);
    std::vector<std::string> args = info.masterContactInfo.contactArguments();
    // args[0]==hostname, args[1]==portNumber
    std::string masterHost = args[0];
    int masterPort = std::stoi(args[1]);
    unsigned int masterId = 0;
    createNewSocket(masterId, SOCK_STREAM); // SOCK_STREAM --> TCP
    setDefaultSocketOptions(masterId);
    
    ASSERT(socket[masterId]->wConnect(masterHost.c_str(), masterPort) != -1);
    
    // Send a message to master containing our portnumber.
    unsigned int msgSize = mtu;
    char* msg = new char[mtu];
    ASSERT(mtu > static_cast<unsigned int>(strlen(myPort.c_str())+1));
    strcpy(msg, myPort.c_str());
    msgSize = static_cast<unsigned int>(strlen(myPort.c_str())+1);
    send(msgSize, msg, masterId);
    
    eclmplConfigFileTable connTable;
    // Receive serializedConnTable from master.
    msgSize = mtu;
    while (recv(msgSize, msg, masterId) == false) {
        msgSize = mtu; // Max allowed recv size.
    }
    ASSERT(masterId == 0); // masterId is a return parameter from recv.
    eclmplConfigFileTable localConnTable;
    localConnTable.deserialize(msg);
    
    // Establish connections with other slaves.
    establishConnectionsWithPeerSlaves(localConnTable);
    
    // Let master know we're done with connection establishment and Wait
    // for synchronize message from master.
    synchronizeWithMaster();
    
    delete[] msg;
} // End of establishConnectionsFromSlaves(...).

void SocketBasedConnectionInterface::tearDownConnections() {
    // RK_NOTE_7-4-10: This is not working properly. In some cases, the recv function
    // gets the teardown message and tries to deserialize it, causing problems. I am not
    // sure of a good way to ensure that the master is the last one to close down.
    /*
    // The master waits until all slaves have closed connections.
    unsigned int msgSize = mtu;
    char* msg = new char[mtu];
    
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
        std::string recvMsg(msg);
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
    }
    delete[] msg;
    */
    
    debug::debugout << "Begin tearing down connections for: " << connectionId;
    for (unsigned int i = 0; i < socket.size(); i++) {
        debug::debugout << " ... " << i;
        shutdown(socket[i]->getSocketFd(),2);
    }
    debug::debugout << std::endl << "Finished tearing down connections for: " << connectionId <<
                    std::endl;
}

void SocketBasedConnectionInterface::send(const unsigned int& msgSize,
                                          const void* const msg,
                                          const unsigned int& destinationId) {
    ASSERT(msgSize <= mtu); // we do not provide fragmentation of messages.
    // note that msgHeader and msg are actually sent separately,
    // although we send them with one call to writev.
    char msgHeader[MSG_HEADER_SIZE] = "00000";
    // Serialize Message Header
    int a = 0;
    unsigned int tempSize = msgSize;
    bool start = false;
    for (unsigned int i = 10000; i > 0; i /= 10) {
        if (tempSize/i > 0) {
            start = true;
        }
        if (start) {
            msgHeader[a] = 48+tempSize/i;
            tempSize %= i;
            a++;
        }
    }
    msgHeader[a] = '\0';

    struct iovec sendVec[2];
    sendVec[0].iov_base = (char*)msgHeader;
    sendVec[0].iov_len = MSG_HEADER_SIZE;
    // Remove const for msg.
    // We take take it upon us to assure that msg is not modified during the call to writev.
    sendVec[1].iov_base = const_cast<void*>(msg);
    sendVec[1].iov_len = msgSize;

    unsigned int ret = writev(socket[destinationId]->getSocketFd(), sendVec, 2);
    ASSERT(ret == MSG_HEADER_SIZE+msgSize);
} // End of send(...).

bool SocketBasedConnectionInterface::recv(unsigned int& msgSize,
                                          char* const msg,
                                          unsigned int& sourceId) {
    ASSERT(msgSize >= mtu); // msg needs to be able to receive a whole message.
    //ASSERT( (msgSize = socket[sourceId]->readn(msg, MSG_HEADER_SIZE)) == MSG_HEADER_SIZE);
    msgSize = socket[sourceId]->readn(msg, MSG_HEADER_SIZE);
    unsigned int len = static_cast<unsigned int>(atoi(msg));
    ASSERT((msgSize = socket[sourceId]->readn(msg, (int)len)) == len);
    return true;
} // End of recv(...).

unsigned int SocketBasedConnectionInterface::getConnectionId() const {
    return connectionId;
}

unsigned int SocketBasedConnectionInterface::getNumberOfConnections() const {
    return numberOfConnections;
}

unsigned int SocketBasedConnectionInterface::getMTU() const {
    return mtu;
}

bool SocketBasedConnectionInterface::isConnected() const {
    return connected;
}

slaveStartupInfo SocketBasedConnectionInterface::parseCommandLineArguments(std::vector<std::string>& argv) {
    ASSERT(argv.size() >= 8);
    slaveStartupInfo info;
    int currArg;
    unsigned int tmp;

    // argv on form:
    // hostname fullPathNameOfExecutable <oldArgv> <optional arguments>
    // masterContactInfo numberOfConnections connectionId amslave
    connectionId = (unsigned int)std::stoul(argv[argv.size()-2]);
    numberOfConnections = (unsigned int)std::stoul(argv[argv.size()-3]);
    tmp = (unsigned int)std::stoul(argv[argv.size()-4]); // nrOfMasterContactArgs
    
    std::vector<std::string> contactArgs;
    currArg = 5;
    for (unsigned int i = 0; i < tmp; i++) {
        contactArgs.push_back(std::string(argv[argv.size()-currArg]));
        currArg++;
    }
    info.masterContactInfo.setContactArguments(contactArgs);
    
    tmp = (unsigned int)std::stoul(argv[argv.size()-currArg]); // Intended for...
    ASSERT(tmp == connectionId);
    info.masterContactInfo.setIntendedFor(tmp);
    currArg++;

    tmp = (unsigned int)std::stoul(argv[argv.size()-currArg]); // Provided by...
    info.masterContactInfo.setProvidedBy(tmp);
    currArg++;

    tmp = (unsigned int)std::stoul(argv[argv.size()-currArg]); // Number of optional arguments...
    currArg++;
    for (unsigned int i = 0; i < tmp; i++) {
        info.configTableEntry.push_back(std::string(argv[argv.size()-currArg]));
        currArg++;
    }
    return info;
} // End of parseCommandLineArguments(...).

// We assume the default number of arguments for each table entry is
// two (which should be enough for most comm-layers). These two
// arguments are, however, required for every comm-layer, and they
// are:
// 1. nodename where process will execute
// 2. full pathname of executable to execute
void SocketBasedConnectionInterface::scanConfigFile(const std::string& fileName, eclmplConfigFileTable& connTable, int argsPerEntry) {
    std::ifstream infile;
    std::vector<std::string> tableEntry;

    char* pwd = getcwd(NULL, 65535);
    ASSERT(pwd != NULL);
    std::string fullFilePathName(pwd);
    // using free instead of delete cause pwd was allocated with malloc
    // and not new ...
    free(pwd);

    fullFilePathName = fullFilePathName + "/" + fileName;
    infile.open(fullFilePathName, std::ios::in);
    ASSERT(infile.good() == true);

    connectionId = 0;
    numberOfConnections = 0;
    while (infile.good()) {
        std::string arg1 = std::string();
        infile >> arg1;
        
        if(arg1.length() != 0) {
            tableEntry.clear();
            tableEntry.push_back(arg1);
            for (int i = 1; i < argsPerEntry; i++) {
                std::string arg2 = std::string();
                infile >> arg2;
                tableEntry.push_back(arg2);
            }
            connTable.addEntry(tableEntry);
            numberOfConnections++;
        }
    }
    
    infile.close();
} // End of scanConfigFile(...).

void SocketBasedConnectionInterface::createSocketPtrVector() {
    ASSERT(numberOfConnections > 0);
    socket.resize(numberOfConnections);
    for (unsigned int i = 0; i < numberOfConnections; i++) {
        socket[i] = NULL;
    }
} // End of createSocketVector()

void SocketBasedConnectionInterface::createNewSocket(const unsigned int& id, const int type) {
    int protocol = 0;
    socket[id] = new eclmplSocket(type, protocol);
    ASSERT(socket[id] != NULL);
} // End of createNewSocket(...).

void SocketBasedConnectionInterface::setDefaultSocketOptions(const unsigned int& id) {
    int i = 0;
    const int sendSocketBufferSize = SEND_SOCKET_BUFFER_SIZE;
    const int recvSocketBufferSize = RECV_SOCKET_BUFFER_SIZE;
    
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
    i = socket[id]->wSetsockopt(SOL_SOCKET, SO_LINGER, (struct linger*)&lingerOptions, sizeof(struct linger));
    ASSERT(i != -1);

    // Set the socket buffer sizes.
    ASSERT(sendSocketBufferSize >= mtu && recvSocketBufferSize >= mtu);
    
    i = socket[id]->wSetsockopt(SOL_SOCKET, SO_SNDBUF, (int*)&sendSocketBufferSize, sizeof(int));
    ASSERT(i != -1);
    i = socket[id]->wSetsockopt(SOL_SOCKET, SO_RCVBUF, (int*)&recvSocketBufferSize, sizeof(int));
    ASSERT(i != -1);
} // End of setDefaultSocketOptions(...).


int SocketBasedConnectionInterface::obtainAndBindUnusedPort(const unsigned int& id) {
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

void SocketBasedConnectionInterface::forkOffSlave(std::vector<std::string>& argv,
                                                  const eclmplConfigFileTable& connTable,
                                                  const eclmplContactInfo& masterContactInfo,
                                                  const unsigned int& id) {
    ASSERT(id >= 1 && id < numberOfConnections);

    std::vector<std::string> slaveEntry = connTable.getEntry(id);
    eclmplContactInfo masterInfo = masterContactInfo;
    masterInfo.setIntendedFor(id);
    if (fork() == 0) {
        int newArgc;
        char** newArgv = NULL;
        createSlaveCommandLineArguments(argv, newArgc, newArgv, slaveEntry, masterInfo, id);
        
        // Slave.
        // execv will subsitute the running child program with a new program.
        // The new program is SSH_COMMAND followed by a set of arguments that will be
        // the new argv for the executing program. The set of new arguments must
        // end with a NULL pointer.
        if (execv(rshCmd.c_str(), newArgv) == -1) {
            perror("ConnectionInterface::forkOffSlaves - execv");
            std::cerr << "Could not start slave (" << id << ")" << std::endl;
            abort();
        }
    } else {
        // Master.
        // We need to deallocate newArgv so it's free to use for next slave.
        //for (int j = 0; j < newArgc; j++) {
        //  delete newArgv[j];
        //}
        //delete[] newArgv;
    }
} // End of forkOffSlave(...).

// Assumptions:
// configTableEntry should contain at a minimum the following information -
// 1. hostname (nodename) of the computer where the slave will execute
// 2. full pathname of the executable that the slave will execute
// It should be contained in the following format -
// hostname fullPathNameOfExecutable <optional arguments>
// The new argv will look like:
// hostname fullPathNameOfExecutable <oldArgv> <optional arguments>
// masterContactInfo numberOfConnections connectionId amslave NULL
void SocketBasedConnectionInterface::createSlaveCommandLineArguments(std::vector<std::string>& argv,
                                                                     int& newArgc, char**& newArgv,
                                                                     const std::vector<std::string>& configTableEntry,
                                                                     const eclmplContactInfo& masterContactInfo,
                                                                     const unsigned int& id) {
    ASSERT(configTableEntry.size() >= 2);
    ASSERT(masterContactInfo.nrOfContactArguments() >= 1);
    std::string tempString;
    newArgc = 2 + argv.size() + configTableEntry.size() + 2 +
              masterContactInfo.nrOfContactArguments() + 1 + 4 + 1;
    int currArg;

    newArgv = new char* [newArgc];

    // newArgv[0] = hostname of slave
    newArgv[0] = new char [configTableEntry[0].length()+1];
    strncpy(newArgv[0], configTableEntry[0].c_str(), configTableEntry[0].length()+1);

    // newArgv[1] = full-path filename of executable for slave
    newArgv[1] = new char [configTableEntry[0].length()+1];
    strncpy(newArgv[1], configTableEntry[0].c_str(), configTableEntry[0].length()+1);

    // newArgv[2] = argv[0]
    // . . .
    // newArgv[2+argc-1] = argv[argc-1]
    for (unsigned int i = 0; i < argv.size(); i++) {
        newArgv[2+i] = new char [argv[i].length()+1];
        strncpy(newArgv[2+i], argv[i].c_str(), argv[i].length()+1);
    }

    currArg = 2+argv.size();
    // <optional arguments>
    //for (unsigned int i = 0; i < configTableEntry.size(); i++) {
    for (int i = configTableEntry.size()-1; i >= 0; i--) {
        //currArg = 2+*argc+i;
        newArgv[currArg] = new char [configTableEntry[i].length()+1];
        strncpy(newArgv[currArg], configTableEntry[i].c_str(), configTableEntry[i].length()+1);
        currArg++;
    }
    //currArg = 2+*argc+configTableEntry.size();

    // newArgv[currArg] =  \"numberOfOptionalArguments\"
    tempString = std::to_string(configTableEntry.size());
    newArgv[currArg] = new char [tempString.length()+1];
    strncpy(newArgv[currArg], tempString.c_str(), tempString.length()+1);
    currArg++;

    // newArgv[currArg] =  \"masterContactInfo.providedBy\"
    tempString = std::to_string(masterContactInfo.providedBy());
    newArgv[currArg] = new char [tempString.length()+1];
    strncpy(newArgv[currArg], tempString.c_str(), tempString.length()+1);
    currArg++;

    // newArgv[currArg] =  \"masterContactInfo.intendedFor\"
    tempString = std::to_string(masterContactInfo.getIntendedFor());
    newArgv[currArg] = new char [tempString.length()+1];
    strncpy(newArgv[currArg], tempString.c_str(), tempString.length()+1);
    currArg++;

    std::vector<std::string> contactArgs = masterContactInfo.contactArguments();
    for (int i = contactArgs.size()-1; i >= 0; i--) {
        // newArgv[currArg] =  \"masterContactInfo.contactArgs[i]\"
        newArgv[currArg] = new char [contactArgs[i].length()+1];
        strncpy(newArgv[currArg], contactArgs[i].c_str(), contactArgs[i].length()+1);
        currArg++;
    }

    // newArgv[currArg] =  \"masterContactInfo.nrOfContactArguments\"
    tempString = std::to_string(contactArgs.size());
    newArgv[currArg] = new char [tempString.length()+1];
    strncpy(newArgv[currArg], tempString.c_str(), tempString.length()+1);
    currArg++;

    // newArgv[currArg] = \"numberOfConnections\"
    tempString = std::to_string(numberOfConnections);
    newArgv[currArg] = new char [tempString.length()+1];
    strncpy(newArgv[currArg], tempString.c_str(), tempString.length()+1);
    currArg++;

    // newArgv[currArg] = \"slaveConnectionId\"
    tempString = std::to_string(id);
    newArgv[currArg] = new char [tempString.length()+1];
    strncpy(newArgv[currArg], tempString.c_str(), tempString.length()+1);
    currArg++;

    // newArgv[currArg] = "amslave"
    newArgv[currArg] = new char [8];
    strncpy(newArgv[currArg], "amslave", 8);
    currArg++;

    // newArgv[currArg] = NULL            --- execl wants argument list to end with NULL
    newArgv[currArg] = NULL;
} // End of createSlaveCommandLineArguments(...).

void SocketBasedConnectionInterface::distributeConfigFileTable(const eclmplConfigFileTable& connTable) {
    unsigned int serializedConnTableSize = MAXBUF; // Value return parameter.
    char serializedConnTable[MAXBUF];
    
    connTable.serialize(serializedConnTable, serializedConnTableSize);
    ASSERT(serializedConnTableSize <= mtu);

    for (unsigned int i = 1; i < numberOfConnections; i++) {
        send(serializedConnTableSize, serializedConnTable, i);
    }
} // End of distributeConfigFileTable(...).

void SocketBasedConnectionInterface::synchronizeWithSlaves() {
    unsigned int msgSize = mtu;
    char* msg = new char[mtu];
    
    // Just wait and receive one message from each slave.
    // The arrival of a message means that the slave is done connecting with its peers.
    // The contents of the message doesn't really matter and we don't bother to check it.
    for (unsigned int i = 1; i < numberOfConnections; i++) {
        msgSize = mtu;
        while (recv(msgSize, msg, i) == false) {
            msgSize = mtu; // Max allowed recv size.
        }
    }

    ASSERT(mtu > strlen("synchronize"));
    strncpy(msg, "synchronize", mtu);
    
    // Now send a synchronize message, meaning that everyone is connected to
    // everyone else, and everyone is ready to start sending and receiving
    // messages for an application.
    msgSize = (unsigned int)strlen(msg)+1;
    for (unsigned int i = 1; i < numberOfConnections; i++) {
        send(msgSize, msg, i);
    }
    
    delete[] msg;
} // End of synchronizeWithSlaves().

void SocketBasedConnectionInterface::establishConnectionsWithPeerSlaves(const eclmplConfigFileTable& connTable) {
    for (unsigned int i = 1; i < numberOfConnections; i++) {
        if (i < connectionId) {
            int newSocketFd;
            struct sockaddr_in addr;
            int addrLen = sizeof(addr);
            ASSERT(socket[connectionId]->wAccept(newSocketFd, (struct sockaddr*)&addr, &addrLen) != -1);

            // Need to make sure that the socket referred to by i in socket[i] actually
            // corresponds to the correct simulation manager with id i.
            unsigned int id = 1;
            while (id < connectionId) {
                struct hostent* net = gethostbyname(((connTable.getEntry(id))[0]).c_str());
                struct in_addr entryAddr;
                entryAddr.s_addr = *((long*)(net->h_addr_list[0]));

                // Compare the address of the current entry with the address of the
                // connection that was just received. If it matches, then use the current id.
                if (entryAddr.s_addr == addr.sin_addr.s_addr) {
                    break;
                } else {
                    id++;
                }
            }

            ASSERT((socket[id] = new eclmplSocket(newSocketFd, addr, (int)addrLen)) != 0);
            setDefaultSocketOptions(id);
        } else if (i > connectionId) {
            createNewSocket(i, SOCK_STREAM); // SOCK_STREAM --> TCP
            setDefaultSocketOptions(i);

            // Establish connection with slave i.
            std::vector<std::string> args = connTable.getEntry(i);
            ASSERT(args.size() == 3);
            // args[0]==hostname, args[1]==executable, args[2]==portNumber
            std::string slaveHost = args[0];
            int slavePort = std::stoi(args[2]);
            
            ASSERT(socket[i]->wConnect(slaveHost.c_str(), slavePort) != -1);
        }
    }
} // End of establishConnectionsWithPeerSlaves(...).

void SocketBasedConnectionInterface::synchronizeWithMaster() {
    unsigned int msgSize = mtu;
    char* msg = new char[mtu];
    
    // Send a message to master saying that we're done with
    // connecting to peer slaves.
    ASSERT(mtu > strlen("done connecting with peer slaves"));
    strncpy(msg, "done connecting with peer slaves", mtu);
    msgSize = (unsigned int)strlen(msg)+1;
    unsigned int masterId = 0;
    send(msgSize, msg, masterId);

    msgSize = mtu;
    // Receive a "synchronize" message from the master.
    while (recv(msgSize, msg, masterId) == false) {
        msgSize = mtu; // Max allowed recv size.
    }
    
    delete[] msg;
} // End of synchronizeWithMaster().

#undef SEND_SOCKET_BUFFER_SIZE
#undef RECV_SOCKET_BUFFER_SIZE
#undef MSG_HEADER_SIZE
