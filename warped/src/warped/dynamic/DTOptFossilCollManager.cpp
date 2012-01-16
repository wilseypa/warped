/// See copyright notice in file Copyright in the root directory of this archive.

#include "DTOptFossilCollManager.h"
#include "DTTimeWarpSimulationManager.h"
#include "SimulationObject.h"
#include "OutputManager.h"
#include "StateManager.h"
#include "RestoreCkptMessage.h"
#include "CommunicationManager.h"
#include "EventFunctors.h"
#include "TerminationManager.h"
#include "IntVTime.h"
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sstream>
#include <fstream>
using namespace std;

const char delimiter = '_';
const string prefixCkptPath = "/tmp/warpedOFCFiles/";

static pthread_key_t threadKey;
unsigned int threadID;

DTOptFossilCollManager::DTOptFossilCollManager(
		DTTimeWarpSimulationManager *sim, int checkPeriod, int minimumSamples,
		int maximumSamples, int defaultLen, double risk) :
			mySimManager(sim),
			myCommManager(sim->getCommunicationManager()),
			checkpointPeriod(checkPeriod),
			firstCheckpointTime(1),
			nextCheckpointTime(sim->getNumberOfSimulationObjects(),
					checkPeriod + 1),
			lastCheckpointTime(sim->getNumberOfSimulationObjects(), 0),
			fossilPeriod(sim->getNumberOfSimulationObjects(), 0),
			riskFactor(risk), defaultLength(defaultLen),
			minSamples(minimumSamples), maxSamples(maximumSamples),
			recovering(false), lastRestoreTime(-1) {
	int simId = sim->getSimulationManagerID();
	if (simId == sim->getNumberOfSimulationManagers() - 1) {
		myPeer = 0;
	} else {
		myPeer = simId + 1;
	}

	for (int i = 0; i < sim->getNumberOfSimulationObjects(); i++) {
		lastCollectTimes.push_back(-1);
		activeHistoryLength.push_back(defaultLength);
		numSamples.push_back(0);
		//availableStateMem.push_back( vector<State*>() );
	}

	// Warped must save the checkpoint files to a place unique to the user
	// running warped to avoid file conflict. One particular problem involves two
	// different users running warped at different times while left over files
	// remain from a previous run. In addition, the files should avoid being saved
	// on a network file system as this causes a significant hit in performance.

	// Form the checkpoint file path by using the username running the process.
	struct passwd *passwd;
	passwd = getpwuid(getuid());
	string uname = passwd->pw_name;
	ckptFilePath = prefixCkptPath + uname + "/";

	// Check if the directory to store the checkpoint files already exists.
	// If it does not exist, then create the directory.
	// This is for the prefixCkptPath only.
	struct stat st;
	if (stat(prefixCkptPath.c_str(), &st) != 0) {
		mkdir(prefixCkptPath.c_str(), 0777);
	}

	// Check if the directory to store the checkpoint files already exists.
	// If it does exist, then clear it out. Otherwise, create the directory.
	if (stat(ckptFilePath.c_str(), &st) == 0) {
		rmdir(ckptFilePath.c_str());
		mkdir(ckptFilePath.c_str(), 0777);
	} else {
		mkdir(ckptFilePath.c_str(), 0777);
	}

	// No longer used.
	//StateManagerImplementationBase *stateMan =
	//  dynamic_cast<StateManagerImplementationBase*>(sim->getStateManager());
	//stateQueue = stateMan->myStateQueue;
}

