#ifndef MPI_PHYSICAL_COMMUNICATION_LAYER_H
#define MPI_PHYSICAL_COMMUNICATION_LAYER_H

#include <vector>                       // for vector

#include "MPIMessage.h"                 // for MPIMessage
#include "PhysicalCommunicationLayer.h"  // for PhysicalCommunicationLayer
#include "eclmplCommonInclude.h"
#include "warped.h"

class SerializedInstance;

/** The MPIPhysicalCommunicationLayer class.

*/
class MPIPhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
    /**@name Public Class Methods of MPIPhysicalCommunicationLayer. */
    //@{

    /// Default Constructor.
    MPIPhysicalCommunicationLayer();

    /// Destructor.
    ~MPIPhysicalCommunicationLayer();

    /** Init physical layer.

        MPI_init(argc, argv) is called from this method, as need to be
        done before any other MPI call can be done. Thus, argc and argv
        needs to be passed in to this method.

        @param configuration The simulation configuration being used.
    */
    virtual void physicalInit();

    /** Get the Id of the simulation manager.

        @return Id of the simulation manager.
    */
    int physicalGetId() const;

    /** Send buffer.

        @param buffer Char buffer to send.
        @param size Size of the buffer to send.
    */
    virtual void physicalSend(const SerializedInstance* toSend, unsigned int dest);

    /** Check the MPI-probe to see if there are message to retrieve.

        @return The retrieved message (NULL if no message).
    */
    virtual SerializedInstance* physicalProbeRecv();

    /** Retrieve message into a buffer.

        @param buffer Buffer to which we save the message.
        @param size Size of the buffer.
        @param sizeStatus Was the size of retr. msg > size?
        @return True/False, Was any message retrieved?
    */
    virtual bool physicalProbeRecvBuffer(char* buffer, int size, bool& sizeStatus);

    /// Clean up and call MPI_Finalize.
    void physicalFinalize();

    /** Return how many processes are involved in the communicator.

        @return The number of processes involved in the communicator.
    */
    virtual int physicalGetSize() const;

    //@} // End of Public Class Methods of MPIPhysicalCommunicationLayer.

protected:
    /**@name Protected Class Attributes of MPIPhysicalCommunicationLayer. */
    //@{

    /// Id of the simulation manager.
    int mySimulationManagerID;

    //@} // End of Protected Class Attributes of MPIPhysicalCommunicationLayer.

private:
    /**
       Checks the status of pending sends.  Clears completed sends out of the
       pendingSend vector, and deallocates the resources associated with
       them.
    */
    void checkPendingSends();

    /**
       Does the dirty work of actually starting MPI.
    */
    void startMPI();


    std::vector<MPIMessage> pendingSends;
    std::vector<MPIMessage> pendingReceives;
};

#endif
