#ifndef SOCKET_CC
#define SOCKET_CC

// See copyright notice in file Copyright in the root directory of this archive.

#include <WarpedConfig.h>
#include "Socket.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
using std::cerr;
using std::endl;

#define DEFAULT_PORT_NUMBER 2049

#ifndef MSG_WAITALL
#define MSG_WAITALL 0x40
#endif

Socket::Socket() {
  // Open a TCP socket (an Internet stream socket).
  
  if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error = errno;
    cerr << "Error opening socket (error = " << Socket::getErrorString(errno)
	 << ")" << endl;
    return;
  }
  
  portNumber  = -1;
  error       = 0;
  nonBlocking = false;
}

int
Socket::_bind(int pNumber) {
  struct sockaddr_in serv_addr;

  portNumber = pNumber;
  
  // Bind our local address so that the someone can send data to us.
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port        = htons(portNumber);
  
  if (bind(socketHandle, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
    error = errno;
    return error;
  }
  
  _listen();
  error = 0;
  nonBlocking = false;
  
  return error;
}

Socket::Socket(int port) {
  struct sockaddr_in serv_addr;

  // Open a TCP socket (an Internet stream socket).
  
  if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error = errno;
    cerr << "Error opening socket (error = " << Socket::getErrorString(errno)
	 << ")" << endl;
    return;
  }

  portNumber = port;
  
  // Bind our local address so that the someone can send data to us.
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port        = htons(portNumber);

  if (bind(socketHandle, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
    error = errno;
    cerr << "Error binding socket to local address (error = "
	 << Socket::getErrorString(errno) << ")" << endl;
    return;
  }

  _listen();
  error = 0;
  nonBlocking = false;
}

// This constructor assusmes that you need to connect to the server whose
// IP address (in standard format with port number) is specified. It creates
// a new socket and establishes connection with the server.
// Example of the parameter would be 129.137.9.5:1024

// Note: the serverAddress string is modified.

Socket::Socket(char *serverAddress) {
  if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error = errno;
    cerr << "Error opening socket (error = "
	 << Socket::getErrorString(errno) << ")" << endl;
    return;
  }
  
  if ((error = _connect(serverAddress)) != 0) {
    cerr << "Error connecting to server (error = "
	 << Socket::getErrorString(error) << ")\n";
    return;
  }

  error = 0;
  nonBlocking = false;
}

// This socket call takes up the server's name and resolves the actual IP
// address and establishes a connection with that server at the specified
// port number. 

Socket::Socket(const char *serverName, int port) {
  if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error = errno;
    cerr << "Error opening socket (error = "
	 << getErrorString(errno) << ")" << endl;
    return;
  }
  
  if ((error = _connect(serverName, port)) != 0) {
    cerr << "Error connecting to server (error = "
	 << getErrorString(error) << ")\n";
  }

  nonBlocking = false;
}

Socket::~Socket() {
  // shutdown(socketHandle, 2);
  // close(socketHandle);
}

