#include "TCPNetworkMessage.h"

#define USER_DATA_SIZE_SERIALIZED_LEN 6

// Default Constructor
TCPNetworkMessage::TCPNetworkMessage() : userDataSize(0), userData(NULL) {
} // End of Default Constructor.

// Initialization Constructor
TCPNetworkMessage::TCPNetworkMessage(const unsigned int& size, char* data) : userDataSize(size) {
    if (data != NULL && userDataSize > 0) {
        userData = new char[userDataSize];
        std::copy_n(data, userDataSize, userData);
    }
    else {
        userDataSize = 0;
        userData = NULL;
    }
} // End of Initialization Constructor.

// Copy Constructor
TCPNetworkMessage::TCPNetworkMessage(const TCPNetworkMessage& original) : userDataSize(original.userDataSize) {
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
TCPNetworkMessage& TCPNetworkMessage::operator=(const TCPNetworkMessage& rhs) {
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

TCPNetworkMessage::~TCPNetworkMessage() {
    delete[] userData;
} // End of Destructor.

unsigned int TCPNetworkMessage::getUserDataSize() const {
    return userDataSize;
}

void TCPNetworkMessage::setUserDataSize(const unsigned int& dataSize) {
    userDataSize = dataSize;
}

unsigned int TCPNetworkMessage::getUserData(char* data, const unsigned int& dataSize) const {
    if (data != NULL && dataSize >= userDataSize) {
        std::copy_n(userData, userDataSize, data);
        return userDataSize;
    }
    
    return 0;
}

void TCPNetworkMessage::setUserData(const char* data, const unsigned int& dataSize) {
    delete[] userData;
    userData = NULL;
    userDataSize = 0;
    
    if (data != NULL && dataSize > 0) {
        userDataSize = dataSize;
        userData = new char[userDataSize];
        std::copy_n(data, dataSize, userData);
    }
}

std::ostream& operator<< (std::ostream& os, const TCPNetworkMessage& msg) {
    os << msg.userDataSize << ", \"";
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
