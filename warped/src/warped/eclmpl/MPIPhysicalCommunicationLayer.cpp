#include <mpi.h>
#include "MPIPhysicalCommunicationLayer.h"
#include "warped/SerializedInstance.h"

using std::ofstream;
using std::endl;

#define MPI_DATA_TAG  100

MPIPhysicalCommunicationLayer::MPIPhysicalCommunicationLayer(){};

MPIPhysicalCommunicationLayer::~MPIPhysicalCommunicationLayer(){};

void
MPIPhysicalCommunicationLayer::getCStyleArguments( int &argc, 
						   char ** &argv,
						   const vector<string> &arguments ){
  argc = arguments.size();
  argv = new char*[argc];
  if (argc > 0) {
    for( int i = 0; i < argc; i++ ){
        argv[i] = const_cast<char *>(arguments[i].c_str());
  //      debug::debugout << "actual MPIArgs[" << i << "] = " << argv[i] << std::endl;
    }
  }else {
      argc = 1;
      argv[0] = const_cast<char *>(" ");
  }
}

void
MPIPhysicalCommunicationLayer::startMPI( const vector<string> &arguments ){
  int argc = 0;
  char **argv = 0;
  getCStyleArguments( argc, argv, arguments );

  debug::debugout << "About to call MPI_Init" << endl;
  MPI_Init( &argc, &argv );
  debug::debugout << "Done with MPI_Init" << endl;

  // MPICH Rewrites these arguments out from under us - we can't delete the
  // values inside the array.  We'll delete the array at list.
  delete [] argv;
}

void
MPIPhysicalCommunicationLayer::physicalInit( SimulationConfiguration &configuration ){
  startMPI( configuration.getArguments() );
  mySimulationManagerID = physicalGetId();
}

int
MPIPhysicalCommunicationLayer::physicalGetId() const {
   // get the id of this simulation manager ...
   int id;
   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   return id;
}

void
MPIPhysicalCommunicationLayer::checkPendingSends(){
  for_each( pendingSends.begin(),
	    pendingSends.end(),
	    MPIMessage::finalizeSend() );
  
  pendingSends.erase( 
		     remove_if( pendingSends.begin(),
				pendingSends.end(),
				MPIMessage::operationComplete() ),
		     pendingSends.end() );
}

void
MPIPhysicalCommunicationLayer::physicalSend( const SerializedInstance *messageToSend,
					     unsigned int dest){
   ASSERT( messageToSend != NULL);
   checkPendingSends();
   
   MPI_Request request;
   MPI_Isend( const_cast<char *>(&messageToSend->getData()[0]), 
	      messageToSend->getSize(), 
	      MPI_BYTE, 
	      dest, 
	      MPI_DATA_TAG, 
	      MPI_COMM_WORLD,
	      &request );

   pendingSends.push_back( MPIMessage( messageToSend,
				       request ) );
}

SerializedInstance *
MPIPhysicalCommunicationLayer::physicalProbeRecv(){
  SerializedInstance *retval = 0;

  MPI_Status status;
  int count = 0;
  char *message = NULL;
  int flag = 0;

  //  if( mySimulationManagerID != 0 )    cout << mySimulationManagerID << "- about to probe" << endl;
  // check to see if any messages have arrived ...
  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status );
  //  if( mySimulationManagerID != 0 )  cout << mySimulationManagerID << "Done!" << endl;
  
  if (flag != 0) {
    // how many messages did we get ? ...
    MPI_Get_count( &status, MPI_BYTE, &count );
    message = new char[count];
    ASSERT(message != NULL);
    MPI_Recv( message, 
	      count, 
	      MPI_BYTE, 
	      MPI_ANY_SOURCE, 
	      MPI_ANY_TAG, 
	      MPI_COMM_WORLD, 
	      MPI_STATUS_IGNORE );
    retval = new SerializedInstance( message, count );
    delete []message;
  }

  return retval;
}

bool 
MPIPhysicalCommunicationLayer::physicalProbeRecvBuffer(char *buffer,
                                                       int size,
                                                       bool& sizeStatus){
  MPI_Status status;
  int flag = 0, msgSize = 0;
  
  // check to see if any messages have arrived ...
  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
  
  if (flag != 0) {
    MPI_Get_count(&status, MPI_BYTE, &msgSize);
    if ( msgSize > size) {
      sizeStatus = false;
    }
    else {
      sizeStatus = true;
      MPI_Recv( buffer, msgSize, MPI_BYTE, MPI_ANY_SOURCE,
		MPI_ANY_TAG, MPI_COMM_WORLD, &status );
    }
    return true;
  }
  else {
    return false;
  }
}

void
MPIPhysicalCommunicationLayer::physicalFinalize(){
  MPI_Finalize();
}

int
MPIPhysicalCommunicationLayer::physicalGetSize() const {
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  return size;
}


