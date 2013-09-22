//-*-c++-*-
#include "../include/RAIDFork.h"
#include "../include/RAIDForkState.h"
#include "IntVTime.h"

RAIDFork::RAIDFork(string &myName, int numOutputs, vector<string> outNames,
		   int disks, int startDisk)  :
  objectName(myName), numberOfOutputs(numOutputs), outputNames(outNames),
  numDisks(disks), startDiskId(startDisk) {
} // End of RAIDFork()

RAIDFork::~RAIDFork() {
  deallocateState(getState());
} // End of ~RAIDFork()


void
RAIDFork::initialize() {
  outputHandles.resize(outputNames.size(),NULL);
  for(int i = 0; i < outputHandles.size(); i++){
    outputHandles[i] = getObjectHandle(outputNames[i]);
  }
} // End of initialize()

void
RAIDFork::finalize() {
} // End of finalize()

void
RAIDFork::executeProcess() {
  RAIDRequest *recvEvent;
  RAIDForkState *myState;
  RAIDRequest *newEvent;

  IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());

  int timeDelay = 1;
  IntVTime ldelay(sendTime + timeDelay++);
  int receiveDisk;

  while(true == haveMoreEvents()) {
    recvEvent = (RAIDRequest *) getEvent();

    if ( recvEvent != NULL ) {
      myState = (RAIDForkState*) getState();
      newEvent = new RAIDRequest(sendTime, ldelay, this, this);
      *newEvent = *recvEvent;

      int firstStripeUnit = newEvent->getStartStripe();

      ObjectID sourceObjId = *(getObjectHandle(recvEvent->getSourceObject())->getObjectID());
      if ( recvEvent->getSender() == sourceObjId ) {
	if (recvEvent->getRead() == true) {
	  int parityDiskId;
	  for (int count = 0; count < recvEvent->getSizeRead(); count++) {
	    newEvent->setStartStripe(firstStripeUnit + count);

	    calculateStripeInfo(newEvent, parityDiskId, receiveDisk);

            delete newEvent;
            newEvent = new RAIDRequest(sendTime, ldelay, this, outputHandles[receiveDisk]);

            *newEvent = *recvEvent;
            newEvent->setStartStripe(firstStripeUnit + count);
	    (outputHandles[receiveDisk])->receiveEvent(newEvent);

	    newEvent = new RAIDRequest(sendTime, ldelay, this, this );

	    *newEvent = *recvEvent;
	  }
	  if (newEvent->getSizeRead() > 0) {
	    delete newEvent; // Get rid of unused one
	  }
	}
	else {
	  // Its a write, so send the paritys.
	  // See how many parity messages needed.
	  int numParity = 0;
	  int parityDiskId;

	  // Kludge
	  // calculate the number of parity messages needed
	  int lastParityDiskId  = -1;
	  int count;
	  for (count = 0; count < newEvent->getSizeRead(); count++) {
	    parityDiskId = getParityDiskId(firstStripeUnit + count);

	    if (parityDiskId != lastParityDiskId) {
	      numParity++;
	      lastParityDiskId = parityDiskId;
	    }
	  }

	  lastParityDiskId = -1;
	  for (count = 0; count < newEvent->getSizeRead(); count++) {
	    newEvent->setStartStripe(firstStripeUnit + count);
	    calculateStripeInfo(newEvent, parityDiskId, receiveDisk);

            delete newEvent;
            newEvent = new RAIDRequest( sendTime, ldelay, this, outputHandles[receiveDisk] );

            *newEvent = *recvEvent;
            newEvent->setStartStripe(firstStripeUnit + count );

	    newEvent->setParityMsg(false);
	    newEvent->setSizeParity(numParity);

	    // Do we need different parity block yet.
	    if (parityDiskId != lastParityDiskId) {

	      lastParityDiskId = parityDiskId;
	      RAIDRequest *parityMsg = new RAIDRequest(sendTime,
                                                       ldelay,
                                                       this,
                                                       outputHandles[parityDiskId%numberOfOutputs]);

              // send a request for the new information.
	      *parityMsg = *newEvent;
	      parityMsg->setParityMsg(true);

	      (outputHandles[parityDiskId%numberOfOutputs])->receiveEvent(parityMsg);
	    }

	    (outputHandles[receiveDisk])->receiveEvent(newEvent);

	    newEvent = new RAIDRequest(sendTime,
                                       ldelay,
                                       this,
                                       this);

	    *newEvent = *recvEvent;

	  }// for (count = 0; count < newEvent->sizeRead; count++)
	  delete newEvent; // Get rid of last unsent message
	} // if (read)
      } // if (sender == source)
    } // End of if (event != null)
  } // End of while "have more events" loop
} // End of executePRocess()

int
RAIDFork::getParityDiskId(int stripeUnit) {
  // This function calculates the placement of the parity information
  // for the stripe. This function implements a right--symmetric parity
  // placement policy where the parity information for the first disk
  // is placed on the last disk, and the parity information for the next
  // stripe is placed on the next to last disk (and so on). see RAID
  // documentation for a descriptive example.
  if (numDisks > 1) {
    return(numDisks - (stripeUnit/(numDisks-1)) % numDisks-1);
  }
  else {
    return 0; // ??? or 1???
  }
} // End of getParityDiskId()

void
RAIDFork::calculateStripeInfo(RAIDRequest *event, int &parityDisk, int &receiveDisk) {
  // This function calculates the placement of the stripe unit on the
  // disk array.
  // The stripes are laid across the disk array and one stripe will act
  // as the parity stripe. Each sector on the disk is given a unique
  // number called the logical sector number.

  int logicalSector;
  if (numDisks > 1) {
    logicalSector = event->getStartStripe()/(numDisks - 1);
  }
  else {
    logicalSector = event->getStartStripe();
  }
  receiveDisk = event->getStartStripe()%numberOfOutputs;
  parityDisk = getParityDiskId(event->getStartStripe());
  if (receiveDisk > parityDisk) {
    receiveDisk++;
    receiveDisk %= numberOfOutputs;
  }
} // End of calculateStripeInfo()

State*
RAIDFork::allocateState() {
  return new RAIDForkState();
} // End of allocateState()

void
RAIDFork::deallocateState(const State* state){
  delete state;
} // End of deallocateState()

void
RAIDFork::reclaimEvent(const Event *event){
   delete event;
} // End of reclaimEvent()

void
RAIDFork::reportError(const string&, SEVERITY){
} // End of reportError()
