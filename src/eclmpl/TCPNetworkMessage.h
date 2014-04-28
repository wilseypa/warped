#ifndef TCP_NETWORK_MESSAGE_H
#define TCP_NETWORK_MESSAGE_H

#include "eclmplCommonInclude.h"

/// The TCPNetworkMessage class.
class TCPNetworkMessage {
public:

    /// Default Constructor.
    TCPNetworkMessage();

    /// Initialization Constructor.
    TCPNetworkMessage(const unsigned int& size, char* data);
    
    /// Copy Constructor
    /** Explicitly defined in order to do a deep copy instead of a shallow one. */
    TCPNetworkMessage(const TCPNetworkMessage& original);
    
    /// Copy Assignment
    /** Explicitly defined in order to do a deep copy instead of a shallow one. */
    TCPNetworkMessage& operator=(const TCPNetworkMessage& rhs);

    /// Destructor.
    virtual ~TCPNetworkMessage();

    virtual unsigned int getUserDataSize() const;
    virtual void setUserDataSize(const unsigned int& dataSize);
    
    virtual unsigned int getUserData(char* data, const unsigned int& dataSize) const;
    virtual void setUserData(const char* data, const unsigned int& dataSize);
    
    friend std::ostream& operator<< (std::ostream& os, const TCPNetworkMessage& msg);
protected:
    unsigned int userDataSize;
    char* userData;
};
#endif
