#ifndef DEFAULT_PHYSICAL_COMMUNICATION_LAYER_H
#define DEFAULT_PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmpl/PhysicalCommunicationLayer.h"
#include "SerializedInstance.h"

/** The DefaultPhysicalCommunicationLayer class.

    This is a default physical communication layer, derived from the
    PhysicalCommunicationLayer abstract interface class. This
    communication layer assumes that a uniprocessor is used and that
    no physical communication layer is actually used. Thus, none of
    the methods in the class have any work to do.
 */
class DefaultPhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
    /**@name Public Class Methods of DefaultPhysicalCommunicationLayer. */
    //@{

    /// Default Constructor.
    DefaultPhysicalCommunicationLayer();

    /// Destructor.
    virtual ~DefaultPhysicalCommunicationLayer();

    /** Initialize the physical communication layer.

        @param configuration The SimulationConfiguration to use for this run.
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
    virtual void physicalSend(const SerializedInstance* toSend, unsigned int dest);


    /** Check the probe to see if there are message to retrieve.

        @return The retrieved message (NULL if no message).
    */
    virtual SerializedInstance* physicalProbeRecv();

    /// Clean up.
    virtual void physicalFinalize();

    /** Return how many processes are involved in the communicator.

        @return The number of processes involved in the communicator.
    */
    virtual unsigned int physicalGetSize() const;

    //@} // End of Public Class Methods of DefaultPhysicalCommunicationLayer.
};

#endif
