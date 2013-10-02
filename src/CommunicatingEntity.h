#ifndef COMMUNICATING_ENTITY_H
#define COMMUNICATING_ENTITY_H


#include "warped.h"
#include "KernelMessage.h"

/** The CommunicatingEntity abstract base class.

    Any class that wants to receive kernel messages from the
    communication manager must be derived from this abstract
    class. This way, the child class must implement the
    "registerWithCommunicationManager" and "receiveKernelMessage"
    methods.

*/
class CommunicatingEntity {
public:

    /**@name Public Class Methods of CommunicatingEntity. */
    //@{

    /// Destructor.
    virtual ~CommunicatingEntity() {};

    /** Method the communication mgr calls to deliver a message.

        This is a pure virtual function that has to be overriden.

        @param msg Pointer to the received message.
    */
    virtual void receiveKernelMessage(KernelMessage* msg) = 0;
    //@} // End of Public Class Methods of CommunicatingEntity.

protected:
    /**@name Protected Class Methods of CommunicatingEntity. */
    //@{

    /// Default Constructor.
    CommunicatingEntity() {};
    //@} // End of Protected Class Methods of CommunicatingEntity.
};

#endif
