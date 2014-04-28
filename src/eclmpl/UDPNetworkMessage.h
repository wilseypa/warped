#ifndef UDP_NETWORK_MESSAGE_H
#define UDP_NETWORK_MESSAGE_H

#include "eclmplCommonInclude.h"

enum UDPNetworkMessageType {UNDEFINED, INITIALIZE, START, APPLICATION, CONTROL, FINALIZE, SYNCH};

/// The UDPNetworkMessage class.
class UDPNetworkMessage {
public:
    /// Default Constructor.
    UDPNetworkMessage();
    
    /// Initialization Constructor.
    UDPNetworkMessage(const UDPNetworkMessageType& msgType,
                      const unsigned int& source,
                      const bool& isAck,
                      const bool& isUserData,
                      const SequenceNumber& ackSqNr,
                      const SequenceNumber& endHoleNr,
                      const SequenceNumber& sqNr,
                      const int& dataSize,
                      const char* data);
    
    /// Copy Constructor
    /** Explicitly defined in order to do a deep copy instead of a shallow one. */
    UDPNetworkMessage(const UDPNetworkMessage& original);
    
    /// Copy Assignment
    /** Explicitly defined in order to do a deep copy instead of a shallow one. */
    UDPNetworkMessage& operator=(const UDPNetworkMessage& rhs);

    /// Destructor.
    virtual ~UDPNetworkMessage();
    
    /// Serialize the message data and put the result in buf
    /** @param buf Buffer in which to store the serialized data
        @param maxBufSize The size of the buffer in which to store the serialized data
    */
    virtual unsigned int serialize(char* buf, const int& maxBufSize) const;
    virtual void deserialize(char* msg, const int& msgLen);
    virtual bool timedOut(const double& timeOut) const;
    
    virtual UDPNetworkMessageType getType() const;
    virtual void setType(const UDPNetworkMessageType& msgType);
    
    virtual unsigned int getSourceId() const;
    virtual void setSourceId(const unsigned int& source);
    
    virtual bool getAckFlag() const;
    virtual void setAckFlag(const bool& isAck);
    
    virtual bool getUserDataFlag() const;
    virtual void setUserDataFlag(const bool& isUserData);
    
    virtual SequenceNumber getAckSequenceNr() const;
    virtual void setAckSequenceNr(const SequenceNumber& nr);
    
    virtual SequenceNumber getEndOfHoleNr() const;
    virtual void setEndOfHoleNr(const SequenceNumber& nr);
    
    virtual SequenceNumber getSequenceNr() const;
    virtual void setSequenceNr(const SequenceNumber& nr);
    
    virtual std::chrono::steady_clock::time_point getSendTime() const;
    virtual void setSendTime();
    
    virtual unsigned int getUserDataSize() const;
    virtual void setUserDataSize(const unsigned int& dataSize);
    
    virtual unsigned int getUserData(char* data, const unsigned int& dataSize) const;
    virtual void setUserData(const char* data, const unsigned int& dataSize);
    
    virtual bool operator<(const UDPNetworkMessage& a) const;
    virtual bool operator>(const UDPNetworkMessage& a) const;

    friend std::ostream& operator<< (std::ostream& os, const UDPNetworkMessage& msg);
protected:
    // Header fields
    UDPNetworkMessageType messageType;
    unsigned int sourceId;
    bool ackFlag;
    bool userDataFlag;
    SequenceNumber ackSequenceNr;
    SequenceNumber endOfHoleNr;
    SequenceNumber sequenceNr;
    
    // Other fields
    unsigned int userDataSize;
    char* userData;
    std::chrono::steady_clock::time_point sendTime;
    unsigned int messageSize;
};
#endif
