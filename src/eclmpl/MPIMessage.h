#ifndef MPI_MESSAGE_H
#define MPI_MESSAGE_H

#include <mpi.h>
#include "SerializedInstance.h"

/**
   Encapsulates an in-transit MPI message.
*/

class MPIMessage {

public:
    /**@name Public Class Methods of MPIMessage. */
    //@{

    /// Default Constructor.
    MPIMessage();
    
    /// Initialization Constructor.
    MPIMessage( const SerializedInstance* initMessage, MPI_Request initRequest );
    
    /// Destructor.
    virtual ~MPIMessage();
    
    /** Check if the current operation on the given MPI message has completed
        
        @param m Message to check
        @return True/False, Has the operation completed?
    */
    static bool operationComplete( MPIMessage& m );
    
    /** Check if sending has completed. If so, deallocate this message.
        
        MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
        is called from this method to check if sending or receiving as finished.
        
        @param m Message to check and deallocate.
    */
    static void finalizeSend( MPIMessage& m );
    
    //@} // End of Public Class Methods of MPIMessage.

protected:
    /**@name Protected Class Attributes of MPIMessage. */
    //@{
    
    /**
       The buffer to be sent/read.
    */
    const SerializedInstance* myMessage;

    /**
       The request associated with this message;
    */
    mutable MPI_Request myRequest;

    /**
       The last status associated with this message.
    */
    mutable MPI_Status myLastStatus;
    
    //@} // End of Protected Class Attributes of MPIMessage.
};

#endif
