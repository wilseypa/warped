#include "MPIMessage.h"
#include "SerializedInstance.h"

MPIMessage::MPIMessage() : myMessage(NULL), myRequest(MPI_REQUEST_NULL)
{
}

MPIMessage::MPIMessage( const SerializedInstance* initMessage, MPI_Request initRequest ) : myMessage(initMessage), myRequest(initRequest)
{
}

MPIMessage::~MPIMessage()
{
    // The SerializedInstance gets allocated by sendMessage in Transceiver.cpp or Serializable::serialize called by sendMessage in CommunicationManagerImplementationBase.cpp
    delete myMessage;
}

bool MPIMessage::operationComplete( MPIMessage& m )
{
    return m.myMessage == NULL;
}

void MPIMessage::finalizeSend( MPIMessage& m )
{
    int mpiCompleteStatus = 0;
    bool checkForCompletion = false;
    
    MPI_Test( &m.myRequest, &mpiCompleteStatus, &m.myLastStatus );
    checkForCompletion = ( mpiCompleteStatus == 0 ) ? false : true;
    
    if (checkForCompletion)
    {
        delete m.myMessage;
        m.myMessage = NULL;
        m.myRequest = MPI_REQUEST_NULL;
    }
}
