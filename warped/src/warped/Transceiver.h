#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"

class SerializedInstance;

#define INTERNAL_BUFFER_SIZE 50000
#define MAXIMUM_AGE 10

enum AggregationMode { AGGREGATING_MODE, AGGRESSIVE_SEND, AGGREGATION_ERROR };

class MessageManager;
class PhysicalCommunicationLayer;

struct Transceiver {

public:

  Transceiver();
  ~Transceiver();

  /* AFAICS, this struct is unused currently
  struct InputMessageInfo {
    VTime& receivedMessageSendTime;
    VTime& myLVT;
  };
  */
   
  /* AFAICS, this struct is unused currently
  struct OutputMessageInfo {
    VTime& leastReceiveTime;
  };
  */
   
  char buffer[INTERNAL_BUFFER_SIZE];

  void setLpId(unsigned int id);
  void setMessageManagerHandle(MessageManager* msgMgrPtr);
  unsigned int  readMsgSize();
  SerializedInstance *readMessage();
  void writeMsgSize(unsigned int msgSize);
  void writeMessage(SerializedInstance *message);
  void sendMessage();
  void receiveMessage();
  bool canWriteMessage(int size);
  bool canReceiveMessage(int);
  void probeToSend();
  void probeToReceive();
  void setCommPhyInterface(PhysicalCommunicationLayer *phyLib);

  int numberOfMsgs;

private:
  int lpId;
  char *currentPosition;
  unsigned int curSize;
   
  // communication interface
  MessageManager* messageManagerHandle;
  PhysicalCommunicationLayer *commPhyLib;
};

#endif
