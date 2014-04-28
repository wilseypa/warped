#include "MPIPhysicalCommunicationLayer.h"
#include <mpi.h>

#define MPI_DATA_TAG  100

// Default Constructor.
MPIPhysicalCommunicationLayer::MPIPhysicalCommunicationLayer() : PhysicalCommunicationLayer()
{
}

// Destructor.
MPIPhysicalCommunicationLayer::~MPIPhysicalCommunicationLayer()
{
}

/*  Init physical layer.

    MPI_init(argc, argv) is called from this method, as need to be
    done before any other MPI call can be done. Thus, argc and argv
    needs to be passed in to this method.

    @param configuration The simulation configuration being used.
*/
void MPIPhysicalCommunicationLayer::physicalInit()
{
    // MPI_Init requires command line arguments, but doesn't use them. Just give
    // it an empty vector.
    int argc = 0;
    char** argv = new char*[1];
    argv[0] = NULL;

    MPI_Init(&argc, &argv);
    
    delete [] argv;
}

/*  Get the Id of the simulation manager.
    
    @return Id of the simulation manager.
*/
unsigned int MPIPhysicalCommunicationLayer::physicalGetId() const
{
    int id = 0;
    
    MPI_Comm_rank( MPI_COMM_WORLD, &id );
    
    return (unsigned int)id;
}

/* Send data.

    @param toSend Serialized instance to send.
    @param dest Destination to send to.
*/
void MPIPhysicalCommunicationLayer::physicalSend( const SerializedInstance* messageToSend, unsigned int dest )
{
    ASSERT( messageToSend != NULL );
    
    // Check the status of pending sends.
    // Clear completed sends from the pendingSend vector, and deallocate
    // the associated resources
    for_each( pendingSends.begin(), pendingSends.end(), MPIMessage::finalizeSend );
    
    pendingSends.erase( remove_if( pendingSends.begin(), pendingSends.end(), MPIMessage::operationComplete ), pendingSends.end() );

    MPI_Request request;
    MPI_Isend( const_cast<char*>( &messageToSend->getData()[0] ), messageToSend->getSize(), MPI_BYTE, dest, MPI_DATA_TAG, MPI_COMM_WORLD, &request );

    pendingSends.push_back( MPIMessage( messageToSend, request ) );
}

/* Check the MPI-probe to see if there are message to retrieve.

    @return The retrieved message (NULL if no message).
*/
SerializedInstance* MPIPhysicalCommunicationLayer::physicalProbeRecv()
{
    SerializedInstance* retval = 0;

    MPI_Status status;
    int count = 0;
    char* message = NULL;
    int flag = 0;

    // check to see if any messages have arrived ...
    MPI_Iprobe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status );
    
    if (flag != 0) {
        // how many messages did we get ? ...
        MPI_Get_count( &status, MPI_BYTE, &count );
        message = new char[count];
        ASSERT( message != NULL );
        MPI_Recv( message, count, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        retval = new SerializedInstance( message, count );
        delete [] message;
    }

    return retval;
}

// Clean up and call MPI_Finalize.
void MPIPhysicalCommunicationLayer::physicalFinalize()
{
    MPI_Finalize();
}

/* Return how many processes are involved in the communicator.

    @return The number of processes involved in the communicator.
*/
unsigned int MPIPhysicalCommunicationLayer::physicalGetSize() const
{
    int size = 0;
    
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    
    return (unsigned int)size;
}
