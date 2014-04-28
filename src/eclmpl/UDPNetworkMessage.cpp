#include "UDPNetworkMessage.h"

// This is defined in "warped.h" so when we compile this within warped
// we cannot define it here.
//const char DELIMITER = ' ';

// Default Constructor
UDPNetworkMessage::UDPNetworkMessage() {
    messageType = UNDEFINED;
    sourceId = 0;
    ackFlag = false;
    userDataFlag = false;
    ackSequenceNr = 0;
    endOfHoleNr = 0;
    sequenceNr = 0;
    
    userDataSize = 0;
    userData = NULL;
    messageSize = 0;
} // End of Default Constructor.

// Initialization Constructor
UDPNetworkMessage::UDPNetworkMessage(const UDPNetworkMessageType& msgType,
                                     const unsigned int& source,
                                     const bool& isAck,
                                     const bool& isUserData,
                                     const SequenceNumber& ackSqNr,
                                     const SequenceNumber& endHoleNr,
                                     const SequenceNumber& sqNr,
                                     const int& dataSize,
                                     const char* data) {
    messageType = msgType;
    sourceId = source;
    ackFlag = isAck;
    userDataFlag = isUserData;
    ackSequenceNr = ackSqNr;
    endOfHoleNr = endHoleNr;
    sequenceNr = sqNr;
    
    userDataSize = dataSize;
    if (userDataSize > 0) {
        userData = new char[userDataSize];
        ECLMPL_ASSERT(userData != NULL);
        std::copy_n(data, userDataSize, userData);
    }
    else {
        userData = NULL;
    }
    messageSize = sizeof(UDPNetworkMessageType) + 2*sizeof(unsigned int) + 2*sizeof(bool) + 3*sizeof(SequenceNumber) + userDataSize;
} // End of Initialization Constructor.

// Copy Constructor
UDPNetworkMessage::UDPNetworkMessage(const UDPNetworkMessage& original) {
    messageType = original.messageType;
    sourceId = original.sourceId;
    ackFlag = original.ackFlag;
    userDataFlag = original.userDataFlag;
    ackSequenceNr = original.ackSequenceNr;
    endOfHoleNr = original.endOfHoleNr;
    sequenceNr = original.sequenceNr;
    
    userDataSize = original.userDataSize;
    sendTime = original.sendTime;
    messageSize = original.messageSize;
    
    if (original.userData != NULL && original.userDataSize > 0) {
        userData = new char[userDataSize];
        std::copy_n(original.userData, userDataSize, userData);
    }
    else {
        userDataSize = 0;
        userData = NULL;
    }
} // End of Copy Constructor.

// Copy Assignment Overload
UDPNetworkMessage& UDPNetworkMessage::operator=(const UDPNetworkMessage& rhs) {
    messageType = rhs.messageType;
    sourceId = rhs.sourceId;
    ackFlag = rhs.ackFlag;
    userDataFlag = rhs.userDataFlag;
    ackSequenceNr = rhs.ackSequenceNr;
    endOfHoleNr = rhs.endOfHoleNr;
    sequenceNr = rhs.sequenceNr;
    
    userDataSize = rhs.userDataSize;
    sendTime = rhs.sendTime;
    messageSize = rhs.messageSize;
    
    if(rhs.userData != NULL && rhs.userDataSize > 0) {
        delete[] userData;
        userData = new char[userDataSize];
        std::copy_n(rhs.userData, userDataSize, userData);
    }
    else {
        userDataSize = 0;
        delete[] userData;
        userData = NULL;
    }
    
    return *this;
} // End of Copy Assignment Overload.

UDPNetworkMessage::~UDPNetworkMessage() {
    delete[] userData;
} // End of Destructor.


