#include <arpa/inet.h>                  // for inet_addr
#include <errno.h>                      // for EBADF, EFAULT, ENOTSOCK, etc
#include <netdb.h>                      // for gethostbyname, etc
#include <stdlib.h>                     // for atoi
#include <string.h>                     // for NULL, bzero, strchr, strlen, etc
#include <sys/socket.h>                 // for socklen_t, AF_INET, accept, etc
#include <unistd.h>                     // for close, read, write
#include <iostream>                     // for operator<<, basic_ostream, etc

#include <WarpedConfig.h>               // for USE_SOCKLEN_T
#include "eclmpl/eclmplCommonInclude.h"  // for ECLMPL_ASSERT
#include "eclmplSocket.h"

#define IP_VER AF_INET // AF_INET for IPv4, AF_INET6 for IPv6

// For connected sockets, how many connection requests will be queued?
const int socketConnectionQLen = 10;

const int maxBuf = 65535;

#ifndef MSG_WAITALL
#define MSG_WAITALL 0x40
#endif

//#define ECLMPL_DEBUG_SOCKET(x) x
#define ECLMPL_DEBUG_SOCKET(x)

// type == SOCK_STREAM, SOCK_DGRAM
eclmplSocket::eclmplSocket(const int& type, const int& protocol) {
    // Open a TCP socket (an Internet stream socket).
    ECLMPL_ASSERT((socketFd = socket(IP_VER, type, protocol)) != -1);
    portNumber  = -1;
    //remoteAddr = NULL;
    remoteAddrLen = 0;//NULL;
} // End of default constructor.

eclmplSocket::eclmplSocket(const int& sockFd, const struct sockaddr_in& addr, const int& addrLen) {
    socketFd = sockFd;
    bzero(&remoteAddr, sizeof(remoteAddr));
    remoteAddr.sin_family = addr.sin_family;;
    remoteAddr.sin_addr.s_addr = addr.sin_addr.s_addr;
    remoteAddr.sin_port = addr.sin_port;
    remoteAddrLen = addrLen; //*remoteAddrLen;
    portNumber = ntohs(remoteAddr.sin_port);
} // End of Constructor.

eclmplSocket::~eclmplSocket() {
    // shutdown(socketFd, 2);
    //std::cout << socketFd << std::endl;
    close(socketFd);
    //delete remoteAddr;
} // End of destructor.

// int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
int
eclmplSocket::wBind(const int& portNr) {
    int retVal;
    struct sockaddr_in serverAddr;
    portNumber = portNr;
    // Bind the socketFd to portNumber.
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = IP_VER;
    // INADDR_ANY --> ip wildcard. Filled in by kernel. See Stevens, p. 92.
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(portNumber);

    retVal = bind(socketFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) { printError(errno, "bind"); })
    return retVal;
} // End of wBind(...).

//int listen(int s, int backlog);
int
eclmplSocket::wListen(const int& maxConnQLen) const {
    int retVal;
    retVal = listen(socketFd, maxConnQLen);
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) { printError(errno, "listen"); })
    return retVal;
} // End of wListen(...).

int
eclmplSocket::wConnect(const char* const fullyQualifiedServerAddress) {
    int retVal;
    //struct sockaddr_in serverAddr;
    char* colonPosition;

    //Create a non constant copy of the fully qualified address
    char* working = strndup(fullyQualifiedServerAddress, strlen(fullyQualifiedServerAddress));
    if ((colonPosition = strchr(working, ':')) == NULL) {
        return -1;
    }

    portNumber    = atoi((colonPosition + 1));
    //Split the addres from the port
    *colonPosition = '\0';

    if (inet_addr(working) == (unsigned) -1) {
        return -1;
    }

    //bzero(&serverAddr, sizeof(serverAddr));
    //serverAddr.sin_family = IP_VER;
    //serverAddr.sin_addr.s_addr = inet_addr(fullyQualifiedServerAddress);
    //serverAddr.sin_port = htons(portNumber);

    bzero(&remoteAddr, sizeof(remoteAddr));
    remoteAddr.sin_family = IP_VER;
    remoteAddr.sin_addr.s_addr = inet_addr(fullyQualifiedServerAddress);
    remoteAddr.sin_port = htons(portNumber);

    *colonPosition = ':';

    retVal = connect(socketFd, (struct sockaddr*) &remoteAddr, sizeof(remoteAddr));
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) { printError(errno, "connect"); })

    return retVal;
} // End of wConnect(...).

