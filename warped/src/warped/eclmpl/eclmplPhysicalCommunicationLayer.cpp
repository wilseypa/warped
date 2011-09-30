#include "eclmplPhysicalCommunicationLayer.h"
#include "warped/SerializedInstance.h" 
#include <fstream>

#define ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
const int maxBuf = 65535;

eclmplPhysicalCommunicationLayer::eclmplPhysicalCommunicationLayer() {
  physicalId = 0;
  physicalSize = 0;
} // End of default constructor.

eclmplPhysicalCommunicationLayer::~eclmplPhysicalCommunicationLayer(){
} // End of desctructor.

void
eclmplPhysicalCommunicationLayer::writeProcGroupFile( SimulationConfiguration &configuration ){
  std::ofstream procGroupFile( "procgroup" );
  if( !procGroupFile ){
    perror("Error writing procgroup file:");
    abort();
  }

  //This line states that the first process is started locally
  char masterHostName[128];
  ASSERT( gethostname(masterHostName, 128) == 0);

  procGroupFile << masterHostName << " 0 " << std::endl;

  //A list of all of the nodes
  const vector<string> nodeList = configuration.getNodes();
  for( vector<string>::const_iterator i = nodeList.begin();
        i < nodeList.end();
        i++ ){
      ///The comma separated nodes in the procgroup file at listed here in
      //the standard format:  nodeName #Processes ProgramLocation
	  //<< configuration.getBinaryName()
      procGroupFile << (*i) << " 1 " << " " << std::endl;
  }
  procGroupFile.close();
}

void
eclmplPhysicalCommunicationLayer::physicalInit( SimulationConfiguration &configuration ) {
	writeProcGroupFile( configuration );
	int argc = configuration.getArguments().size();
	char **argv = new char*[argc];
	int i;
	for( i = 0; i < argc; i++ ){
		argv[i] = strdup( configuration.getArguments()[i].c_str() );
	}

	connInterface->establishConnections(&argc, &argv);
	physicalId = connInterface->getConnectionId();
	if( (physicalSize = connInterface->getNumberOfConnections()) > 1)
	{
	  initializeCommunicationLayerAttributes();
	}

	for( i = 0; i < argc; i++ ){
		free( argv[i] );
	}
	delete [] argv;
} // End of physicalInit(...).

int
eclmplPhysicalCommunicationLayer::physicalGetId() const {
  return physicalId;
} // End of physicalGetId().

void
eclmplPhysicalCommunicationLayer::physicalSend( const SerializedInstance *toSend,
						unsigned int dest){

  const void *buffer = &toSend->getData()[0];
  unsigned int size = toSend->getSize();

  ECLMPL_ASSERT(buffer != NULL);
  ECLMPL_ASSERT(dest <= physicalSize);
  ECLMPL_ASSERT(size <= connInterface->getMTU());
  if (physicalSize>1)
  {
	  ECLMPL_ASSERT(dest != physicalId);
	  connInterface->send(size, buffer, dest);
  }
  //toSend is created by serialize and needs to be deleted
  delete toSend;
} // End of physicalSend(...).

SerializedInstance *
eclmplPhysicalCommunicationLayer::physicalProbeRecv(){
#ifdef ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
  if (physicalSize>1 && inOrderMessageQ.empty() == true) {
    probeNetwork();
  }
#else
    probeNetwork();
#endif
    return getNextInSequence();
} // End of physicalProbeRecv().

// const SerializedInstance *
// eclmplPhysicalCommunicationLayer::physicalProbeRecvBuffer(){
//   const SerializedInstance *retval = 0;

//   int msgSize;

//   // Get messages waiting in the network input queues and
//   // insert them in our buffering queues that ensure messages
//   // are delivered in FIFO and sequence order to the kernel.
//   probeNetwork();

//   // Peek next message and get it's size.
//   msgSize = peekNextInSequenceSize();
//   if (msgSize != -1) {
//     retval = getNextInSequence();
//   }

//   return retval;
// } // End of physicalProbeRecvBuf(...).

void
eclmplPhysicalCommunicationLayer::physicalFinalize(){
  connInterface->tearDownConnections();
} // End of physicalFinalize().

int
eclmplPhysicalCommunicationLayer::physicalGetSize() const {
  return physicalSize;
} // End of physicalGetSize().

SerializedInstance *
eclmplPhysicalCommunicationLayer::getNextInSequence() {
  SerializedInstance *retval = 0;

  NetworkMessage *nwMsg;
  char *msg = NULL;

  if (inOrderMessageQ.empty() == false) {
    nwMsg = inOrderMessageQ.front();
    inOrderMessageQ.pop_front();
    msg = nwMsg->getUserData();
    retval = new SerializedInstance( nwMsg->getUserData(), 
				     nwMsg->getUserDataSize() );
    delete nwMsg;
  } // End of if (inOrderMessageQIsEmpty == false).
  return retval;
} // End of getNextInSequence().

// Size -1 will be returned if no in-sequence message is avaiable.
int
eclmplPhysicalCommunicationLayer::peekNextInSequenceSize() {
  int size = -1;
  if (inOrderMessageQ.empty() == false) {
    size = inOrderMessageQ.front()->getUserDataSize();
  }
  return size;
} // End of peekNextInSequenceSize().

#ifdef ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
#undef ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
#endif