DTOptFossilCollManager::~DTOptFossilCollManager() {

	map<int, vector<State*>*>::iterator it = checkpointedStates.begin();
	while (it != checkpointedStates.end()) {
		vector<State*> *states = it->second;
		for (int i = 0; i < states->size(); i++) {
			delete (*states)[i];
		}
		delete states;
		it++;
	}

	for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
		for (int f = firstCheckpointTime; f < nextCheckpointTime[0]; f
				+= checkpointPeriod) {
			stringstream filename;
			filename << ckptFilePath << "LP"
					<< mySimManager->getSimulationManagerID() << "." << f
					<< "." << i;
		//	remove(filename.str().c_str());
		}
	}
	rmdir(ckptFilePath.c_str());

	// No longer used for memory management.
	/*  map<int, vector<Event*> *>::iterator m;
	 vector<Event*>::iterator i;

	 vector<NegativeEvent*>::iterator y;
	 for(y = inUseNegEventMem.begin(); y != inUseNegEventMem.end(); y++){
	 ::delete (*y);
	 }

	 char *c;
	 for(y = availableNegEventMem.begin(); y != availableNegEventMem.end(); y++){
	 c = (char*)(*y);
	 // The event destuctor has already been called so just release the memory.
	 delete []c;
	 }

	 vector<State*>::iterator it;
	 for(int n = 0; n < availableStateMem.size(); n++){
	 for(it = availableStateMem[n].begin(); it != availableStateMem[n].end(); it++){
	 ::delete (*it);
	 }
	 }*/
}

void DTOptFossilCollManager::checkpoint(const VTime &checkTime,
		const ObjectID &objId, const unsigned int &threadId) {
	int time = checkTime.getApproximateIntTime();
	int id = objId.getSimulationObjectID();

	// If the time is less than the last checkpoint time, then save at the last
	// checkpoint time again.
	updateCheckpointTime(id, time);

	while (time >= nextCheckpointTime[id]) {
		utils::debug << mySimManager->getSimulationManagerID()
				<< " - Checkpoint: " << nextCheckpointTime[id] << endl;

		vector<State *> *states;
		map<int, vector<State*>*>::iterator it = checkpointedStates.find(
				nextCheckpointTime[id]);
		if (it != checkpointedStates.end()) {
			// The checkpoint has been reached once but a rollback is causing it to be made
			// again. Only update the state for the object that rolled back.
			int id = objId.getSimulationObjectID();
			states = it->second;
			SimulationObject *object = mySimManager->getObjectHandle(objId);

			// Save the state of the object at the checkpoint time.
			State *newState = object->allocateState();
			newState->copyState(object->getState());
			(*states)[id] = newState;

			lastCheckpointTime[id] = nextCheckpointTime[id];
			nextCheckpointTime[id] += checkpointPeriod;

			stringstream filename;
			filename << ckptFilePath << "LP"
					<< mySimManager->getSimulationManagerID() << "."
					<< lastCheckpointTime[id] << "." << id;

			ofstream ckFile(filename.str().c_str(), ofstream::binary);
			if (!ckFile.is_open()) {
				cerr << mySimManager->getSimulationManagerID()
						<< " - Could not open file: " << filename.str()
						<< ", aborting simulation." << endl;
				abort();
			}

			mySimManager->saveFileQueuesCheckpoint(&ckFile, objId,
					lastCheckpointTime[id]);
			mySimManager->getOutputManagerNew()->saveOutputCheckpoint(&ckFile,
					objId, lastCheckpointTime[id], threadId);
			ckFile.close();
		} else {
			// This is the first time the checkpoint has been made. Save the state of all
			// of the objects.
			states = new vector<State*> (
					mySimManager->getNumberOfSimulationObjects(), NULL);
			checkpointedStates.insert(
					pair<int, vector<State*> *> (nextCheckpointTime[id], states));

			unsigned int simMgrID = mySimManager->getSimulationManagerID();
			for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
				SimulationObject *object = mySimManager->getObjectHandle(
						ObjectID(i, simMgrID));

				// Save the state of the object at the checkpoint time.
				State *newState = object->allocateState();
				newState->copyState(object->getState());
				(*states)[i] = newState;

				lastCheckpointTime[i] = nextCheckpointTime[i];
				nextCheckpointTime[i] += checkpointPeriod;

				stringstream fname;
				fname << ckptFilePath << "LP" << simMgrID << "."
						<< lastCheckpointTime[i] << "." << i;

				ofstream ckptFile(fname.str().c_str(), ofstream::binary);
				if (!ckptFile.is_open()) {
					cerr << simMgrID << " - Could not open file: "
							<< fname.str() << ", aborting simulation." << endl;
					abort();
				}

				// Save the file queues.
				mySimManager->saveFileQueuesCheckpoint(&ckptFile,
						ObjectID(i, simMgrID), lastCheckpointTime[i]);

				// Save the output events.
				mySimManager->getOutputManagerNew()->saveOutputCheckpoint(
						&ckptFile, ObjectID(i, simMgrID), lastCheckpointTime[i], threadId);

				ckptFile.close();
			}
		}
	}
}