int
eclmplSocket::wConnect(const char* const serverName, const int& portNr) {
    int retVal;
    struct hostent* serverInfo = NULL;
    //struct sockaddr_in serverAddr;

    portNumber = portNr;
    ECLMPL_ASSERT((serverInfo = wGethostbyname(serverName)) != NULL);

    //bzero((char *) &serverAddr, sizeof(serverAddr));
    //serverAddr.sin_family      = AF_INET;
    //serverAddr.sin_addr.s_addr = *((long *) serverInfo->h_addr_list[0]);
    //serverAddr.sin_port        = htons(portNumber);

    //if ( (retVal = connect(socketFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) == -1) {

    bzero((char*) &remoteAddr, sizeof(remoteAddr));
    remoteAddr.sin_family      = AF_INET;
    remoteAddr.sin_addr.s_addr = *((long*) serverInfo->h_addr_list[0]);
    remoteAddr.sin_port        = htons(portNumber);
    remoteAddrLen = sizeof(remoteAddr);

    retVal = connect(socketFd, (struct sockaddr*) &remoteAddr, remoteAddrLen);
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) { printError(errno, "connect"); })

    return retVal;
}

//struct hostent *gethostbyname(const char *name);
struct hostent*
eclmplSocket::wGethostbyname(const char* const name) const {
    struct hostent* retVal;
    retVal = gethostbyname(name);
    ECLMPL_DEBUG_SOCKET(if (retVal == NULL) { printError(h_errno, "gethostbyname"); })
    return retVal;
} // End of w(...).

int
eclmplSocket::wAccept() {
    remoteAddrLen = sizeof(remoteAddr);
#ifdef USE_SOCKLEN_T
    socklen_t len = (socklen_t)remoteAddrLen;
#else
    int len = remoteAddrLen;
#endif
    socketFd = accept(socketFd, (struct sockaddr*)&remoteAddr, &len);
    remoteAddrLen = (int)len;
    ECLMPL_DEBUG_SOCKET(if (socketFd == -1) { printError(errno, "accept"); })
    return socketFd;
} // End of wAccept(...).

int
eclmplSocket::wAccept(int& sockFd, struct sockaddr* addr, int* addrLen) const {
    *addrLen = sizeof(struct sockaddr);
#ifdef USE_SOCKLEN_T
    socklen_t* len = (socklen_t*)addrLen;
#else
    int* len = addrLen;
#endif
    sockFd = accept(socketFd, addr, len);
    *addrLen = (int)*len;
    ECLMPL_DEBUG_SOCKET(if (sockFd == -1) { printError(errno, "accept"); })
    return sockFd;
} // End of wAccept(...).

#if 0
// THIS WAS REPLACED....
//int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int
eclmplSocket::wAccept(const int& s, struct sockaddr* addr, socklen_t* addrLen) const {
    int retVal;
    retVal = accept(s, addr, addrLen);
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) { printError(errno, "accept"); })
    return retVal;
} // End of wAccept(...).
#endif

int
eclmplSocket::wRecv(char* const buf, const int& len, const unsigned int& flags) const {
    int retVal;
    retVal = recvfrom(socketFd, buf, len, flags|MSG_WAITALL, NULL, NULL);
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) {
    printError(errno, "recv");
        std::cerr << "\nsocketFD=" << socketFd << std::endl;
    })
    return retVal;
} // End of wRecv(...).

int
eclmplSocket::peek(char* const buf, const int& len) const {
    int retVal;
    retVal = recvfrom(socketFd, buf, len, MSG_PEEK|MSG_DONTWAIT, NULL, NULL);
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) {
    if (errno != EAGAIN && errno != EINTR) {
            printError(errno, "peek(recvfrom - MSG_PEEK|MSG_DONTWAIT)");
        }
    })
    return retVal;
} // End of peek(...).

