#ifndef PHYSICAL_COMMUNICATION_LAYER_H
#define PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmplCommonInclude.h"
#include "SocketBasedConnectionInterface.h"
#include "SerializedInstance.h"

/** The PhysicalCommunicationLayer abstract base class.

    This is the abstract base class for all physical communication
    library implementations that can be used in the simulation
    kernel. To use different physical communication library
    implementations, a separate class should be defined. It, however,
    MUST be derived from this abstract base class (to support the
    interface).

    To accomodate different physical communication libraries, we
    define the PhysicalCommunicationLayer abstract base class. The
    methods in this interface class are all pure virtual.  Therefore,
    when one wants to use a new physical communication library
    manager, one has to write a new physical communication layer class
    (which is derived from the PhysicalCommunicationLayer abstract
    class) and re-implement the functions specified in it.  Currently,
    the following physical communication library implementations are
    supported: (a) DefaultPhysicalCommunicationLayer (for
    uni-processor simulation without MPI or any library support); (b)
    MPIPhysicalCommunicationLayer and
    MPINonBlockingPhysicalCommunicationLayer (for making calls to the
    MPI library); and (c) TCPMPLPhysicalCommunicationLayer (for making
    calls to the TCPMPL library).

*/
class PhysicalCommunicationLayer {

public:
    /**@name Public Class Methods of PhysicalCommunicationLayer. */
    //@{
    
    /// Destructor.
    virtual ~PhysicalCommunicationLayer() {}

    /** Init physical layer.

        This is a pure virtual function that has to be overriden.
    */
    virtual void physicalInit() = 0;

    /** Get the Id of the simulation manager.

        This is a pure virtual function that has to be overriden.

        @return Id of the simulation manager.
    */
    virtual unsigned int physicalGetId() const = 0;

    /** Send data.

        This is a pure virtual function that has to be overriden.

        @param toSend Serialized instance to send.
        @param dest Destination to send to.
    */
    virtual void physicalSend(const SerializedInstance* toSend, unsigned int dest) = 0;

    /** Check the probe to see if there are messages to retrieve.

        This is a pure virtual function that has to be overriden.

        @return The retrieved message (NULL if no message).
    */
    virtual SerializedInstance* physicalProbeRecv() = 0;

    /// Clean up.
    virtual void physicalFinalize() = 0;

    /** Return how many processes are involved in the communicator.

        @return The number of processes involved in the communicator.
    */
    virtual unsigned int physicalGetSize() const = 0;

    //@} // End of Public Class Methods of PhysicalCommunicationLayer.
    
protected:
    /**@name Protected Class Methods of PhysicalCommunicationLayer. */
    //@{

    /// Default Constructor.
    /// Do not instanciate objects of this class
    /** The constructor has been made protected to prevent instanciation of objects of this type.
        However, derived classes will need to use it to construct themselves.
    */
    PhysicalCommunicationLayer() {}
    
    //@} // End of Protected Class Methods of PhysicalCommunicationLayer.
};

#endif
