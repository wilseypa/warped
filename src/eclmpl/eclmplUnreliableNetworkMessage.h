#ifndef ECLMPL_UNRELIABLE_NETWORK_MESSAGE_H
#define ECLMPL_UNRELIABLE_NETWORK_MESSAGE_H

#include <stddef.h>                     // for NULL
#include <sys/time.h>                   // for timeval, gettimeofday
#include <iosfwd>                       // for ostream

#include "NetworkMessage.h"             // for NetworkMessage
#include "eclmplCommonInclude.h"        // for SequenceNumber

enum eclmplUnreliableNetworkMessageType {UNDEFINED, INITIALIZE, START, APPLICATION, CONTROL, FINALIZE, SYNCH};

struct eclmplUnreliableNetworkMessageHeader {
    eclmplUnreliableNetworkMessageType type; // e.g. INIT, FINALIZE, APPLICATION

    unsigned int sourceId;

    bool ackFlag;
    bool userDataFlag;
    SequenceNumber ackSequenceNr;
    SequenceNumber endOfHoleNr;
    SequenceNumber sequenceNr;
};

/** The eclmplUnreliableNetworkMessage class.

*/
class eclmplUnreliableNetworkMessage : public NetworkMessage {
public:
    /// Default Constructor.
    eclmplUnreliableNetworkMessage();

    eclmplUnreliableNetworkMessage(const eclmplUnreliableNetworkMessageType&,
                                   const unsigned int&, const bool&, const bool&,
                                   const SequenceNumber&, const SequenceNumber&,
                                   const SequenceNumber&,
                                   //const short &,
                                   const int&, char*);

    /// Destructor.
    ~eclmplUnreliableNetworkMessage();

    //char *serialize() const;
    int serialize(char* buf, const int& maxBufSize) const;
    //void deserialize(char *msg);
    void deserialize(char* msg, const int& msgLen);

    inline void setType(const eclmplUnreliableNetworkMessageType& msgType) { hdr.type = msgType; }
    inline eclmplUnreliableNetworkMessageType getType() const { return hdr.type; }
    inline void setSourceId(const unsigned int& source) { hdr.sourceId = source; }
    inline unsigned int getSourceId() const { return hdr.sourceId; }
    inline void setAckFlag(const bool& isAck) { hdr.ackFlag = isAck; }
    inline bool getAckFlag() const { return hdr.ackFlag; }
    inline void setUserDataFlag(const bool& isUserData) { hdr.userDataFlag = isUserData; }
    inline bool getUserDataFlag() const { return hdr.userDataFlag; }
    inline void setAckSequenceNr(const SequenceNumber& nr) { hdr.ackSequenceNr = nr; }
    inline SequenceNumber getAckSequenceNr() const { return hdr.ackSequenceNr; }
    inline void setEndOfHoleNr(const SequenceNumber& nr) { hdr.endOfHoleNr = nr; }
    inline SequenceNumber getEndOfHoleNr() const { return hdr.endOfHoleNr; }
    inline void setSequenceNr(const SequenceNumber& nr) { hdr.sequenceNr = nr; }
    inline SequenceNumber getSequenceNr() const { return hdr.sequenceNr; }
    //void setAdvertisedWindow(const short &windowSize) { advertisedWindow = windowSize; }
    //short getAdvertisedWindow() const { return advertisedWindow; }
    inline int setSendTime() { return gettimeofday(&sendTime, NULL); }
    inline bool timedOut(const struct timeval& currTime, const double& timeOut) {
        return (((currTime.tv_sec*1.0+currTime.tv_usec/1.0e+6) -
                 (sendTime.tv_sec*1.0+sendTime.tv_usec/1.0e+6)) > timeOut);
    }
    inline struct timeval getSendTime() const { return sendTime; }

    inline bool operator<(const eclmplUnreliableNetworkMessage& a) { return hdr.sequenceNr < a.getSequenceNr(); }
    inline bool operator>(const eclmplUnreliableNetworkMessage& a) { return hdr.sequenceNr > a.getSequenceNr(); }

    friend std::ostream& operator<< (std::ostream& os, const eclmplUnreliableNetworkMessage& msg);
protected:
    eclmplUnreliableNetworkMessageHeader hdr;
    struct timeval sendTime;
    /// Advertised window size for number of NetworkMessages, not size in bytes.
    //short advertisedWindow;
};
#endif