void DTOptFossilCollManager::restoreCheckpoint(unsigned int restoredTime) {
	for (int i = 0; i < nextCheckpointTime.size(); i++) {
		lastCheckpointTime[i] = restoredTime;
		nextCheckpointTime[i] = restoredTime + checkpointPeriod;
	}
	utils::debug << mySimManager->getSimulationManagerID()
			<< " - Restoring to checkpoint: " << restoredTime << endl;

	// Reset the GVT.
	mySimManager->getGVTManager()->setGVT(mySimManager->getZero());

	// Restore the states to the objects. The actual state queue will be filled
	// after all of the events have been transmitted
	unsigned int size;
	char delIn;
	SimulationObject *object;
	unsigned int simMgrID = mySimManager->getSimulationManagerID();

	vector<State*> *states = checkpointedStates.find(restoredTime)->second;
	vector<Event*> restoredEvents;
	for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
		object = mySimManager->getObjectHandle(ObjectID(i, simMgrID));
		object->setSimulationTime(mySimManager->getZero());
		object->getState()->copyState((*states)[i]);

		stringstream filename;
		filename << ckptFilePath << "LP"
				<< mySimManager->getSimulationManagerID() << "."
				<< restoredTime << "." << i;
		ifstream ckFile;
		ckFile.open(filename.str().c_str());
		if (!ckFile.is_open()) {
			cerr << mySimManager->getSimulationManagerID()
					<< " - Could not open file: " << filename.str()
					<< ", aborting simulation." << endl;
			abort();
		}

		// Restore the file queues.
		unsigned int simMgrID = mySimManager->getSimulationManagerID();
		mySimManager->restoreFileQueues(&ckFile, ObjectID(i, simMgrID),
				restoredTime);

		// Read in the events.
		while (ckFile.peek() != EOF) {
			ckFile.read((char*) (&size), sizeof(size));
			ckFile.read(&delIn, sizeof(delIn));

			if (delIn != delimiter) {
				cerr << mySimManager->getSimulationManagerID()
						<< "ALIGNMENT ERROR in Event"
						<< "restoreCheckpoint. Got: " << delIn << endl;
				abort();
			}

			char *buf = new char[size];
			ckFile.read(buf, size);
			SerializedInstance *serEvent = new SerializedInstance(buf, size);
			Event *restoredEvent =
					dynamic_cast<Event*> (serEvent->deserialize());
			delete[] buf;
			delete serEvent;

			restoredEvents.push_back(restoredEvent);
		}

		ckFile.close();
	}

	std::sort(restoredEvents.begin(), restoredEvents.end(),
			receiveTimeLessThanEventIdLessThan());

	// Restore the output queue and transmit the events to restore the event set.
	for (int i = 0; i < restoredEvents.size(); i++) {
		mySimManager->handleEvent(restoredEvents[i]);
	}

	lastRestoreTime = restoredTime;

	utils::debug << mySimManager->getSimulationManagerID()
			<< " - Done with restore process, " << restoredTime << endl;
}

void DTOptFossilCollManager::makeInitialCheckpoint() {
	vector<State *> *states = new vector<State*> (
			mySimManager->getNumberOfSimulationObjects(), NULL);
	checkpointedStates.insert(
			pair<int, vector<State*> *> (firstCheckpointTime, states));

	unsigned int simMgrID = mySimManager->getSimulationManagerID();
	for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
		SimulationObject *object = mySimManager->getObjectHandle(
				ObjectID(i, simMgrID));

		// Save the state of the object at the checkpoint time.
		State *newState = object->allocateState();
		newState->copyState(object->getState());
		(*states)[i] = newState;
		lastCheckpointTime[i] = firstCheckpointTime;

		stringstream fname;
		fname << ckptFilePath << "LP" << simMgrID << "." << firstCheckpointTime
				<< "." << i;

		ofstream ckptFile(fname.str().c_str(), ofstream::binary);
		if (!ckptFile.is_open()) {
			cerr << simMgrID << " - Could not open file: " << fname.str()
					<< ", aborting simulation." << endl;
			abort();
		}

		mySimManager->saveFileQueuesCheckpoint(&ckptFile,
				ObjectID(i, simMgrID), lastCheckpointTime[i]);

		//cout << i << endl << lastCheckpointTime[0] << endl << ObjectID(i,simMgrID) << endl << &ckptFile << endl;
		//testOutputManager = mySimManager->getOutputManager();
		//testOutputManager->testfunction();

		mySimManager->getOutputManagerNew()->saveOutputCheckpoint(&ckptFile,
				ObjectID(i, simMgrID), lastCheckpointTime[i], 1); // ThreadId hard-coded as 1 as it it the manager

		ckptFile.close();
	}
}