int
eclmplSocket::readn(char* buf, const unsigned int& n) const {
    unsigned int dataRead = 0;
    int result = 0;

    while (dataRead < n) {
        if ((result = read(socketFd, (buf+dataRead), (n - dataRead))) <= 0) {
            if (errno == EINTR) {
                dataRead = 0;
            } else {
                ECLMPL_DEBUG_SOCKET(printError(errno, "read");)
                return (-1);
            }
        } else {
            dataRead += result;
        }
#if 0
        if (dataRead < n) {
            std::cerr << "____wRecv____ READ SO FAR (dataRead="<<dataRead<<",n="<<n<<")=\"";
            for (unsigned int i = 0; i < dataRead; i++) {
                if (buf[i] == '\0')
                { fprintf(stderr, "'\\0'"); }
                else if (buf[i] == '\n')
                { fprintf(stderr, "'\\n'"); }
                else if (buf[i] >= 32 && buf[i] <= 126)
                { fprintf(stderr, "%c", buf[i]); }
                else
                { fprintf(stderr, "'%d'", (int)buf[i]); }
            }
            std::cerr << "\" (length=" << strlen(buf)+1 << ")" << std::endl;
        }
#endif
    }
#if 0
    std::cerr << "SOCKET_READN:\"";
    for (int i = 0; i < dataRead; i++) {
        if (buf[i] == '\0')
        { fprintf(stderr, "'\\0'"); }
        else if (buf[i] == '\n')
        { fprintf(stderr, "'\\n'"); }
        else if (buf[i] >= 32 && buf[i] <= 126)
        { fprintf(stderr, "%c", buf[i]); }
        else
        { fprintf(stderr, "'%d'", (int)buf[i]); }
    }
    std::cerr << "\"" << std::endl;
#endif
    return dataRead;
} // End of readn().

int
eclmplSocket::wSend(const char* const buf, const int& length, const unsigned int& flags) const {
    int retVal;
    retVal = sendto(socketFd, buf, length, flags, NULL, 0);
    ECLMPL_DEBUG_SOCKET(if (retVal == -1) { printError(errno, "send"); })
    return retVal;
} // End of wSend(...).

int
eclmplSocket:: wWrite(const char* const buf, const unsigned int& length) const {
    unsigned int dataWritten = 0;
    int result = 0;

    while (dataWritten < length) {
        if ((result = write(socketFd, (buf + dataWritten), (length - dataWritten))) == -1) {
            if (errno == EINTR) {
                dataWritten = 0;
            } else {
                ECLMPL_DEBUG_SOCKET(printError(errno, "write");)
                return (-1);
            }
        } else {
            dataWritten += result;
        }
    }
    return dataWritten;
} // End of wWrite(...).

int
eclmplSocket::wSetsockopt(const int& level, const int& optname,
                          const void* optval, const int optlen) {
    int retVal;
#ifdef USE_SOCKLEN_T
    socklen_t len = (socklen_t)optlen;
#else
#ifdef USE_SIZE_T
    size_t len = (size_t)optlen;
#else
    int len = optlen;
#endif
#endif

#ifdef USE_CHAR_FOR_VOID
    retVal = setsockopt(socketFd, level, optname, (const char*)optval, len);
#else
    retVal = setsockopt(socketFd, level, optname, optval, len);
#endif

    ECLMPL_DEBUG_SOCKET(if (retVal == -1) { printError(errno, "setsockopt"); })
    return retVal;
} // End of wSetsockopt(...).

void
eclmplSocket::wFD_CLR(fd_set* set) {
    FD_CLR(socketFd, set);
} // End of wFD_CLR(...).

bool
eclmplSocket::wFD_ISSET(fd_set* set) {
    return FD_ISSET(socketFd, set);
} // End of wFD_ISSET(...).

void
eclmplSocket::wFD_SET(fd_set* set) {
    FD_SET(socketFd, set);
} // End of wFD_SET(...).

void
eclmplSocket::wFD_ZERO(fd_set* set) {
    FD_ZERO(set);
} // End of wFD_ZERO(...).

int
eclmplSocket::getSocketFd() const {
    return socketFd;
} // End of getSocketFd().

int
eclmplSocket::getPortNumber() const {
    return portNumber;
} // End of getPortNumber().

