//-*-c++-*-
#include <stdio.h>
#include "../include/RAIDProcess.h"
#include "../include/RAIDProcessState.h"
#include "IntVTime.h"
#include "TimeWarpSimulationManager.h"

// maxdisks == # of total disks in the simulation
RAIDProcess::RAIDProcess(string &myName, string &outName, int maxdisks,
		DISK_TYPE disk, int maxrequests, int firstdisk, double initSeed) :
	objectName(myName), outputName(outName), maxDisks(maxdisks),
			maxRequests(maxrequests), firstDiskId(0), diskType(disk),
			seed(initSeed) {
} // End of RAIDProcess()

RAIDProcess::~RAIDProcess() {
	deallocateState(getState());
} // End of ~RAIDProcess()

void RAIDProcess::initialize() {
	RAIDProcessState *myState = (RAIDProcessState*) getState();
	int processNumber = getObjectID()->getSimulationObjectID();
	IntVTime sendTime = dynamic_cast<const IntVTime&> (getSimulationTime());

	delete (myState->getGen());
	MLCG *gen = new MLCG(seed, seed + 1);
	myState->setGen(gen);

	outputHandle = getObjectHandle(outputName);

	newConfiguration();

	RAIDRequest* newRequest = new RAIDRequest(sendTime, sendTime + 1, this,
			this);
	newRequest->setSourceObject(getName());
	newRequest->setSinkObject("");
	newRequest->setStartCylinder(-1);
	newRequest->setStartStripe(-1);
	newRequest->setStartSector(-1);
	newRequest->setSizeRead(1);
	newRequest->setLogicalSector(-1);
	newRequest->setRead(true);
	newRequest->setParityMsg(false);
	newRequest->setBeginningOfStrype(-1);

	myState->setBeginningOfStrype(newRequest->getBeginningOfStrype());
	myState->setStopProcessing(false);

	this->receiveEvent(newRequest);
}

void RAIDProcess::finalize() {
	RAIDProcessState *myState = dynamic_cast<RAIDProcessState *> (getState());
	cout << getName() << " Finished:\n" << "Number of Requests: "
			<< myState->getNumRequests() << " Reads: "
			<< myState->getNumReads() << " Writes " << myState->getNumWrites()
			<< "\n";
} // End of finalize().

void RAIDProcess::executeProcess() {

	RAIDProcessState *myState = (RAIDProcessState*) getState();
	RAIDRequest* recvEvent;

	while (true == haveMoreEvents()) {
		recvEvent = (RAIDRequest*) getEvent();
		// int senderSimObjID = getObjectHandle(outputName)->getObjectID()->getSimulationObjectID();
		if (recvEvent != NULL) {
			/*#if WHATISTHEPURPOSEOFTHIS
			 if ( myState->getStopProcessing() == true) {
			 return;
			 }
			 if (recvEvent->getTokenNumber() != myState->getNumRequests()) {
			 myState->setStopProcessing(true);
			 return;
			 }
			 #endif*/

			/* This is not correct anymore. Because the number of disk requests
			 outstanding is not critical, I am not fixing it at this time.
			 // keep memory from being munged on errors and first event
			 if ( (senderSimObjID - firstDiskId) >= 0 &&
			 (senderSimObjID - firstDiskId) < (unsigned) maxDisks) {
			 myState->diskRequests[senderSimObjID-firstDiskId]++;
			 }
			 else if (senderSimObjID != getObjectID()->getSimulationObjectID()) {
			 //cldebug::debugout << objectName << " ignoring event from " << senderSimObjID
			 //  << " in diskRequests count\n";
			 }*/
	//		cout << "State Size before::" << myState->get_size()
	//				<< "  Parity Size before :: " << myState->getParitySize()
	//				<< endl;
			// If this is a write then parity information will be expected.
			if ((recvEvent->getRead() == false) && myState->getParitySize()
					== -1) {
				// our first write message comes back home, lets see how many
				// paritys were generated.
				//cout << "Waiting for First parity of Write" << endl;
				myState->setParitySize(recvEvent->getSizeParity());
	//			cout << "Parity Size = " << myState->getParitySize() << endl;
			}

			if (recvEvent->getParityMsg() == true) {
				// Its a parity msg so decrement the parity
				myState->setParitySize(myState->getParitySize() - 1);
			} else {
				myState->set_size(myState->get_size() - 1);
			}
			//Print Current State
//			cout << "State Size After::" << myState->get_size()
//					<< "  Parity Size After:: " << myState->getParitySize()
//					<< endl;
			if (recvEvent->getRead() == true) {
				// see if the operation is complete
				if ((myState->get_size() == 0) && (myState->getParitySize()
						== -1)) {
					myState->setDiskOperationPending(false);
					myState->setNumReads(myState->getNumReads() + 1);
					myState->set_size(0);

					for (int i = 0; i < maxDisks; i++) {
						myState->diskRequests[i] = 0;
					}
//					cout
//							<< "Read Completed------------------------------------------------------------------------------------"
//							<< endl;
					if (myState->getNumRequests() < maxRequests) {
						newMsg();
					}
				}
			} else {
				// event is a write
				// see if the operation is complete
				if ((myState->get_size() == 0)) {
					if ((myState->getParitySize() == 0)) {
						myState->setDiskOperationPending(false);
						myState->setNumWrites(myState->getNumWrites() + 1);
						myState->set_size(0);
						myState->setParitySize(-1);
//						cout
//								<< "Write Completed------------------------------------------------------------------------------------"
//								<< endl;
						for (int i = 0; i < maxDisks; i++) {
							myState->diskRequests[i] = 0;
						}

						if (myState->getNumRequests() < maxRequests) {
							newMsg();
						}
					} else {
						//cout << "The Partity Condition is not met" << endl;
					}
				} // size and parity size is not equal to zero
			} // else (write)
		}
	} // while (haveMoreEvents() == true)
}