int
Socket::_connect(char *serverAddress) {
  struct sockaddr_in serv_addr;
  char *colonPosition;
  
  if ((colonPosition = strchr(serverAddress, ':')) == NULL) {
    return -1;
  }
  
  portNumber    = atoi((colonPosition + 1));
  *colonPosition = '\0';
  
  if (inet_addr(serverAddress) == (unsigned) -1) {
    return -1;
  }
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(serverAddress);
  serv_addr.sin_port        = portNumber;

  *colonPosition = ':';
  
  if (connect(socketHandle, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    return errno;
  }

  return 0;
}

int
Socket::_connect(const char *serverName, int port) {
  struct hostent *serverInfo;
  struct sockaddr_in serv_addr;

  portNumber = port;
  
  if ((serverInfo = gethostbyname(serverName)) == NULL) {
    return errno;
  }
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = *((long *) serverInfo->h_addr_list[0]);
  serv_addr.sin_port        = htons(portNumber);
  
  if (connect(socketHandle, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    return errno;
  }

  return 0;
}

int
Socket::_listen() const {
  int error = 0;
  
  if ((error = listen(socketHandle, 5)) != 0) {
    cerr << "_listen encountered error (error = " << getErrorString(error)
	 << ")" << endl;
  }

  return error;
}

Socket*
Socket::_accept() const {
  return new Socket(this);
}

int
Socket::_read(char *buffer, int length) {

  // The following is a protocol used to get the upcoming message length.  An
  // integer is read with the length of the message, then the message is read.
  int messageSize;
  int error;
  if ((error = waitAndRead((char *) &messageSize, sizeof(int))) != sizeof(int)) {
    cerr << "Read encountered an error (error = " << getErrorString(error)
	 << ")" << endl;
  }


  messageSize = (int) ntohl(messageSize);

  
  //if (length > messageSize) {
  length = messageSize + 1;
  //}
  

  if ((error = waitAndRead(buffer, length)) != length) {
    return error * -1;
  }
  else
      *(buffer + length) = '\0';

  error = 0;
  return length;
}

int
Socket::_write(const char *buffer, int length) {
//   int messageSize = (int) htonl(length);

  /* Following is the code I found in this class left as reference. */
  //if ((error = waitAndWrite((char *) &messageSize, sizeof(int)))
  //  != sizeof(int)) {
  //  return error * -1;
  //}

  if ((error = waitAndWrite((char *) buffer, length)) != length) {
    return error * -1;
  }

  error = 0;
  return length;
}

int
Socket::_writeLine(const char *buffer, int length) {
//   int messageSize = (int) htonl(length);
  
  if ((error = waitAndWrite((char *) buffer, length)) != length) {
    return error * -1;
  }
  if ((error = waitAndWrite("\n", sizeof('\n'))) != sizeof('\n')) {
      return error * -1;
  }
  error = 0;
  return length;
}

char*
Socket::readMessage(int *length) {
  int  messageSize;
  char *buffer = NULL;
  
  if ((error = waitAndRead((char *) &messageSize, sizeof(int)))
      != sizeof(int)) {
    cerr << "Read encountered an error (error = " << getErrorString(error)
	 << ")" << endl;
  }
  
  messageSize = (int) ntohl(messageSize);

  *length = messageSize;
  buffer  = new char[messageSize];
  
  if ((error = waitAndRead(buffer, messageSize)) != messageSize) {
    delete [] buffer;
    return NULL;
  }

  error = 0;
  return buffer;
}

int
Socket::waitAndRead(char *buffer, int length) {
  int dataRead = 0, result = 0;
  
  while (dataRead < length) {
    if ((result = recv(socketHandle, (buffer + dataRead), (length - dataRead), MSG_WAITALL)) < 0) {
      if (nonBlocking == false) {
	return errno * -1;
      }
      else
	  return result;
    }
    else {
      dataRead += result;
    }
  }

  return dataRead;
}

int
Socket::waitAndWrite(const char *buffer, int length) {
  int dataWritten = 0, result = 0;

  while (dataWritten < length) {
    if ((result = write(socketHandle, (buffer + dataWritten), (length - dataWritten))) < 0) {
      if (nonBlocking == false) {
	return errno * -1;
      }
    }
    else {
      dataWritten += result;
    }
  }

  return dataWritten;
}

int
Socket::getSocketHandle() const {
  return socketHandle;
}

int
Socket::getPortNumber() const {
  return portNumber;
}

Socket::Socket(const Socket *parent) {
  struct sockaddr *address = (sockaddr *) new char[sizeof(struct sockaddr)];

#ifdef USE_SOCKLEN_T
  socklen_t len = 0;
#else
  int len = 0;
#endif
  
  socketHandle = accept(parent->getSocketHandle(), address, &len);

  if (socketHandle == -1) {
    error = errno;
    cerr << "Error accepting connection (error = " << getErrorString(error)
	 << ")" << endl;
  }
  
  nonBlocking = true;
  portNumber  = -1;
  error       = 0;
}

void
Socket::myClose(int sockHandle) {
  close(sockHandle);
}

int
Socket::getError() const {
  return error;
}

int
Socket::setNonBlocking() {
  nonBlocking = true;
  return fcntl(getSocketHandle(), F_SETFL, O_NONBLOCK);
}

const char*
Socket::getErrorString(int errorNumber) {
  switch(errorNumber) {
  case EBADF:
    return "EBADF";

  case EINVAL:
    return "EINVAL";

  case EACCES:
    return "EACCES";

  case ENOTSOCK:
    return "ENOTSOCK";

  case EROFS:
    return "EROFS";

  case EFAULT:
    return "EFAULT";

  case ENAMETOOLONG:
    return "ENAMETOOLONG";

  case ENOENT:
    return "ENOENT";

  case ENOMEM:
    return "ENOMEM";

  case ELOOP:
    return "ELOOP";

  case EISCONN:
    return "EISCONN";

  case ECONNREFUSED:
    return "ECONNREFUSED";

  case ETIMEDOUT:
    return "ETIMEDOUT";

  case ENETUNREACH:
    return "ENETUNREACH";
    
  default:
    return "UNKNOWN";
  }

  return NULL;
}

#endif
