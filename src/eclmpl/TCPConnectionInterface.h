#ifndef TCP_CONNECTION_INTERFACE_H
#define TCP_CONNECTION_INTERFACE_H

#include "eclmplCommonInclude.h"
#include "SocketBasedConnectionInterface.h"

/// The TCPConnectionInterface class
class TCPConnectionInterface : public SocketBasedConnectionInterface {
public:
    /**@name Public Class Methods of TCPConnectionInterface. */
    //@{
    
    /// Initialization Constructor
    TCPConnectionInterface(const unsigned int& mtuSize);
    
    // Disable Copy Constructor and Copy Assignment or they will be implicitly defined
    TCPConnectionInterface(const TCPConnectionInterface& original) = delete;
    TCPConnectionInterface& operator=(const TCPConnectionInterface& rhs) = delete;
    
    /// Destructor
    virtual ~TCPConnectionInterface();
    
    virtual void disableNagle(const unsigned int& id);
    virtual void disableNagle();
    
    //@} // End of Public Class Methods of TCPConnectionInterface.
protected:
    /**@name Protected Class Methods of TCPConnectionInterface. */
    //@{
    
    /** Default Constructor
        
        Made Protected because this class must be initialized with an argument for mtu.
    */
    TCPConnectionInterface();
    
    //@} // End of Protected Class Methods of SocketBasedConnectionInterface.
};

#endif
