// See copyright notice in file Copyright in the root directory of this archive.

#include "VisualizationClient.h"

using std::cin;

Socket visSocket;

/**
 * Constructor
 */
VisualizationClient::VisualizationClient(const char *hostname, int port) {
  this->hostname = hostname;
  this->port = port;
}

//Use automatically generated copy constructor, or uncomment below.
//VisualizationClient::VisualizationClient(
//         const VisualizationClient &oldVisualizationClient ) {}

/**
 * Destructor
 */
VisualizationClient::~VisualizationClient() {}


int
VisualizationClient::clientConnect(void) {

  if (port < 0 || strlen(hostname) < 1 )
    return -1;
  else {
    visSocket._connect(hostname, port);

    pthread_t inThread, outThread;

    pthread_create( &inThread, NULL, readIn, (void*)&visSocket);
    pthread_create( &outThread, NULL, writeOut, (void*)&visSocket);

    return 0;
  }
}

int
VisualizationClient::clientClose(void) {
  return 0;
}

int
main(int argc, char **argv)
{
  int error, sockHandle, writeSize;
  char toServer[1024];
  char *hostname;
  int port;
  //  VisualizationClient myClient;

  // Check for proper argument length and give usage.
  if (argc < 3 || argc > 3) {
    cout << "Usage: VisualizationClient <servername> <service port #>\n";
    return (-1);
  }
  else {
    hostname = argv[1];
    port = atoi( argv[2] );
    if (port <= 0) {
      cout << "Port invalid\n";
      return (-1);
    }
  }

  VisualizationClient myClient(hostname, port);

  if (  !myClient.clientConnect() ) {

    while (1) {
      cin.getline(toServer, sizeof(toServer));
      writeSize = strlen(toServer);
      if ((error = visSocket._writeLine(toServer, writeSize))  != writeSize ) {
	cout << "Client: Couldn't write to socket.\n";
      }//if
    }//while
    
    sockHandle = visSocket.getSocketHandle();
    visSocket.myClose(sockHandle);
    
    return(0);
    
  }//if
  else {
    cerr << "Could not connect\n";
    return(-1);
  }

  sockHandle = visSocket.getSocketHandle();
  visSocket.myClose(sockHandle);

  return(0);
}



void* 
readIn(void *inPtr) {
  //char* fromServer;
  int readsize;
  char *fromServer = new char[1024];

  while ( (readsize = visSocket._read(fromServer, sizeof(int) )) != -1) {
    cout  << "From Server: " << fromServer << endl;
    if ( (strcmp(fromServer, "quit") == 0) )
      exit(0);
      //break;
    else if ( (strcmp(fromServer, "Protocol error.") == 0) )
      exit(1);

    if (readsize < 0) {
      cerr << "Error: Read error\n";
      exit(8);
    }//if


  }//while
  
  return fromServer;
}//readIn

void*
writeOut(void* outPtr) {
  char toServer[1024];
  int writeSize, error, sockHandle;

    while (1) {
      cin.getline(toServer, sizeof(toServer));
      writeSize = strlen(toServer);
      if ((error = visSocket._writeLine(toServer, writeSize))  != writeSize ) {
	cout << "Client: Couldn't write to socket.\n";
      }//if
    }//while

  sockHandle = visSocket.getSocketHandle();
  visSocket.myClose(sockHandle);
  
  return(0);

}//writeOut


