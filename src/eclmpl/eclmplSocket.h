#ifndef ECLMPL_SOCKET_H
#define ECLMPL_SOCKET_H

#include "eclmplCommonInclude.h"

/** The eclmplSocket class.

    This class provides a convenient handle on the network sockets.
*/

class eclmplSocket {
public:
    /// Constructor
    eclmplSocket(const int& type, const int& protocol);
    eclmplSocket(const int& sockFd, const struct sockaddr_in& addr, const int& addrLen);

    /// Destructor
    ~eclmplSocket();

    int wBind(const int& portNr);
    int wListen(const int& maxConnQLen) const;
    int wConnect(const char* const fullyQualifiedServerAddress);
    int wConnect(const char* const serverName, const int& portNr);
    int wAccept();
    int wAccept(int& sockFd, struct sockaddr* addr, int* addrLen) const;
    int wRecv(char* const buf, const int& len, const unsigned int& flags) const;
    int peek(char* const buf, const int& len) const;
    int readn(char* buf, const unsigned int& n) const;
    int wSend(const char* const buf, const int& length, const unsigned int& flags) const;
    int wWrite(const char* const buf, const unsigned int& length) const;
    int wSetsockopt(const int& level, const int& optname, const void* optval, const int optlen);

    void wFD_CLR(fd_set* set);
    bool wFD_ISSET(fd_set* set);
    void wFD_SET(fd_set* set);
    void wFD_ZERO(fd_set* set);

    /// A method which returns the socket handle of an established socket.
    int getSocketFd() const;
    /// A method which returns the port number of an established socket.
    int getPortNumber() const;

protected:

    eclmplSocket() {}

    /// Socket file descriptor to refer to when reading, writing and 'rithmetic.
    int socketFd;
    /// Port number to read to and write from.
    int portNumber;

    /// Remote address information from accept(), or connect().
    struct sockaddr_in remoteAddr;
    /// Remote address length.
    int remoteAddrLen;

    void printError(const int& error, const std::string fName) const;
    struct hostent* wGethostbyname(const char* const name) const;
    int wFcntl(const int& fd, const int& cmd, const long& arg);
};

#endif
