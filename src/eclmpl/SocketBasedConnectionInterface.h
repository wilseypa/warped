#ifndef SOCKET_BASED_CONNECTION_INTERFACE
#define SOCKET_BASED_CONNECTION_INTERFACE

#include "eclmplCommonInclude.h"
#include "eclmplSocket.h"
#include "eclmplConfigFileTable.h"
#include "eclmplContactInfo.h"

/** The SocketBasedConnectionInterface class.
    
    This class defines the interface for all connection interfaces. A
    connection interface is a lower level communication interface than
    a PhysicalCommunicationLayer. Implementations of this class will
    manage the tasks of establishing connections with peers, sending
    and receiving data on transport layer protocols, and managing and
    storing information necessary to communicate with these peers.
    
    Examples of this class would be a TCP (Transport Control Protocol)
    or UDP (User Datagram Protocol) based connection interface that
    would store all information about open sockets and how to
    communicate using these sockets. Another example would be a VIA
    (Virtual Interface Architecture) based connection interface that
    would store information about a network card device, virtual
    interfaces for communication and ways of managing send and receive
    descriptors. Note, however, that implementations of this class
    should not deal with reliability issues of communication when
    using non-reliable transport layers. These matters should be left
    to the PhysicalCommunicationLayer object using a
    ConnectionInterface.

    The attribute numberOfConnections for this class signifies the
    total number of peers (communicating nodes in a network) that may
    communicate with each other. It is guaranteed that every peer can
    communicate with every other peer after connections has been
    established. The minimum number of connections allowed is two.
*/
class SocketBasedConnectionInterface {
public:
    /**@name Public Class Methods of SocketBasedConnectionInterface. */
    //@{
    
    /// Initialization Constructor
    SocketBasedConnectionInterface(const unsigned int& mtuSize);
    
    // Disable Copy Constructor and Copy Assignment or they will be implicitly defined
    SocketBasedConnectionInterface(const SocketBasedConnectionInterface& original) = delete;
    SocketBasedConnectionInterface& operator=(const SocketBasedConnectionInterface& rhs) = delete;
    
    /// Destructor
    virtual ~SocketBasedConnectionInterface();
    
    /** Establish connections with peers.

        This method must be called prior to using send or recv. A post
        condition of this method is that the attributes connectionId,
        numberOfConnections, connected and mtu will be set. Arguments
        for how to establish connections with peers will be given
        through the input parameters argc and argv. It is quite possible
        that each peer will send messages to distribute or retrieve
        further information, after reading the input arguments, to be
        able to establish all connections.

        All nodes participating in the connection establishment will be
        synchronized after connections has been established so that each
        node is ready to start communicating at the same time.

        Connection establishment is a best effort attempt. A boolean
        value of true will be returned if connection establishment is
        deemed successful, and a boolean value of false will be returned
        otherwise.

        @return True if successful, false otherwise.
        @param argv Startup arguments.
    */
    virtual bool establishConnections(std::vector<std::string>& argv);
    
    /** Tear down connections made with peers.

        This method will close and terminate connections with all
        peers. This is a local operation and will not consider the
        effect this has on peers. It is up to the user of the connection
        interface to make sure that all communication has ended before
        terminating connections.
    */
    virtual void tearDownConnections();
    
    /** Send a message on a connection.

        This method will request a transport layer protocol to send a
        message on a certain connection. The implementing class will
        decide whether to use a blocking or non-blocking call for
        sending a message. It is possible that the message will not be
        sent if using non-reliable communication, and it is up to the
        user to handle such situations. Many transport layers do not
        provide information about whether the message was sent or not,
        so this method will not provide such information either.

        No assumptions are to be made of the format of msg. The message
        will be transmitted as is, without concern about byte order. A
        total of msgSize bytes will be transmitted, regardless of how
        many bytes (including zero) were allocated for msg. It is up to
        the user to make sure that the message size is greater than zero
        bytes and less than the MTU (maximum transfer unit) of the
        connection interface.

        @param msgSize Size of message in bytes.
        @param msg Message to be transmitted.
        @param destinationId Destination of the message.
    */
    virtual void send(const unsigned int& msgSize, const void* const msg, const unsigned int& destinationId);
    
    /** Retrieve a message from transport layer if one is available.

        This method will check if the underlying transport layer has a
        message available for reading in its input buffer. If so, the
        message will be removed from the transport layer's buffer,
        copied into msg, and msgSize set to the MTU of the connection
        interface. The msgSize parameter is a value-return parameter,
        and upon the call to the method must contain a value greater or
        equal to the MTU of the connection interface. There are no
        attempts made to see if msg has been allocated to receive the
        entire message. The parameter sourceId will be filled in with
        the id of the peer that sent the message.

        A boolean value of false will be returned if no message was
        available for retreiving, and a boolean value of true will be
        returned if a message was indeed retrieved and copied into msg.

        @return True if a message was retrieved, false otherwise.
        @param msgSize Value-return parameter for retrieved message.
        @param msg A buffer than should be allocated prior to method call.
        @param sourceId Id of peer that sent message.
    */
    virtual bool recv(unsigned int& msgSize, char* const msg, unsigned int& sourceId);
    
    /** Get the id of this peer.

        @return Id of peer.
    */
    unsigned int getConnectionId() const;