void
eclmplSocket::printError(const int& error, const std::string fName) const {
    std::string defaultError = "Unknown.";
    std::string msg;

    std::cerr << "ERROR occured for call to \"" << fName << "\" - Cause: ";
    if (fName == "socket") {
        switch (error) {
        case EPROTONOSUPPORT:
            msg = "The protocol type or the specified protocol is not supported within this domain.";
            break;
        case ENFILE:
            msg = "Not enough kernel memory to allocate a new socket structure.";
            break;
        case EMFILE:
            msg = "Process file table overflow.";
            break;
        case EACCES:
            msg = "Permission to create a socket of the specified type and/or protocol is denied.";
            break;
        case ENOBUFS: // ENOBUFS || ENOMEM
        case ENOMEM:
            msg = "Insufficient memory is available.  The socket cannot be created until "
                  "sufficient resources are freed.";
            break;
        case EINVAL:
            msg = "Unknown protocol, or protocol family not available.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of if (fname == "socket").
    else if (fName == "bind") {
        switch (error) {
        case EBADF:
            msg = "sockfd is not a valid descriptor.";
            break;
        case EINVAL:
            msg = "The socket is already bound to an address.";
            break;
        case EACCES:
            msg = "The address is protected, and the user is not the super-user.";
            break;
        case ENOTSOCK:
            msg = "Argument is a descriptor for a file, not a socket.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of else if (fName == "bind").
    else if (fName == "listen") {
        switch (error) {
        case EBADF:
            msg = "The argument sockfd is not a valid descriptor";
            break;
        case ENOTSOCK:
            msg = "The argument sockfd is not a socket.";
            break;
        case EOPNOTSUPP:
            msg = "The socket is not of a type that supports the listen operation.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of else if (fName == "listen").
    else if (fName == "connect") {
        switch (error) {
        case EBADF:
            msg = "Bad descriptor.";
            break;
        case EFAULT:
            msg = "The socket structure address is outside the user's address space.";
            break;
        case ENOTSOCK:
            msg = "The descriptor is not associated with a socket.";
            break;
        case EISCONN:
            msg = "The socket is already connected.";
            break;
        case ECONNREFUSED:
            msg = "Connection refused at server.";
            break;
        case ETIMEDOUT:
            msg = "Timeout while attempting connection.";
            break;
        case ENETUNREACH:
            msg = "Network is unreachable.";
            break;
        case EADDRINUSE:
            msg = "Address is already in use.";
            break;
        case EINPROGRESS:
            msg = "The  socket is non-blocking and the connection cannot be completed "
                  "immediately.  It is possible to select(2) or poll(2) for completion "
                  "by selecting the socket for writing. After select indicates writability, "
                  "use getsockopt(2) to read the  SO_ERROR option  at  level  SOL_SOCKET  "
                  "to  determine  whether  connect  completed successfully (SO_ERROR is zero) "
                  "or unsuccessfully (SO_ERROR is one of the usual error codes listed above, "
                  "explaining the reason for the failure).";
            break;
        case EALREADY:
            msg = "The socket is non-blocking and a previous connection attempt has not yet been completed.";
            break;
        case EAFNOSUPPORT:
            msg = "The passed address didn't have the correct address family in its sa_family field.";
            break;
        case EACCES:
            msg = "The user tried to connect to a broadcast address without having the socket broadcast flag enabled.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of else if (fName == "connect").
    else if (fName == "gethostbyname") {
        switch (error) {
        case HOST_NOT_FOUND:
            msg = "The specified host is unknown.";
            break;
        case NO_ADDRESS: // NO_ADDRESS||  NO_DATA
            //case NO_DATA:
            msg = "The requested name is valid but does not have an IP address.";
            break;
        case NO_RECOVERY:
            msg = "A non-recoverable name server error occurred.";
            break;
        case TRY_AGAIN:
            msg = "A temporary error occurred on an authoritative name server.  Try again later.";
            break;
        default:
            msg = defaultError;
        }
    } // End of else if (fName == "gethostbyname").
    else if (fName == "accept") {
        switch (error) {
        case EBADF:
            msg = "The descriptor is invalid.";
            break;
        case ENOTSOCK:
            msg = "The descriptor references a file, not a socket.";
            break;
        case EOPNOTSUPP:
            msg = "The referenced socket is not of type SOCK_STREAM";
            break;
        case EFAULT:
            msg = "The addr parameter is not in a writable part of the user address space.";
            break;
        case EAGAIN:
            msg = "The socket is marked non-blocking and no connections are present to be accepted.";
            break;
        case EPERM:
            msg = "Firewall rules forbid connection.";
            break;
        case ENOBUFS:
        case ENOMEM:
            msg = "Not enough free memory.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of  else if (fName == "accept").
    else if (fName == "recv" || fName == "recvfrom") {
        switch (error) {
        case EBADF:
            msg = "The argument s is an invalid descriptor.";
            break;
        case ENOTCONN:
            msg = "The socket is associated with a connection-oriented protocol and has  not  been  connected.";
            break;
        case ENOTSOCK:
            msg = "The argument s does not refer to a socket.";
            break;
        case EAGAIN:
            msg = "The  socket  is  marked non-blocking and the receive operation would block, or a receive timeout "
                  "had been set and the timeout expired before data was received.";
            break;
        case EINTR:
            msg = "The receive was interrupted by delivery of a signal before any data were available.";
            break;
        case EFAULT:
            msg = "The receive buffer pointer(s) point outside the process's address space.";
            break;
        case EINVAL:
            msg = "Invalid argument passed.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of else if (fName == "recv" || fName == "recvfrom").
    else if (fName == "write") {
        switch (error) {
        case EBADF:
            msg = "fd is not a valid file descriptor or is not open for writing.";
            break;
        case EINVAL:
            msg = "d is attached to an object which is unsuitable for writing.";
            break;
        case EFAULT:
            msg = "buf is outside your accessible address space.";
            break;
        case EPIPE:
            msg = "fd is connected to a pipe or socket whose reading end is closed. "
                  "When this happens  the  writing process  will  receive a SIGPIPE signal; "
                  "if it catches, blocks or ignores this the error EPIPE is returned.";
            break;
        case EAGAIN:
            msg = "Non-blocking I/O has been selected using O_NONBLOCK and there was no room "
                  "in the pipe  or  socket connected to fd to write the data immediately.";
            break;
        case EINTR:
            msg = "The call was interrupted by a signal before any data was written.";
            break;
        case ENOSPC:
            msg = "The device containing the file referred to by fd has no room for the data.";
            break;
        case EIO:
            msg = "A low-level I/O error occurred while modifying the inode.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of else if (fName == "write").
    else if (fName == "send" || fName == "sendto") {
        switch (error) {
        case EBADF:
            msg = "An invalid descriptor was specified.";
            break;
        case ENOTSOCK:
            msg = "The argument s is not a socket.";
            break;
        case EFAULT:
            msg = "An invalid user space address was specified for a parameter.";
            break;
        case EMSGSIZE:
            msg = "The socket requires that message be sent atomically, and the size of the "
                  "message to be sent made this impossible.";
            break;
        case EAGAIN:
            msg = "The socket is marked non-blocking and the requested operation would block.";
            break;
        case ENOBUFS:
            msg = "The system was unable to allocate an internal memory block. "
                  "The operation may succeed when buffers become available.";
            break;
        case EINTR:
            msg = "A signal occurred.";
            break;
        case ENOMEM:
            msg = "No memory available.";
            break;
        case EINVAL:
            msg = "Invalid argument passed.";
            break;
        case EPIPE:
            msg = "The local end has been shut down on a connection oriented socket. "
                  "In this case the process will also receive a SIGPIPE unless MSG_NOSIG�"
                  "NAL is set.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of .
    else if (fName == "setsockopt") {
        switch (error) {
        case EBADF:
            msg = "The argument s is not a valid descriptor.";
            break;
        case ENOTSOCK:
            msg = "The argument s is a file, not a socket.";
            break;
        case ENOPROTOOPT:
            msg = "The option is unknown at the level indicated.";
            break;
        case EFAULT:
            msg = "The address pointed to by optval is not in a valid part of the process address space. "
                  "For getsockopt, this error may also be returned if optlen is not in a valid part of the "
                  "process address space.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of .
    else if (fName == "read") {
        switch (error) {
        case EAGAIN:
            msg = "Non-blocking  I/O  has  been  selected  using  O_NONBLOCK "
                  "and no data was immediately available for reading.";
            break;
        case EIO:
            msg = "I/O error. This will happen for example when the process is in a  background  process "
                  "group,  tries to read from its controlling tty, and either it is ignoring or blocking "
                  "SIGTTIN or its process group is orphaned.  It may also occur when  there  is  a  low- "
                  "level I/O error while reading from a disk or tape.";
            break;
        case EISDIR:
            msg = "fd refers to a directory.";
            break;
        case EBADF:
            msg = "fd is not a valid file descriptor or is not open for reading.";
            break;
        case EINVAL:
            msg = "fd is attached to an object which is unsuitable for reading.";
            break;
        case EFAULT:
            msg = "buf is outside your accessible address space.";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of .
#if 0
    else if (fName == "") {
        switch (error) {
        case :
            msg = "";
            break;
        default:
            msg = defaultError;
            break;
        }
    } // End of .
#endif
    else {
        msg = "Unknown - command \"" + fName + "\" is not entered in printError function.";
    }
    std::cerr << msg << std::endl;
} // End of printError(...).

