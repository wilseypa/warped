#ifndef MPI_PHYSICAL_COMMUNICATION_LAYER_H
#define MPI_PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmplCommonInclude.h"
#include "PhysicalCommunicationLayer.h"
#include "SerializedInstance.h"
#include "MPIMessage.h"

/** The MPIPhysicalCommunicationLayer class.

*/
class MPIPhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
    /**@name Public Class Methods of MPIPhysicalCommunicationLayer. */
    //@{

    /// Default Constructor.
    MPIPhysicalCommunicationLayer();

    /// Destructor.
    virtual ~MPIPhysicalCommunicationLayer();

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
    virtual unsigned int physicalGetId() const;

    /** Send data.

        @param toSend Serialized instance to send.
        @param dest Destination to send to.
    */
    virtual void physicalSend( const SerializedInstance* toSend, unsigned int dest );

    /** Check the MPI-probe to see if there are message to retrieve.

        @return The retrieved message (NULL if no message).
    */
    virtual SerializedInstance* physicalProbeRecv();
    
    /// Clean up and call MPI_Finalize.
    virtual void physicalFinalize();

    /** Return how many processes are involved in the communicator.

        @return The number of processes involved in the communicator.
    */
    virtual unsigned int physicalGetSize() const;

    //@} // End of Public Class Methods of MPIPhysicalCommunicationLayer.

protected:
    /**@name Protected Class Attributes of MPIPhysicalCommunicationLayer. */
    //@{

    std::vector<MPIMessage> pendingSends;
    std::vector<MPIMessage> pendingReceives;
    
    //@} // End of Protected Class Attributes of MPIPhysicalCommunicationLayer.
};

#endif