void DTOptFossilCollManager::updateCheckpointTime(unsigned int objId,
		int checkTime) {
	if (checkTime < lastCheckpointTime[objId]) {
		nextCheckpointTime[objId] = lastCheckpointTime[objId];
		lastCheckpointTime[objId] -= checkpointPeriod;
		if (lastCheckpointTime[objId] < firstCheckpointTime) {
			lastCheckpointTime[objId] = firstCheckpointTime;
			nextCheckpointTime[objId] = checkpointPeriod + 1;
		}
	}
}

void DTOptFossilCollManager::startRecovery(unsigned int objId,
		unsigned int rollbackTime) {
	RestoreCkptMessage *restoreMsg = NULL;
	unsigned int dest = 0;
	int checkpt;

	// Increase the active history length of this object.
	activeHistoryLength[objId] = activeHistoryLength[objId] * 1.3;

	// Enter recovery mode.
	mySimManager->setRecoveringFromCheckpoint(true);
	myCommManager->setRecoveringFromCheckpoint(true);

	// Find the checkpoint to restore from.
	checkpt = lastCheckpointTime[0];
	for (int i = 1; i < lastCheckpointTime.size(); i++) {
		if (lastCheckpointTime[i] < checkpt) {
			checkpt = lastCheckpointTime[i];
		}
	}

	while (rollbackTime < checkpt) {
		checkpt -= checkpointPeriod;
	}

	if (checkpt < firstCheckpointTime) {
		checkpt = firstCheckpointTime;
	}

	// If we aren't the master, send a message to the master to initiate recovery.
	if (mySimManager->getSimulationManagerID() != 0) {
		dest = 0;
		restoreMsg = new RestoreCkptMessage(
				mySimManager->getSimulationManagerID(), dest, checkpt,
				RestoreCkptMessage::SEND_TO_MASTER, false);
	} else {
		dest = myPeer;
		recovering = true;
		myCommManager->incrementNumRecoveries();
		restoreMsg = new RestoreCkptMessage(
				mySimManager->getSimulationManagerID(), dest, checkpt,
				RestoreCkptMessage::FIRST_CYCLE, false);
	}

	myCommManager->sendMessage(restoreMsg, dest);
}