void RAIDProcess::newConfiguration() {

	RAIDProcessState *myState = (RAIDProcessState*) getState();
	myState->setFirstDiskId(firstDiskId);

	switch (diskType) {
	case FUJITSU:
		maxCylinder = 1944;
		maxTrack = 20;
		maxSector = 88;
		break;
	case FUTUREDISK:
		maxCylinder = 2500;
		maxTrack = 20;
		maxSector = 132;
		break;
	case LIGHTNING:
		maxCylinder = 949;
		maxTrack = 14;
		maxSector = 48;
		break;
	}

	totalStripeSize = (maxDisks - 1) * maxCylinder * maxTrack * maxSector;
	//  MAXSTRIPESIZE = 2*MAXDISKS;
	maxStripeSize = 2 * maxDisks;

	DiscreteUniform read(0, 1, myState->getGen());

	// Add some randomness to the simulation.
	for (int i = 0; (unsigned) i < getObjectID()->getSimulationObjectID() % 40; i++) {
		read();
	}

	// Ignore the first read completion.
	myState->setNumReads(myState->getNumReads() - 1);

	// make the process look for the first Request.
	myState->set_size(1);

	// set the number of disks in the state.
	myState->setNumDisks(maxDisks);

	// set Disk Requests
	//myState->setDiskRequests(maxDisks); // = new int[MAXDISKS];
	// set Disk Requests
	myState->diskRequests = new int[maxDisks];

	for (int i = 0; i < maxDisks; i++) {
		myState->diskRequests[i] = 0; // [i] = 0;
	}
}

void RAIDProcess::newMsg() {
	RAIDProcessState *myState = (RAIDProcessState*) getState();
	int ldelay = msgDelay();
	IntVTime sendTime = dynamic_cast<const IntVTime&> (getSimulationTime());
	RAIDRequest* newRequest = new RAIDRequest(sendTime, sendTime + ldelay + 1,
			this, outputHandle);

	newRequest->setSourceObject(getName());
	newRequest->setSinkObject("");
	newRequest->setStartCylinder(-1);

	DiscreteUniform stripe(0, totalStripeSize, myState->getGen());
	DiscreteUniform read(0, 1, myState->getGen());
	DiscreteUniform size(1, maxStripeSize, myState->getGen());

	newRequest->setStartStripe((int) stripe());
	newRequest->setStartSector(-1);
	newRequest->setSizeRead((int) size());
	newRequest->setLogicalSector(-1);

	if (newRequest->getSizeRead() == 0) {
		cerr << "\n\n\n\nHold IT! sizeRead = 0\n\n\n\n\n" << "\n";
	}

	// If we try and read off the end of the array then move our starting
	// location, so that we can still read the same amount of data.
	if ((newRequest->getStartStripe() + newRequest->getSizeRead())
			>= totalStripeSize) {
		newRequest->setStartStripe(maxStripeSize - newRequest->getSizeRead());
	}

	newRequest->setBeginningOfStrype(newRequest->getStartStripe());
	newRequest->setTokenNumber(myState->getNumRequests() + 1);
	myState->setBeginningOfStrype(newRequest->getBeginningOfStrype());
	myState->set_size(newRequest->getSizeRead());
	newRequest->setSizeParity(0);
	newRequest->setParityMsg(false);

	if (read() == 0) {
		newRequest->setRead(false);
//		cout
//				<< "Write Started of size----------------------------------------------------------------------------------  "
//				<< myState->get_size() << endl;

		myState->setRead(false);
	} else {
		newRequest->setRead(true);
		myState->setRead(true);
//		cout
//				<< "Read Started of size----------------------------------------------------------------------------------  "
//				<< myState->get_size() << endl;
	}

	myState->setDiskOperationPending(true);
	myState->setBeginningOfStrype(newRequest->getStartStripe());
	myState->setStrypeSize(newRequest->getSizeRead());
	myState->setNumRequests(myState->getNumRequests() + 1);

	getObjectHandle(outputName)->receiveEvent(newRequest);
} // End of newMsg()

int RAIDProcess::msgDelay() {
	RAIDProcessState *myState = (RAIDProcessState*) getState();
	double ldelay;

	// Hard coded for now... add to ssl later
	sourcedist = UNIFORM;
	first = 1;
	second = 2;

	switch (sourcedist) {
	case UNIFORM: {
		Uniform uniform(first, second, myState->getGen());
		ldelay = uniform();
		break;
	}
	case NORMAL: {
		Normal normal(first, second, myState->getGen());
		ldelay = normal();
		break;
	}
	case BINOMIAL: {
		Binomial binomial((int) first, second, myState->getGen());
		ldelay = binomial();
		break;
	}
	case POISSON: {
		Poisson poisson(first, myState->getGen());
		ldelay = poisson();
		break;
	}
	case EXPONENTIAL: {
		NegativeExpntl expo(first, myState->getGen());
		ldelay = expo();
		break;
	}
	case FIXED:
		ldelay = (int) first;
		break;
	default:
		ldelay = 0;
		cerr << "Improper Distribution for a Source Object!!!" << "\n";
		break;
	}
	return ((int) ldelay);

}

State*
RAIDProcess::allocateState() {
	return new RAIDProcessState();
} // End of allocateState()

void RAIDProcess::deallocateState(const State* state) {
	delete state;
} // End of deallocateState()

void RAIDProcess::reclaimEvent(const Event *event) {
	delete event;
} // End of reclaimEvent()

void RAIDProcess::reportError(const string&, SEVERITY) {
} // End of reportError()
