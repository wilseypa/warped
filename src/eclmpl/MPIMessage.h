#ifndef MPI_MESSAGE_H
#define MPI_MESSAGE_H

#include <mpi.h>                        // for MPI_REQUEST_NULL, etc
#include <algorithm>
#include <functional>                   // for unary_function

#include "SerializedInstance.h"         // for SerializedInstance

using std::unary_function;

/**
   Encapsulates an in-transit MPI message.
*/

class MPIMessage {

public:
    MPIMessage(const SerializedInstance* initMessage,
               MPI_Request initRequest)
        : myMessage(initMessage),
          myRequest(initRequest) {}

    MPIMessage() : myMessage(0),
        myRequest(MPI_REQUEST_NULL) {}


    MPIMessage(const MPIMessage& m) : myMessage(m.myMessage),
        myRequest(m.myRequest) {}

    bool isComplete() const {
        return myMessage == 0;
    }

    bool checkForCompletion() const {
        int mpiCompleteStatus = 0;
        MPI_Test(&myRequest, &mpiCompleteStatus, &myLastStatus);
        return bool(mpiCompleteStatus);
    }

    void complete() {
        delete myMessage;
        myMessage = 0;
        myRequest = MPI_REQUEST_NULL;
    }

    virtual ~MPIMessage() {}

    class operationComplete : public unary_function<MPIMessage&, bool> {
    public:
        bool operator()(MPIMessage& m) {
            return m.isComplete();
        }
    };

    /**
       A functor for finishing send operations.
    */
    class finalizeSend : public unary_function<MPIMessage&, void> {
    public:
        void operator()(MPIMessage& m) {
            if (m.checkForCompletion()) {
                m.complete();
            }
        }
    };

    MPIMessage operator=(const MPIMessage& m) {
        myMessage = m.myMessage;
        myRequest = m.myRequest;
        myLastStatus = m.myLastStatus;

        return *this;
    }

protected:
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
};

#endif
