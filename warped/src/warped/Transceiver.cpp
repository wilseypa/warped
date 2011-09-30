// See copyright notice in file Copyright in the root directory of this archive.

#include "Transceiver.h"
#include "MsgAggregatingCommunicationManager.h"
using std::cerr;
using std::endl;

// The first position in the buffer will always be how many messages are
// being sent. Therefore, when resetting the values of currentPosition
// and curSize, reset them to these initial values.
Transceiver::Transceiver() : numberOfMsgs( 0 ),
			     lpId( 0 ),
			     currentPosition( &buffer[sizeof(unsigned int)] ),
			     messageManagerHandle( 0 ),
			     commPhyLib( 0 ),
                             curSize(sizeof(unsigned int)){}

Transceiver::~Transceiver() {}

void
Transceiver::setLpId(unsigned int id) {
  lpId = id;
}

void
Transceiver::setMessageManagerHandle(MessageManager* msgMgrPtr) {
  messageManagerHandle = msgMgrPtr;
}

unsigned int
Transceiver::readMsgSize() {
  unsigned int* ptr;
  ptr = (unsigned int*)currentPosition;
  // The following operation is REALLY COMPILER, ARCHITECTURE dependent
  unsigned int val = *ptr;
  ptr++;
  currentPosition = (char*) ptr;
  return val;
}

SerializedInstance *
Transceiver::readMessage() {
  unsigned int msgSize = readMsgSize();

  SerializedInstance *retval = new SerializedInstance(currentPosition, msgSize);

  currentPosition = currentPosition + msgSize;
  numberOfMsgs--;

  return retval;
}


void
Transceiver::writeMsgSize( unsigned int msgSize ){
  unsigned int* ptr;
  ptr = (unsigned int*)currentPosition;
  // The following operation is REALLY COMPILER, ARCHITECTURE dependent
  *ptr = msgSize;
  ptr++;
  currentPosition = (char*) ptr;
  curSize = curSize + sizeof(unsigned int);
}

void
Transceiver::writeMessage(SerializedInstance* message) {
  unsigned int msgSize = message->getSize();
  writeMsgSize(msgSize);

  memcpy(currentPosition, &message->getData()[0], msgSize);

  numberOfMsgs++;
  curSize = curSize + msgSize;
  currentPosition = currentPosition + msgSize;
}

void
Transceiver::sendMessage() {
  unsigned int *ptr = (unsigned int*)buffer;
  *ptr = numberOfMsgs;

  SerializedInstance *toSend = new SerializedInstance( buffer, curSize );

  commPhyLib->physicalSend(toSend, lpId);

  // Reset the values.
  numberOfMsgs = 0;
  currentPosition = &buffer[sizeof(unsigned int)];
  curSize = sizeof(unsigned int);
}

void
Transceiver::receiveMessage() {
  numberOfMsgs = 0;
  currentPosition = buffer;
  SerializedInstance *newMsg = commPhyLib->physicalProbeRecv();
  if( newMsg != 0 ){
    if( newMsg->getSize() > INTERNAL_BUFFER_SIZE + sizeof(int) ){
      cerr << "Transceiver::receiveMessage message size greater "
	   << "than INTERNAL_BUFFER_SIZE+sizeof(int):"
	   << INTERNAL_BUFFER_SIZE+sizeof(int) << endl;
      abort();
    }
    else{
      unsigned int msgSize = newMsg->getSize();
      const vector<char> &data = newMsg->getData();
      unsigned int *temp = (unsigned int *)(&data[0]);
      numberOfMsgs = *temp;

      memcpy(buffer, &data[sizeof(unsigned int)], msgSize - sizeof(unsigned int));

      delete newMsg;
    }
  }
}

bool
Transceiver::canWriteMessage(int size) {
  if((currentPosition + size) < ((char*)buffer + INTERNAL_BUFFER_SIZE)) {
    return true;
  }
  else {
    return false;
  }
}

bool
Transceiver::canReceiveMessage(int) {
  if(numberOfMsgs == 0) {
    return true;
  }
  else {
    return false;
  }
}

void
Transceiver::probeToSend() {
  if(numberOfMsgs > 0) {
    sendMessage();
  }
}

void
Transceiver::probeToReceive() {
  if(numberOfMsgs == 0) {
    receiveMessage();
  }
}

void
Transceiver::setCommPhyInterface(PhysicalCommunicationLayer *phyLib){
  commPhyLib = phyLib;
}
