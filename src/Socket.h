#ifndef SOCKET_HH
#define SOCKET_HH


/** The Socket class.

This class provides a convenient handle on the network sockets.

*/

class Socket {
public:
    /// Constructor
    Socket();
    /** Constructor
        @param portNumber TCP port number
    */
    Socket(int portNumber);
    /** Constructor
        @param serverAddress IP Address
    */
    Socket(char* serverAddress);
    /** Constructor
        @param serverName TCP/IP computer name
        @param port TCP port number
    */
    Socket(const char* serverName, int port);

    /// Destructor
    ~Socket();

    /** Bind a socket to a port
        @param pNumber The port number to which we want to bind
    */
    int _bind(int pNumber);

    /** Connect a socket to an address and port
        @param serverAdress IP address:Port e.g. 129.137.9.104:2345
    */
    int _connect(char* serverAddress);
    /** Connect a socket to an address and port
        @param serverName Name of a machine to which to connect.
        @param port Port number on machine to which to connect.
    */
    int _connect(const char* serverName, int port);

    /// Listens on previously defined socketHandle for incoming connections.
    int _listen() const;

    /// Accepts incoming connections.
    Socket* _accept() const;

    /** Reads incoming network traffic
        @param buffer Buffer to which incoming data is written.
        @param length Length of incoming message.
    */
    int _read(char* buffer, int length);
    /** Writes data to the network
        @param buffer Buffer from which data is read.
        @param length Length of outgoing message.
    */
    int _write(const char* buffer, int length);
    /** Writes a line of data to the network
        @param buffer Buffer from which data is read.
        @param length Length of outgoing message.
    */
    int _writeLine(const char* buffer, int length);
    /** Read a message from the network
        @param size Size of message to read.
    */
    char* readMessage(int* size);

    /// A method which returns the socket handle of an established socket.
    int getSocketHandle() const;
    /// A method which returns the port number of an established socket.
    int getPortNumber() const;

    /** Return a specific error message
        @param errorNumber
    */
    static const char* getErrorString(int errorNumber);

    /// Returns the current value of error.
    int getError() const;
    /// Set the socket to non-blocking.
    int setNonBlocking();

    /** Close the socket i.e. no more reads and/or writes.
        @param sockHandle Socket handle to close.
    */
    void myClose(int sockHandle);



protected:
    /** Reads messages from the network.
        @param buffer Buffer in which to write data.
        @param length Length of incoming message.
    */
    int waitAndRead(char* buffer, int length);

    /** I'm not sure how this differs from _write
        @param buffer Buffer from which to send data to the network.
        @param length Length of outgoing message.
    */
    int waitAndWrite(const char* buffer, int length);
    /// Socket handle to refer to when reading, writing and 'rithmetic.
    int socketHandle;
    /// Port number to read to and write from.
    int portNumber;

    /** Used in the _accept method to define a new socket for incoming traffic.
        @param parent Socket class usually "this".
    */
    Socket(const Socket* parent);

    /// Error!
    int error;
    /// non-blocking = true, blocking = false.
    bool nonBlocking;

private:
};

#endif
