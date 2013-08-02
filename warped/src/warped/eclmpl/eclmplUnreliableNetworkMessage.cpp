#include "eclmplCommonInclude.h"
#include "eclmplUnreliableNetworkMessage.h"

// This is defined in "warped.h" so when we compile this within warped
// we cannot define it here.
//const char DELIMITER = ' ';

eclmplUnreliableNetworkMessage::eclmplUnreliableNetworkMessage() {
  hdr.type = UNDEFINED;
  hdr.sourceId = 0;
  hdr.ackFlag = false;
  hdr.userDataFlag = false;
  hdr.ackSequenceNr = 0;
  hdr.endOfHoleNr = 0;
  hdr.sequenceNr = 0;
  userDataSize = 0;
  userData = NULL;
  sendTime.tv_sec = 0;
  sendTime.tv_usec = 0;
} // End of constructor.

eclmplUnreliableNetworkMessage::eclmplUnreliableNetworkMessage(const eclmplUnreliableNetworkMessageType &msgType,
				     const unsigned int &source, 
				     const bool &isAck, 
				     const bool &isUserData, 
				     const SequenceNumber &ackSqNr, 
				     const SequenceNumber &endHoleNr,
				     const SequenceNumber &sqNr, 
				     //const short &windowSize, 
				     const int &dataSize, 
				     char *data) {
  hdr.type = msgType;
  hdr.sourceId = source;
  hdr.ackFlag = isAck;
  hdr.userDataFlag = isUserData;
  hdr.ackSequenceNr = ackSqNr;
  hdr.endOfHoleNr = endHoleNr;
  hdr.sequenceNr = sqNr;
  //advertisedWindow = windowSize;
  userDataSize = dataSize;
  userData = data;  
  sendTime.tv_sec = 0;
  sendTime.tv_usec = 0;
} // End of constructor.

eclmplUnreliableNetworkMessage::~eclmplUnreliableNetworkMessage() {
  // We don't allocate it, so we don't delete it!
} // End of destructor.


int
eclmplUnreliableNetworkMessage::serialize( char *buf, const int & ) const {
  int currLen = sizeof(eclmplUnreliableNetworkMessageHeader);
  eclmplUnreliableNetworkMessageHeader srlHdr = hdr;
  srlHdr.sourceId = htonl(srlHdr.sourceId); // Convert to network byte order unsigned int.
  srlHdr.ackSequenceNr = htonl(srlHdr.ackSequenceNr);
  srlHdr.endOfHoleNr = htonl(srlHdr.endOfHoleNr);
  srlHdr.sequenceNr = htonl(srlHdr.sequenceNr);
  int srlDataSize = htonl(userDataSize);

  memcpy(buf, (char *)&srlHdr, currLen);
  memcpy(buf+currLen, (char *)&srlDataSize, sizeof(srlDataSize));
  currLen += sizeof(srlDataSize);
  memcpy(buf+currLen, userData, userDataSize);
  currLen += userDataSize;

  return currLen;
} // End of serialize(...).


void
eclmplUnreliableNetworkMessage::deserialize( char *msg, const int & ){
  if(msg == NULL){
    userDataSize = 0;
    userData = NULL;
  }
  else {
    memcpy((unsigned char *)&hdr, msg, sizeof(eclmplUnreliableNetworkMessageHeader));
    memcpy((unsigned char *)&userDataSize, msg+sizeof(eclmplUnreliableNetworkMessageHeader), sizeof(userDataSize));
    hdr.sourceId = ntohl(hdr.sourceId); // Convert to network byte order unsigned short int.
    hdr.ackSequenceNr = ntohl(hdr.ackSequenceNr);
    hdr.endOfHoleNr = ntohl(hdr.endOfHoleNr);
    hdr.sequenceNr = ntohl(hdr.sequenceNr);
    userDataSize = ntohl(userDataSize);
    if (userDataSize > 0) {
      userData = cppStrDup( msg 
			    + sizeof(eclmplUnreliableNetworkMessageHeader)
			    + sizeof(userDataSize),
			    userDataSize );
    }
    else {
      userData = NULL;
    }
  }
} // End of deserialize(...).

ostream&
operator<< (ostream& os, const eclmplUnreliableNetworkMessage& msg) {
  string type;
  switch(msg.hdr.type) {
  case UNDEFINED: type = "UNDEFINED";
    break;
  case INITIALIZE: type = "INITIALIZE";
    break;
  case START: type = "START";
    break;
  case APPLICATION: type = "APPLICATION";
    break;
  case FINALIZE: type = "FINALIZE";
    break;
  case CONTROL: type = "CONTROL";
    break;
  case SYNCH: type = "SYNCH";
    break;
  default: type = "ERROR-UNKNOWN-TYPE";
    break;
  }

  os << type << ", "
     << "srcId" << msg.hdr.sourceId << ", "
     << "ackFlg" << msg.hdr.ackFlag << ", " 
     << "usrDtaFlg" << msg.hdr.userDataFlag << ", "
     << "ackSqNr" << msg.hdr.ackSequenceNr << ", "
     << "eOHleNr" << msg.hdr.endOfHoleNr << ", "
     << "sqNr" << msg.hdr.sequenceNr << ", "
     << "usrDtaSz" << msg.userDataSize << ", \"";

  for (unsigned int i = 0; i < msg.userDataSize; i++) {
    if (msg.userData[i] == '\0')
      os << "'\\0'";
    else if (msg.userData[i] == '\n')
      os << "'\\n'";
    else if (msg.userData[i] >= 32 && msg.userData[i] <= 126)
      os << msg.userData[i];
    else 
      os << "'" << (int)msg.userData[i] << "'";
  }
  os << "\"";
  return os;
}
