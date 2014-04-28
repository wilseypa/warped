#ifndef UDP_CONNECTION_INTERFACE_H
#define UDP_CONNECTION_INTERFACE_H

#include "eclmplCommonInclude.h"
#include "SocketBasedConnectionInterface.h"

/// The UDPConnectionInterface class.
class UDPConnectionInterface : public SocketBasedConnectionInterface {
public:
    /**@name Public Class Methods of UDPConnectionInterface. */
    //@{
    
    /// Initialization Constructor
    UDPConnectionInterface(const unsigned int& mtuSize);
    
    // Disable Copy Constructor and Copy Assignment or they will be implicitly defined
    UDPConnectionInterface(const UDPConnectionInterface& original) = delete;
    UDPConnectionInterface& operator=(const UDPConnectionInterface& rhs) = delete;
    
    /// Destructor
    virtual ~UDPConnectionInterface();
    
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
    virtual void send(const unsigned int& msgSize,  const char* const msg, const unsigned int& destinationId);
    
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
    virtual bool recv(unsigned int& msgSize, char* const msg,unsigned int& sourceId);
    
    //@} // End of Public Class Methods of UDPConnectionInterface.
    
    /**@name Public Class Attributes of UDPConnectionInterface. */
    //@{

    std::vector<eclmplSocket*> sendSocket;
    std::vector<eclmplSocket*> recvSocket;
    
    //@} // End of Public Class Attributes of UDPConnectionInterface.
protected:
    /**@name Protected Class Methods of UDPConnectionInterface. */
    //@{
    
    /** Default Constructor
        
        Made Protected because this class must be initialized with an argument for mtu.
    */
    UDPConnectionInterface();
    
    virtual void createAndDistributeRecvSocketVector(eclmplConfigFileTable& udpConnectionTable);
    virtual void createAndReceiveSendSocketVector(eclmplConfigFileTable& udpConnectionTable);
    virtual void closeTcpSockets();
    virtual void createSocketPtrVector(std::vector<eclmplSocket*>& socketVector);
    virtual void obtainAndBindUnusedPorts(std::vector<eclmplSocket*>& socketVector);
};

#endif
