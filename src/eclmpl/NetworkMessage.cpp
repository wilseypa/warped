#include "eclmplCommonInclude.h"
#include "NetworkMessage.h"

const unsigned int userDataSizeSerializedLen = 6;

NetworkMessage::NetworkMessage(const unsigned int& size, char* data) {
    userDataSize = size;
    userData = data;
} // End of constructor.

NetworkMessage::~NetworkMessage() {
    // We delete even though we did not allocate
    if (userData!=0)
    { delete[] userData; }
} // End of destructor.

char*
NetworkMessage::serialize() const {
    char* buf = new char[userDataSizeSerializedLen+userDataSize];
    unsigned int len = userDataSize;
    bool start = false;
    unsigned int a = 0;

    for (unsigned int i = 10000; i > 0; i /= 10) {
        if (len/i > 0) {
            start = true;
        }
        if (start) {
            buf[a] = 48+len/i;
            len = len % i;
            a++;
        }
    }
    buf[a++] = '\0';
    memcpy(buf+a, userData, userDataSize);
    return buf;
} // End of serialize().

void
NetworkMessage::serializedUserDataSize(char* buf, int& nrBytes) const {
    buf = new char[userDataSizeSerializedLen];
    unsigned int len = userDataSize;
    bool start = false;
    unsigned int a = 0;

    for (unsigned int i = 10000; i > 0; i /= 10) {
        if (len/i > 0) {
            start = true;
        }
        if (start) {
            buf[a] = 48+len/i;
            len = len % i;
            a++;
        }
    }
    buf[a++] = '\0';
    nrBytes = userDataSizeSerializedLen;
} // End of serializedSize(...).

void
NetworkMessage::deserialize(char* msg) {
    if (msg == NULL) {
        userDataSize = 0;
        userData = NULL;
    } else {
        userDataSize = strlen(msg)+1;
        if (userDataSize > 0) {
            ECLMPL_ASSERT((userData = new char[userDataSize]) != 0);
            memcpy(userData, msg+userDataSizeSerializedLen, userDataSize);
        } else {
            userData = NULL;
        }
    }
} // End of deserialize(...).

std::ostream&
operator<< (std::ostream& os, const NetworkMessage& msg) {
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