void DTOptFossilCollManager::receiveKernelMessage(KernelMessage *msg) {
	RestoreCkptMessage *restoreMsg = dynamic_cast<RestoreCkptMessage *> (msg);
	RestoreCkptMessage *sendMsg = NULL;
	int checkpt;

	if (restoreMsg != NULL) {
		if (mySimManager->getSimulationManagerID() == 0) {

			switch (restoreMsg->getTokenState()) {
			case RestoreCkptMessage::SEND_TO_MASTER:
				if (!recovering) {
					utils::debug << "Master: SEND_TO_MASTER received."
							<< endl;

					// Master receiving a message from another manager.
					// Start round one of the process.
					mySimManager->setRecoveringFromCheckpoint(true);
					myCommManager->setRecoveringFromCheckpoint(true);
					myCommManager->incrementNumRecoveries();
					recovering = true;

					sendMsg = new RestoreCkptMessage(
							mySimManager->getSimulationManagerID(), myPeer,
							restoreMsg->getCheckpointTime(),
							RestoreCkptMessage::FIRST_CYCLE, false);

					myCommManager->sendMessage(sendMsg, myPeer);
				}
				break;
			case RestoreCkptMessage::FIRST_CYCLE:
				if (recovering) {
					utils::debug << "Master: FIRST_CYCLE received." << endl;

					// Clean any received messages
					while (myCommManager->checkPhysicalLayerForMessages(1000)
							== 1000)
						;

					// Purge the input, output, and state queues.
					threadID = *((unsigned int*) pthread_getspecific(threadKey));
					mySimManager->getOutputManagerNew()->ofcPurge(threadID);
					mySimManager->getEventSetManagerNew()->ofcPurge(threadID);
					mySimManager->getStateManagerNew()->ofcPurge(threadID);
					mySimManager->getGVTManager()->setGVT(
							mySimManager->getZero());

					// Reset the last collect times.
					for (int i = 0; i
							< mySimManager->getNumberOfSimulationObjects(); i++) {
						lastCollectTimes[i] = -1;
					}

					// Determine if this manager has the current checkpoint to restore.
					checkpt = lastCheckpointTime[0];
					for (int i = 1; i < lastCheckpointTime.size(); i++) {
						if (lastCheckpointTime[i] < checkpt) {
							checkpt = lastCheckpointTime[i];
						}
					}
					if (checkpt > restoreMsg->getCheckpointTime()) {
						checkpt = restoreMsg->getCheckpointTime();
					}

					// Start the second round.
					sendMsg = new RestoreCkptMessage(
							mySimManager->getSimulationManagerID(), myPeer,
							checkpt, RestoreCkptMessage::SECOND_CYCLE, false);

					myCommManager->sendMessage(sendMsg, myPeer);
				}
				break;
			case RestoreCkptMessage::SECOND_CYCLE:
				if (recovering) {
					utils::debug << "Master: SECOND_CYCLE received." << endl;

					// Now send around the message informing the other managers of the
					// checkpoint to use. This is the third round.
					for (int dest = 1; dest
							< mySimManager->getNumberOfSimulationManagers(); dest++) {
						sendMsg = new RestoreCkptMessage(
								mySimManager->getSimulationManagerID(), dest,
								restoreMsg->getCheckpointTime(),
								RestoreCkptMessage::THIRD_CYCLE, true);

						myCommManager->sendMessage(sendMsg, dest);
					}

					mySimManager->setRecoveringFromCheckpoint(false);
					myCommManager->setRecoveringFromCheckpoint(false);
					mySimManager->getGVTManager()->ofcReset();
					mySimManager->getTerminationManager()->ofcReset();
					recovering = false;

					restoreCheckpoint(restoreMsg->getCheckpointTime());
				}
				break;
			default:
				break;
			}
		} else {
			switch (restoreMsg->getTokenState()) {
			case RestoreCkptMessage::SEND_TO_MASTER:
				cerr
						<< "ERROR: Start checkpoint recovery message send to wrong manager!"
						<< endl;
				break;
			case RestoreCkptMessage::FIRST_CYCLE:
				if (!recovering) {
					utils::debug << mySimManager->getSimulationManagerID()
							<< " - FIRST_CYCLE received." << endl;

					// Go into recovery mode.
					mySimManager->setRecoveringFromCheckpoint(true);
					myCommManager->setRecoveringFromCheckpoint(true);
					myCommManager->incrementNumRecoveries();
					recovering = true;

					// Clean any received messages
					while (myCommManager->checkPhysicalLayerForMessages(1000)
							== 1000)
						;

					// Purge the input, output, and state queues.
					mySimManager->getOutputManagerNew()->ofcPurge(threadID);
					mySimManager->getEventSetManagerNew()->ofcPurge(threadID);
					mySimManager->getStateManagerNew()->ofcPurge(threadID);
					mySimManager->getGVTManager()->setGVT(
							mySimManager->getZero());

					// Reset the last collect times.
					for (int i = 0; i
							< mySimManager->getNumberOfSimulationObjects(); i++) {
						lastCollectTimes[i] = -1;
					}

					// Pass the message along to the next manager.
					sendMsg = new RestoreCkptMessage(
							mySimManager->getSimulationManagerID(), myPeer,
							restoreMsg->getCheckpointTime(),
							restoreMsg->getTokenState(),
							restoreMsg->getCheckpointConsensus());

					myCommManager->sendMessage(sendMsg, myPeer);
				}
				break;
			case RestoreCkptMessage::SECOND_CYCLE:
				if (recovering) {
					utils::debug << mySimManager->getSimulationManagerID()
							<< " - SECOND_CYCLE received." << endl;

					// Clean any received messages
					while (myCommManager->checkPhysicalLayerForMessages(1000)
							== 1000)
						;

					// Determine if this manager has the current checkpoint to restore.
					checkpt = lastCheckpointTime[0];
					for (int i = 1; i < lastCheckpointTime.size(); i++) {
						if (lastCheckpointTime[i] < checkpt) {
							checkpt = lastCheckpointTime[i];
						}
					}
					if (checkpt > restoreMsg->getCheckpointTime()) {
						checkpt = restoreMsg->getCheckpointTime();
					}

					// Pass the message along to the next manager.
					sendMsg = new RestoreCkptMessage(
							mySimManager->getSimulationManagerID(), myPeer,
							checkpt, restoreMsg->getTokenState(),
							restoreMsg->getCheckpointConsensus());

					myCommManager->sendMessage(sendMsg, myPeer);
				}
				break;
			case RestoreCkptMessage::THIRD_CYCLE:
				if (recovering) {
					utils::debug << mySimManager->getSimulationManagerID()
							<< " - THIRD_CYCLE received." << endl;

					// Exit checkpoint recovery mode.
					mySimManager->setRecoveringFromCheckpoint(false);
					myCommManager->setRecoveringFromCheckpoint(false);
					mySimManager->getGVTManager()->ofcReset();
					mySimManager->getTerminationManager()->ofcReset();
					recovering = false;

					restoreCheckpoint(restoreMsg->getCheckpointTime());
				}
				break;
			default:
				break;
			}
		}
		delete restoreMsg;
	} else {
		cerr << "OptFossilCollManager::receiveKernelMessage() received"
				<< " unknown (" << msg->getDataType() << ") message type"
				<< endl;

		cerr << "Aborting simulation ..." << endl;
		abort();
	}
}