    /** Get the total number of participating peers.

        This method returns the total number of participating peers,
        including this connection interface.

        @return Total number of participating peers.
    */
    unsigned int getNumberOfConnections() const;

    /** Get the maximum message size that can be sent or received.

        This method will return the MTU (Maximum Transfer Unit) for any
        sent or received message.

        @return MTU of this connection interface.
    */
    unsigned int getMTU() const;

    /** Get connection status.

        This method returns true if connections are currently
        established, false if connections are currently not
        established. Note that no verification of connections take
        place. Only a status of whether connections were established
        without being torn down explicitly is given.

        @return Was connections established?
    */
    bool isConnected() const;

    //@} // End of Public Class Methods of SocketBasedConnectionInterface.
    
    /**@name Public Class Attributes of SocketBasedConnectionInterface. */
    //@{
    
    /// Simulation manager file descriptors for sends and receives.
    std::vector<eclmplSocket*> socket;
    
    //@} // End of Public Class Attributes of SocketBasedConnectionInterface.
protected:
    /**@name Protected Class Methods of SocketBasedConnectionInterface. */
    //@{
    
    /** Default Constructor
        
        Made Protected because this class must be initialized with an argument for mtu.
    */
    SocketBasedConnectionInterface();
    
    /** Master establishes connections with slaves.

        This method is called by the master in order to establish
        connections with the slaves.

        @param argv Arguments that the master was started with.
        @param connTable The scanned configuration file.
    */
    virtual void establishConnectionsMasterToSlaves(std::vector<std::string>& argv, const eclmplConfigFileTable& connTable);
    
    /** Slave establishes connections with everyone else.

        This method is called by the slave in order to establish
        connections with everyone else. In most cases this means first
        establishing a connection with the master, receiving information
        about all peers, and then finally establishing connections with
        all these peers.

        @param info Information necessary to initialize slave and begin connection establishment.
    */
    virtual void establishConnectionsFromSlaves(const slaveStartupInfo& info);
    
    /** Parse command line arguments.

        This method will parse command line arguments that were passed
        by the master who forked the slave that is this connection
        interface. A slaveStartupInfo structure will be populated so
        that the slave knows how to contact the master.

        @return Startup info for the slave.
        @param argv Arguments to parse.
     */
    virtual slaveStartupInfo parseCommandLineArguments(std::vector<std::string>& argv);
    
    /** Scan a configuration file and store it.

        This method will scan a configuration file and store it in an
        eclmplConfigFileTable object. A new object will be created and
        returned, so whoever is calling this method is responsible for
        freeing up the memory that was allocated.

        @return A new populated configuration table.
        @param fileName Full path file name of the configuration file.
        @param argsPerEntry Number of arguments per line in the file.
    */
    virtual void scanConfigFile(const std::string& fileName, eclmplConfigFileTable& connTable, int argsPerEntry);
    
    virtual void createSocketPtrVector();
    
    virtual void createNewSocket(const unsigned int& id, const int type);
    
    virtual void setDefaultSocketOptions(const unsigned int& id);
    
    virtual int obtainAndBindUnusedPort(const unsigned int& id);
    
    /** Fork off slave.

        This method is called by the master in order to fork off
        slaves. The total number of slaves that will be forked off is
        numberOfConnections - 1.

        @param argv Arguments that the master was started with.
        @param connTable The scanned configuration file.
        @param masterContactInfo Information about how to contact the master.
        @param id Connection Id of slave to fork off.
    */
    virtual void forkOffSlave(std::vector<std::string>& argv,
                              const eclmplConfigFileTable& connTable,
                              const eclmplContactInfo& masterContactInfo,
                              const unsigned int& id);
    
    /** Create command line arguments for the slave to parse.

        This method will create the command line arguments that will be
        read by a forked off slave in order to successfully establish
        connections.

        @param argv Arguments that the master was started with.
        @param newArgc Number of arguments that the slave will be started with.
        @param newArgv Arguments that the slave will be started with.
        @param configTableEntry Configuration file entry for the slave.
        @param masterContactInfo Information necessary for the slave to contact the master.
        @param id Connection id of slave to be started.
    */
    virtual void createSlaveCommandLineArguments(std::vector<std::string>& argv,
                                                 int& newArgc, char**& newArgv,
                                                 const std::vector<std::string>& configTableEntry,
                                                 const eclmplContactInfo& masterContactInfo,
                                                 const unsigned int& id);
    
    /** Distribute a configuration table to all slaves.

        @param connTable The configuration table.
    */
    virtual void distributeConfigFileTable(const eclmplConfigFileTable& connTable);
    virtual void synchronizeWithSlaves();
    virtual void establishConnectionsWithPeerSlaves(const eclmplConfigFileTable& connTable);
    virtual void synchronizeWithMaster();
    
    
    //@} // End of Protected Class Methods of SocketBasedConnectionInterface.
    
    /**@name Protected Class Attributes of SocketBasedConnectionInterface. */
    //@{

    std::string hostName;
    
    /// Id of this connection interface.
    unsigned int connectionId;

    /// Total number of participating peers.
    unsigned int numberOfConnections;

    /// MTU (Maximum Transfer Unit) for connection interface.
    unsigned int mtu;

    /// Has connections been established?
    bool connected;

    /// Full-path name of remote shell command.
    std::string rshCmd;
    
    //@} // End of Protected Class Attributes of SocketBasedConnectionInterface.
};

#endif