unsigned int UDPNetworkMessage::serialize(char* buf, const int& maxBufSize) const {
    unsigned int currLen = 0;
    unsigned int copyMessageType = htonl(messageType);
    unsigned int copySourceId = htonl(sourceId);
    bool copyAckFlag = ackFlag;
    bool copyUserDataFlag = userDataFlag;
    SequenceNumber copyAckSequenceNr = htonl(ackSequenceNr);
    SequenceNumber copyEndOfHoleNr = htonl(endOfHoleNr);
    SequenceNumber copySequenceNr = htonl(sequenceNr);
    unsigned int copyUserDataSize = htonl(userDataSize);
    
    if ((unsigned int)maxBufSize >= messageSize) {
        memcpy(buf+currLen, &copyMessageType, sizeof(copyMessageType));
        currLen += sizeof(copyMessageType);
        
        memcpy(buf+currLen, &copySourceId, sizeof(copySourceId));
        currLen += sizeof(copySourceId);
        
        memcpy(buf+currLen, &copyAckFlag, sizeof(copyAckFlag));
        currLen += sizeof(copyAckFlag);
        
        memcpy(buf+currLen, &copyUserDataFlag, sizeof(copyUserDataFlag));
        currLen += sizeof(copyUserDataFlag);
        
        memcpy(buf+currLen, &copyAckSequenceNr, sizeof(copyAckSequenceNr));
        currLen += sizeof(copyAckSequenceNr);
        
        memcpy(buf+currLen, &copyEndOfHoleNr, sizeof(copyEndOfHoleNr));
        currLen += sizeof(copyEndOfHoleNr);
        
        memcpy(buf+currLen, &copySequenceNr, sizeof(copySequenceNr));
        currLen += sizeof(copySequenceNr);
        
        memcpy(buf+currLen, &copyUserDataSize, sizeof(copyUserDataSize));
        currLen += sizeof(copyUserDataSize);
        
        if (userDataSize > 0) {
            memcpy(buf+currLen, userData, userDataSize);
            currLen += userDataSize;
        }
    }

    return currLen;
} // End of serialize(...).


void UDPNetworkMessage::deserialize(char* msg, const int&) {
    unsigned int tempMessageType = 0;
    messageSize = 0;
    
    if (msg == NULL) {
        userDataSize = 0;
        
        delete userData;
        userData = NULL;
    }
    else {
        memcpy(&tempMessageType, msg+messageSize, sizeof(tempMessageType));
        messageSize += sizeof(tempMessageType);
        messageType = (UDPNetworkMessageType)ntohl(tempMessageType);
        
        memcpy(&sourceId, msg+messageSize, sizeof(sourceId));
        messageSize += sizeof(sourceId);
        sourceId = ntohl(sourceId);
        
        memcpy(&ackFlag, msg+messageSize, sizeof(ackFlag));
        messageSize += sizeof(ackFlag);
        
        memcpy(&userDataFlag, msg+messageSize, sizeof(userDataFlag));
        messageSize += sizeof(userDataFlag);
        
        memcpy(&ackSequenceNr, msg+messageSize, sizeof(ackSequenceNr));
        messageSize += sizeof(ackSequenceNr);
        ackSequenceNr = ntohl(ackSequenceNr);
        
        memcpy(&endOfHoleNr, msg+messageSize, sizeof(endOfHoleNr));
        messageSize += sizeof(endOfHoleNr);
        endOfHoleNr = ntohl(endOfHoleNr);
        
        memcpy(&sequenceNr, msg+messageSize, sizeof(sequenceNr));
        messageSize += sizeof(sequenceNr);
        sequenceNr = ntohl(sequenceNr);
        
        memcpy(&userDataSize, msg+messageSize, sizeof(userDataSize));
        messageSize += sizeof(userDataSize);
        userDataSize = ntohl(userDataSize);
        
        if (userDataSize > 0) {
            // If we already had data in storage, clear it.
            delete[] userData;
            
            userData = new char[userDataSize];
            ECLMPL_ASSERT(userData != NULL);
            memcpy(userData, msg+messageSize, userDataSize);
            messageSize += userDataSize;
        }
        else {
            delete[] userData;
            userData = NULL;
        }
    }
} // End of deserialize(...).

bool UDPNetworkMessage::timedOut(const double& timeOut) const {
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeoutThreshold(timeOut);
    std::chrono::duration<double> timeDifference(0.0);
    
    timeDifference = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - sendTime);
    
    return (timeDifference > timeoutThreshold);
}

UDPNetworkMessageType UDPNetworkMessage::getType() const {
    return messageType;
}

void UDPNetworkMessage::setType(const UDPNetworkMessageType& msgType) {
    messageType = msgType;
}