void DTOptFossilCollManager::fossilCollect(SimulationObject *object,
		const VTime &currentTime, const unsigned int &threadId) {
	unsigned int objId = object->getObjectID()->getSimulationObjectID();
	fossilPeriod[objId]++;
	if (fossilPeriod[objId] >= 10) {
		int intCurTime = currentTime.getApproximateIntTime();
		if (intCurTime > activeHistoryLength[objId]) {
			int collectTime = intCurTime - activeHistoryLength[objId];
			if (collectTime > lastCollectTimes[objId]) {
				lastCollectTimes[objId] = collectTime;
				mySimManager->getStateManagerNew()->fossilCollect(object,
						collectTime,threadId);
				mySimManager->getOutputManagerNew()->fossilCollect(object,
						collectTime, threadId);
				mySimManager->getEventSetManagerNew()->fossilCollect(object,
						collectTime,threadId);
				mySimManager->fossilCollectFileQueues(object, collectTime);
			}
		}
		fossilPeriod[objId] = 0;
	}
}

bool DTOptFossilCollManager::checkFault(SimulationObject *object) {
	bool isFault = true;
	unsigned int objId = object->getObjectID()->getSimulationObjectID();

	if (lastCollectTimes[objId] == -1) {
		// Then restore the proper state.
		vector<State*> *states =
				checkpointedStates.find(lastRestoreTime)->second;

		object->getState()->copyState((*states)[objId]);

		// Remove the older states from this object's queue.
		mySimManager->getStateManagerNew()->ofcPurge(objId);

		isFault = false;
	}

	return isFault;
}

void DTOptFossilCollManager::registerWithCommunicationManager() {
	ASSERT(myCommManager != NULL);
	myCommManager->registerMessageType(
			RestoreCkptMessage::getRestoreCkptMessageType(), this);
}
void DTOptFossilCollManager::configure(SimulationConfiguration &configuration) {
	registerWithCommunicationManager();
}
