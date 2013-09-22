//-*-c++-*-
#include <iostream>
#include <math.h>
#include "../include/RAIDRequest.h"
#include "../include/RAIDDisk.h"
#include "../include/RAIDDiskState.h"
#include "IntVTime.h"
#include <map>

RAIDDisk::RAIDDisk(string &myName, DISK_TYPE theDisk) :
	objectName(myName) {
	switch (theDisk) {
	case FUJITSU:
		revolutionTime = 11.1;
		minSeekTime = 2;
		avgSeekTime = 11;
		maxSeekTime = 22;
		numCylinders = 1944;
		tracksPerCyl = 20;
		sectorsPerTrack = 88;
		break;
	case FUTUREDISK:
		revolutionTime = 9.1;
		minSeekTime = 1.8;
		avgSeekTime = 10;
		maxSeekTime = 20;
		numCylinders = 2500;
		tracksPerCyl = 20;
		sectorsPerTrack = 132;
		break;
	case LIGHTNING:
		revolutionTime = 13.9;
		minSeekTime = 2;
		avgSeekTime = 12.5;
		maxSeekTime = 25;
		numCylinders = 949;
		tracksPerCyl = 14;
		sectorsPerTrack = 48;
		break;
	}
    lastEventTime["test"]=120;
	//cout << "Initial LastEvent Time for the Disk = " << lastEventTime;
} // End of RAIDDisk()

RAIDDisk::~RAIDDisk() {
	deallocateState(getState());
}

void RAIDDisk::initialize() {
}

void RAIDDisk::finalize() {
}

void RAIDDisk::executeProcess() {
	RAIDRequest *recvEvent;
	RAIDDiskState *myState;
	RAIDRequest *newEvent;
	RAIDRequest *tmpEvent;
	IntVTime recvTime(0);
	int seekDist;
	double seekTime, a, b, c;
	int nextCylinder;
	int timeDelay = 1;
    int objectId;

	IntVTime sendTime = dynamic_cast<const IntVTime&> (getSimulationTime());

	myState = (RAIDDiskState *) getState();

	while (true == haveMoreEvents()) {
		recvEvent = (RAIDRequest*) getEvent();

		if (recvEvent != NULL) {
			SimulationObject *recvr = getObjectHandle(
					recvEvent->getSourceObject());
            string receiverName = recvr->getName();
            objectId=recvr->getObjectID()->getSimulationObjectID();
            tmpEvent = new RAIDRequest(sendTime, sendTime + 1, this, recvr);
            *tmpEvent = *recvEvent;

			nextCylinder = (tmpEvent->getStartStripe() / (sectorsPerTrack
					* tracksPerCyl)) % numCylinders;
			tmpEvent->setStartCylinder(nextCylinder);
			tmpEvent->setStartSector(
					tmpEvent->getStartStripe() % sectorsPerTrack);

			seekDist = nextCylinder - myState->getCurrentCylinder();

			myState = (RAIDDiskState*) getState();

			if (seekDist < 0) {
				seekDist = abs(seekDist);
			}
			if (seekDist != 0) {
				a = (-10 * minSeekTime + 15 * avgSeekTime - 5 * maxSeekTime)
						/ (3 * sqrt(numCylinders));
				b = (7 * minSeekTime - 15 * avgSeekTime + 8 * maxSeekTime) / (3
						* numCylinders);
				c = minSeekTime;
				if (a > 0 && b > 0) {
					seekTime = a * sqrt(seekDist - 1) + b * (seekDist - 1) + c;
				} else {
					cerr << "Disk Model parameters are not correct for model"
							<< endl;
					seekTime = 0;
				}
			} else {
				seekTime = 0;
			}

			recvTime = max(sendTime.getApproximateIntTime(), lastEventTime[receiverName])
					+ int(seekTime + 0.5 * revolutionTime);
           // recvTime = lastEventTime + int(seekTime + 0.5 * revolutionTime);

			newEvent = new RAIDRequest(sendTime, recvTime, this, recvr);
			//delete lastEventTime;
			lastEventTime[receiverName] = recvTime.getApproximateIntTime();
			//cout << "LastEvent Time for the Disk = " << lastEventTime << endl;
#if 0
			recvTime = VTime(getSimulationTime() +
					int(seekTime + 0.5*revolutionTime) + timeDelay++);
			newEvent = new RAIDRequest(sendTime, recvTime, this, recvr);
#endif
			*newEvent = *tmpEvent;

			delete tmpEvent;

			myState->setCurrentCylinder(nextCylinder);

			// Send event back to source.
			recvr->receiveEvent(newEvent);
		} // End of if (event != null)
	} // End of while "have more events" loop
}

State*
RAIDDisk::allocateState() {
	return new RAIDDiskState();
} // End of allocateState()

void RAIDDisk::deallocateState(const State* state) {
	delete state;
} // End of deallocateState()

void RAIDDisk::reclaimEvent(const Event *event) {
	delete event;
} // End of reclaimEvent()

void RAIDDisk::reportError(const string&, SEVERITY) {
} // End of reportError()