unsigned int UDPNetworkMessage::getSourceId() const {
    return sourceId;
}

void UDPNetworkMessage::setSourceId(const unsigned int& source) {
    sourceId = source;
}

bool UDPNetworkMessage::getAckFlag() const {
    return ackFlag;
}

void UDPNetworkMessage::setAckFlag(const bool& isAck) {
    ackFlag = isAck;
}

bool UDPNetworkMessage::getUserDataFlag() const {
    return userDataFlag;
}

void UDPNetworkMessage::setUserDataFlag(const bool& isUserData) {
    userDataFlag = isUserData;
}

SequenceNumber UDPNetworkMessage::getAckSequenceNr() const {
    return ackSequenceNr;
}

void UDPNetworkMessage::setAckSequenceNr(const SequenceNumber& nr) {
    ackSequenceNr = nr;
}

SequenceNumber UDPNetworkMessage::getEndOfHoleNr() const {
    return endOfHoleNr;
}

void UDPNetworkMessage::setEndOfHoleNr(const SequenceNumber& nr) {
    endOfHoleNr = nr;
}

SequenceNumber UDPNetworkMessage::getSequenceNr() const {
    return sequenceNr;
}

void UDPNetworkMessage::setSequenceNr(const SequenceNumber& nr) {
    sequenceNr = nr;
}

std::chrono::steady_clock::time_point UDPNetworkMessage::getSendTime() const {
    return sendTime;
}

void UDPNetworkMessage::setSendTime() {
    sendTime = std::chrono::steady_clock::now();
}

unsigned int UDPNetworkMessage::getUserDataSize() const {
    return userDataSize;
}

void UDPNetworkMessage::setUserDataSize(const unsigned int& dataSize) {
    userDataSize = dataSize;
}

unsigned int UDPNetworkMessage::getUserData(char* data, const unsigned int& dataSize) const {
    if (dataSize >= userDataSize) {
        std::copy_n(userData, userDataSize, data);
        return userDataSize;
    }
    
    return 0;
}

void UDPNetworkMessage::setUserData(const char* data, const unsigned int& dataSize) {
    delete[] userData;
    userData = NULL;
    userDataSize = 0;
    
    if (data != NULL && dataSize > 0) {
        userDataSize = dataSize;
        userData = new char[userDataSize];
        std::copy_n(data, dataSize, userData);
    }
}

bool UDPNetworkMessage::operator<(const UDPNetworkMessage& a) const {
    return (sequenceNr < a.sequenceNr);
}

bool UDPNetworkMessage::operator>(const UDPNetworkMessage& a) const {
    return (sequenceNr > a.sequenceNr);
}

std::ostream& operator<< (std::ostream& os, const UDPNetworkMessage& msg) {
    std::string type;
    switch (msg.messageType) {
    case UNDEFINED:
        type = "UNDEFINED";
        break;
    case INITIALIZE:
        type = "INITIALIZE";
        break;
    case START:
        type = "START";
        break;
    case APPLICATION:
        type = "APPLICATION";
        break;
    case FINALIZE:
        type = "FINALIZE";
        break;
    case CONTROL:
        type = "CONTROL";
        break;
    case SYNCH:
        type = "SYNCH";
        break;
    default:
        type = "ERROR-UNKNOWN-TYPE";
        break;
    }

    os << type << ", "
       << "srcId" << msg.sourceId << ", "
       << "ackFlg" << msg.ackFlag << ", "
       << "usrDtaFlg" << msg.userDataFlag << ", "
       << "ackSqNr" << msg.ackSequenceNr << ", "
       << "eOHleNr" << msg.endOfHoleNr << ", "
       << "sqNr" << msg.sequenceNr << ", "
       << "usrDtaSz" << msg.userDataSize << ", \"";

    for (unsigned int i = 0; i < msg.userDataSize; i++) {
        if (msg.userData[i] == '\0')
        { os << "'\\0'"; }
        else if (msg.userData[i] == '\n')
        { os << "'\\n'"; }
        else if (msg.userData[i] >= 32 && msg.userData[i] <= 126)
        { os << msg.userData[i]; }
        else
        { os << "'" << (int)msg.userData[i] << "'"; }
    }
    os << "\"";
    
    return os;
}
