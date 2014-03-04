
#include <dirent.h>                     // for dirent, closedir, opendir, etc
#include <pthread.h>                    // for pthread_getspecific
#include <pwd.h>                        // for getpwuid, passwd
#include <stdio.h>                      // for NULL, remove, printf, EOF
#include <stdlib.h>                     // for abort
#include <sys/stat.h>                   // for mkdir, stat
#include <unistd.h>                     // for rmdir, getuid
#include <algorithm>                    // for sort
#include <fstream>                      // for operator<<, basic_ostream, etc
#include <iostream>                     // for cerr, cout
#include <sstream>
#include <utility>                      // for pair

#include "CommunicationManager.h"       // for CommunicationManager
#include "Event.h"                      // for Event
#include "EventFunctors.h"
#include "KernelMessage.h"              // for KernelMessage
#include "LockState.h"                  // for LockState
#include "RestoreCkptMessage.h"         // for RestoreCkptMessage, etc
#include "Serializable.h"               // for Serializable
#include "SerializedInstance.h"         // for SerializedInstance
#include "SimulationObject.h"           // for SimulationObject
#include "State.h"                      // for State
#include "TerminationManager.h"         // for TerminationManager
#include "ThreadedOptFossilCollManager.h"
#include "ThreadedOutputManager.h"      // for ThreadedOutputManager
#include "ThreadedStateManager.h"       // for ThreadedStateManager
#include "ThreadedTimeWarpEventSet.h"   // for ThreadedTimeWarpEventSet
#include "ThreadedTimeWarpSimulationManager.h"
#include "VTime.h"                      // for VTime
#include "WarpedDebug.h"                // for debugout
#include "WorkerInformation.h"          // for WorkerInformation
#include "warped.h"                     // for ASSERT

class SimulationConfiguration;

using namespace std;

const char delimiter = '_';
const string prefixCkptPath = "/tmp/warpedOFCFiles/";

unsigned int threadID;

ThreadedOptFossilCollManager::ThreadedOptFossilCollManager(
    ThreadedTimeWarpSimulationManager* sim, int checkPeriod, int minimumSamples,
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
    recovering(false), lastRestoreTime(-1),startCata(false) {
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
    struct passwd* passwd;
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
        deleteWarpedOFCFiles(ckptFilePath);
        rmdir(ckptFilePath.c_str());
        mkdir(ckptFilePath.c_str(), 0777);
    } else {
        mkdir(ckptFilePath.c_str(), 0777);
    }

    ofcFlagLock = new LockState();
    ofcCheckPoint = new LockState();
}

ThreadedOptFossilCollManager::~ThreadedOptFossilCollManager() {

    map<int, vector<State*>*>::iterator it = checkpointedStates.begin();
    while (it != checkpointedStates.end()) {
        vector<State*>* states = it->second;
        for (int i = 0; i < states->size(); i++) {
            delete(*states)[i];
        }
        delete states;
        it++;
    }

    for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
        for (int f = firstCheckpointTime; f < nextCheckpointTime[i]; f
                += checkpointPeriod) {
            stringstream filename;
            filename << ckptFilePath << "LP"
                     << mySimManager->getSimulationManagerID() << "." << f
                     << "." << i;
            remove(filename.str().c_str());
        }
    }
    deleteWarpedOFCFiles(ckptFilePath);
    rmdir(ckptFilePath.c_str());
    delete ofcFlagLock;
    delete ofcCheckPoint;
}

void ThreadedOptFossilCollManager::checkpoint(const VTime& checkTime,
                                              const ObjectID& objId, const unsigned int& threadId) {
    int time = checkTime.getApproximateIntTime();
    int id = objId.getSimulationObjectID();

    // If the time is less than the last checkpoint time, then save at the last
    // checkpoint time again.

    updateCheckpointTime(id, time);

    while (time >= nextCheckpointTime[id]) {
        getOfcCheckPointLock(threadId,mySimManager->getSyncMechanism()); 
        debug::debugout << mySimManager->getSimulationManagerID()
                        << " - Checkpointing object " << id << " at " << time << endl;
        int highestNextCheckpointTime = nextCheckpointTime[0];
        for (int iter = 1; iter < mySimManager->getNumberOfSimulationObjects(); iter++) {
            if (nextCheckpointTime[iter] > highestNextCheckpointTime)
            { highestNextCheckpointTime = nextCheckpointTime[iter]; }
        }

        vector<State*>* states;
        if (nextCheckpointTime[id] == highestNextCheckpointTime) {
            // No states have been check pointed for this time yet.
            // Thus we create a state vector for all the objects
            debug::debugout << "Creating new states to be saved at time "
                            << nextCheckpointTime[id] << endl;
            states = new vector<State*> (
                mySimManager->getNumberOfSimulationObjects(), NULL);
            checkpointedStates.insert(
                pair<int, vector<State*> *> (nextCheckpointTime[id], states));
        } else {
            // States have been saved for other objects but not this object at this time
            // or a roll back in this object is causing this to happen.
            debug::debugout
                    << "Adding the current state of the object to checkpointedStates "
                    << nextCheckpointTime[id] << endl;

            map<int, vector<State*>*>::iterator it = checkpointedStates.find(
                                                         nextCheckpointTime[id]);
            states = it->second;
        }

        SimulationObject* object = mySimManager->getObjectHandle(objId);

        // Save the state of the object at the checkpoint time.
        State* newState = object->allocateState();
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
        

        mySimManager->getOutputManagerNew()->saveOutputCheckpoint(&ckFile,
                                                                  objId, lastCheckpointTime[id], threadId);
        ckFile.close();
        releaseOfcCheckPointLock(threadId,mySimManager->getSyncMechanism()); 
    }

}

void ThreadedOptFossilCollManager::restoreCheckpoint(unsigned int restoredTime) {
    for (int i = 0; i < nextCheckpointTime.size(); i++) {
        lastCheckpointTime[i] = restoredTime;
        nextCheckpointTime[i] = restoredTime + checkpointPeriod;
    }
    debug::debugout << mySimManager->getSimulationManagerID()
                    << " - Restoring to checkpoint: " << restoredTime << endl;

    // Restore the states to the objects. The actual state queue will be filled
    // after all of the events have been transmitted
    unsigned int size;
    char delIn;
    SimulationObject* object;
    unsigned int simMgrID = mySimManager->getSimulationManagerID();

    vector<State*>* states = checkpointedStates.find(restoredTime)->second;
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

        // Read in the events.
        while (ckFile.peek() != EOF) {
            ckFile.read((char*)(&size), sizeof(size));
            ckFile.read(&delIn, sizeof(delIn));

            if (delIn != delimiter) {
                cerr << mySimManager->getSimulationManagerID()
                     << "ALIGNMENT ERROR in Event"
                     << "restoreCheckpoint. Got: " << delIn << endl;
                abort();
            }

            char* buf = new char[size];
            ckFile.read(buf, size);
            SerializedInstance* serEvent = new SerializedInstance(buf, size);
            Event* restoredEvent =
                dynamic_cast<Event*>(serEvent->deserialize());
            delete[] buf;
            delete serEvent;

            restoredEvents.push_back(restoredEvent);
            debug::debugout << "restoring to objID "
                            << restoredEvent->getSender().getSimulationObjectID()
                            << endl;
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

    // Set recovering to false
    mySimManager->setRecoveringFromCheckpoint(false);
    //myCommManager->setRecoveringFromCheckpoint(false);

   debug::debugout  << mySimManager->getSimulationManagerID()
                    << " - Done with restore process, " << restoredTime << endl;
}

void ThreadedOptFossilCollManager::makeInitialCheckpoint() {
    vector<State*>* states = new vector<State*> (
        mySimManager->getNumberOfSimulationObjects(), NULL);
    checkpointedStates.insert(
        pair<int, vector<State*> *> (firstCheckpointTime, states));

    unsigned int simMgrID = mySimManager->getSimulationManagerID();
    for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
        SimulationObject* object = mySimManager->getObjectHandle(
                                       ObjectID(i, simMgrID));

        // Save the state of the object at the checkpoint time.
        State* newState = object->allocateState();
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
        mySimManager->getOutputManagerNew()->saveOutputCheckpoint(&ckptFile,
                                                                  ObjectID(i, simMgrID), lastCheckpointTime[i], 1); // ThreadId hard-coded as 1 as it is the manager

        ckptFile.close();
    }
}

void ThreadedOptFossilCollManager::updateCheckpointTime(unsigned int objId,
                                                        int checkTime) {
    int threadId = *((int*) pthread_getspecific(threadKey));
    getOfcCheckPointLock(threadId,mySimManager->getSyncMechanism()); 
    while (checkTime < lastCheckpointTime[objId]) {
        nextCheckpointTime[objId] = lastCheckpointTime[objId];
        lastCheckpointTime[objId] -= checkpointPeriod;
        if (lastCheckpointTime[objId] < firstCheckpointTime) {
            lastCheckpointTime[objId] = firstCheckpointTime;
            nextCheckpointTime[objId] = checkpointPeriod + 1;
        }
    }
   releaseOfcCheckPointLock(threadId,mySimManager->getSyncMechanism()); 
}

void ThreadedOptFossilCollManager::purgeQueuesAndRecover() {

    // Wait for all the worker threads to stop execution
    debug::debugout << "Waiting for all the worker threads" << endl;
    while (mySimManager->workerStatus[0]->getStillBusyCount() > 0)
        debug::debugout << mySimManager->workerStatus[0]->getStillBusyCount()
                        << endl;
    debug::debugout << "Stopped all the threads." << endl;

    // Release all the Object and State Queue locks
    mySimManager->releaseObjectLocksRecovery();
    mySimManager->getStateManagerNew()->releaseStateLocksRecovery();

    // Find the checkpoint to restore from.
    int checkpt = lastCheckpointTime[0];
    for (int i = 1; i < lastCheckpointTime.size(); i++) {
        if (lastCheckpointTime[i] < checkpt) {
            checkpt = lastCheckpointTime[i];
        }
    }

    while (restoreRollbackTime < checkpt) {
        checkpt -= checkpointPeriod;
    }

    if (checkpt < firstCheckpointTime) {
        checkpt = firstCheckpointTime;
    }
    debug::debugout << "The check point to be restored is " << checkpt << "."
                    << endl;

    // Purge all the Queues
    threadID = *((unsigned int*) pthread_getspecific(threadKey));
    mySimManager->getOutputManagerNew()->ofcPurge(threadID);
    mySimManager->getEventSetManagerNew()->ofcPurge(threadID);
    mySimManager->getStateManagerNew()->ofcPurge(threadID);
    debug::debugout << "Purged all queues." << endl;

    // Reset the last collect times.
    for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
        lastCollectTimes[i] = -1;
    }
    debug::debugout << "Reseted the last collect times." << endl;

    //mySimManager->getGVTManager()->ofcReset();
    mySimManager->getTerminationManager()->ofcReset();
    debug::debugout << "Set recovery to false and about to continue execution"
                    << endl;

    // Restore all the queues
    restoreCheckpoint(checkpt);
    debug::debugout << "Restored to the last checkpoint!!" << endl;
}

void ThreadedOptFossilCollManager::setRecovery(unsigned int objId,
                                               unsigned int rollbackTime) {
    // Enter recovery mode.
    if (mySimManager->getNumberOfSimulationManagers() == 1) {
        if (mySimManager->getRecoveringFromCheckpoint() == false) {
            myCommManager->setRecoveringFromCheckpoint(true);
            mySimManager->setRecoveringFromCheckpoint(true);

            // Increase the active history length of this object.
            activeHistoryLength[objId] = activeHistoryLength[objId] * 1.3;
            restoreRollbackTime = rollbackTime;
            mySimManager->setInitiatedRecovery(true);
        }
    } else {
        if (mySimManager->getRecoveringFromCheckpoint() == false
                && myCommManager->getRecoveringFromCheckpoint() == false) {
            myCommManager->setRecoveringFromCheckpoint(true);
            mySimManager->setRecoveringFromCheckpoint(true);
            startCata = true;     
            myCommManager->incrementNumRecoveries();
            // Increase the active history length of this object.
            activeHistoryLength[objId] = activeHistoryLength[objId] * 1.3;
            restoreRollbackTime = rollbackTime;
            mySimManager->setInitiatedRecovery(true);
        } else {
            myCommManager->setRecoveringFromCheckpoint(true);
            mySimManager->setRecoveringFromCheckpoint(true);
            startCata == true;
            myCommManager->incrementNumRecoveries();
        }
    }
}

void ThreadedOptFossilCollManager::startRecovery() {
    RestoreCkptMessage* restoreMsg = NULL;
    unsigned int dest = 0;
    int checkpt;

    debug::debugout << mySimManager->getSimulationManagerID()
                    << " - Started Recovery to rollback time " << restoreRollbackTime
                    << "; checkpoint at " << lastCheckpointTime[0] << endl;

    // Find the checkpoint to restore from.
    checkpt = lastCheckpointTime[0];
    for (int i = 1; i < lastCheckpointTime.size(); i++) {
        if (lastCheckpointTime[i] < checkpt) {
            checkpt = lastCheckpointTime[i];
        }
    }

    while (restoreRollbackTime < checkpt) {
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
        debug::debugout << mySimManager->getSimulationManagerID()
                        << " - Sent message to master" << endl;
    } else {
        dest = myPeer;
        recovering = true;
        restoreMsg = new RestoreCkptMessage(
            mySimManager->getSimulationManagerID(), dest, checkpt,
            RestoreCkptMessage::FIRST_CYCLE, false);
        debug::debugout << mySimManager->getSimulationManagerID()
                        << " - Sent the FIRST_CYCLE message " << endl;
    }
    myCommManager->sendMessage(restoreMsg, dest);
}

void ThreadedOptFossilCollManager::receiveKernelMessage(KernelMessage* msg) {
    RestoreCkptMessage* restoreMsg = dynamic_cast<RestoreCkptMessage*>(msg);
    RestoreCkptMessage* sendMsg = NULL;
    int checkpt;
    if (restoreMsg != NULL) {
         if (mySimManager->getSimulationManagerID() == 0) {

            switch (restoreMsg->getTokenState()) {
            case RestoreCkptMessage::SEND_TO_MASTER:
                if (!recovering) {
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - SEND_TO_MASTER received." << endl;
                    // Master receiving a message from another manager.
                    // Start round one of the process.
                  if (true !=startCata){
                    threadID
                        = *((unsigned int*) pthread_getspecific(threadKey));
		            this->getOfcFlagLock(threadID,mySimManager->getSyncMechanism());
                    mySimManager->setRecoveringFromCheckpoint(true);
                    myCommManager->setRecoveringFromCheckpoint(true);
		            this->releaseOfcFlagLock(threadID,mySimManager->getSyncMechanism());
                    myCommManager->incrementNumRecoveries();
                    recovering = true;

                    // Wait for all the worker threads to stop execution
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << "Waiting for all the worker threads" << endl;
                    while (mySimManager->workerStatus[0]->getStillBusyCount()
                            > 0)
                        debug::debugout
                                << mySimManager->workerStatus[0]->getStillBusyCount()
                                << endl;

                    sendMsg = new RestoreCkptMessage(
                        mySimManager->getSimulationManagerID(), myPeer,
                        restoreMsg->getCheckpointTime(),
                        RestoreCkptMessage::FIRST_CYCLE, false);

                    myCommManager->sendMessage(sendMsg, myPeer);
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - Sent the FIRST_CYCLE message " << endl;
                    }
                }
                break;
            case RestoreCkptMessage::FIRST_CYCLE:
                if (recovering) {
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - FIRST_CYCLE received." << endl;
                    // Release all the Object and State Queue locks
                    mySimManager->releaseObjectLocksRecovery();
                    mySimManager->getStateManagerNew()->releaseStateLocksRecovery();

                    // Clear the message buffer
                    mySimManager->clearMessageBuffer();

                    // Clean any received messages
                    while (myCommManager->checkPhysicalLayerForMessages(1000)
                            == 1000)
                        ;

                    // Purge the input, output, and state queues.
                    threadID
                        = *((unsigned int*) pthread_getspecific(threadKey));
                    mySimManager->getOutputManagerNew()->ofcPurge(threadID);
                    mySimManager->getEventSetManagerNew()->ofcPurge(threadID);
                    mySimManager->getStateManagerNew()->ofcPurge(threadID);
                    /*mySimManager->getGVTManager()->setGVT(
                     mySimManager->getZero());*/

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

                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - Sent the SECOND_CYCLE message " << endl;

                    myCommManager->sendMessage(sendMsg, myPeer);
                }
                break;
            case RestoreCkptMessage::SECOND_CYCLE:
                if (recovering) {
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - SECOND_CYCLE received." << endl;
                    // Now send around the message informing the other managers of the
                    // checkpoint to use. This is the third round.
                    for (int dest = 1; dest
                            < mySimManager->getNumberOfSimulationManagers(); dest++) {
                        sendMsg = new RestoreCkptMessage(
                            mySimManager->getSimulationManagerID(), dest,
                            restoreMsg->getCheckpointTime(),
                            RestoreCkptMessage::THIRD_CYCLE, true);

                        debug::debugout << mySimManager->getSimulationManagerID()
                                        << " - Sent the THIRD_CYCLE message " << endl;
                        myCommManager->sendMessage(sendMsg, dest);
                    }


                    /*mySimManager->getGVTManager()->ofcReset();*/
                    mySimManager->getTerminationManager()->ofcReset();

                    restoreCheckpoint(restoreMsg->getCheckpointTime());
                    recovering = false;
                    startCata = false;
                   // mySimManager->setRecoveringFromCheckpoint(false);
                    myCommManager->setRecoveringFromCheckpoint(false);
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
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - FIRST_CYCLE received." << endl;
                    // Go into recovery mode.
	            
                    threadID
                        = *((unsigned int*) pthread_getspecific(threadKey));
		            this->getOfcFlagLock(threadID,mySimManager->getSyncMechanism());
                    mySimManager->setRecoveringFromCheckpoint(true);
                    myCommManager->setRecoveringFromCheckpoint(true);
		            this->releaseOfcFlagLock(threadID,mySimManager->getSyncMechanism());
                    if(startCata != true){
                      myCommManager->incrementNumRecoveries();
                    }
                    recovering = true;

                    // Wait for all the worker threads to stop execution
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << "Waiting for all the worker threads" << endl;
                    while (mySimManager->workerStatus[0]->getStillBusyCount()
                            > 0)
                        debug::debugout
                                << mySimManager->workerStatus[0]->getStillBusyCount()
                                << endl;

                    // Release all the Object and State Queue locks
                    mySimManager->releaseObjectLocksRecovery();
                    mySimManager->getStateManagerNew()->releaseStateLocksRecovery();

                    // Clear the message buffer
                    mySimManager->clearMessageBuffer();

                    // Clean any received messages
                    while (myCommManager->checkPhysicalLayerForMessages(1000)
                            == 1000)
                        ;

                    // Purge the input, output, and state queues.
                    threadID
                        = *((unsigned int*) pthread_getspecific(threadKey));
                    mySimManager->getOutputManagerNew()->ofcPurge(threadID);
                    mySimManager->getEventSetManagerNew()->ofcPurge(threadID);
                    mySimManager->getStateManagerNew()->ofcPurge(threadID);
                    /*mySimManager->getGVTManager()->setGVT(
                     mySimManager->getZero());*/

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
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - Sent the FIRST_CYCLE" << endl;
                }
                break;
            case RestoreCkptMessage::SECOND_CYCLE:
                if (recovering) {
                    debug::debugout << mySimManager->getSimulationManagerID()
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

                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - Sent the SECOND_CYCLE message " << endl;
                    myCommManager->sendMessage(sendMsg, myPeer);
                }
                break;
            case RestoreCkptMessage::THIRD_CYCLE:
                if (recovering) {
                    debug::debugout << mySimManager->getSimulationManagerID()
                                    << " - THIRD_CYCLE received." << endl;
                    // Exit checkpoint recovery mode.


                    /*mySimManager->getGVTManager()->ofcReset();*/
                    mySimManager->getTerminationManager()->ofcReset();
                    restoreCheckpoint(restoreMsg->getCheckpointTime());
                    recovering = false;
                    startCata = false;
                    //mySimManager->setRecoveringFromCheckpoint(false);
                    myCommManager->setRecoveringFromCheckpoint(false);
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
        cout << "ERROR!!" << endl;
        cerr << "Aborting simulation ..." << endl;
        abort();
    }
}

void ThreadedOptFossilCollManager::fossilCollect(SimulationObject* object,
                                                 const VTime& currentTime, const unsigned int& threadId) {
    unsigned int objId = object->getObjectID()->getSimulationObjectID();
    fossilPeriod[objId]++;
    if (fossilPeriod[objId] >= 200) {
        int intCurTime = currentTime.getApproximateIntTime();
        if (intCurTime > activeHistoryLength[objId]) {
            int collectTime = intCurTime - activeHistoryLength[objId];
            if (collectTime > lastCollectTimes[objId]) {
                lastCollectTimes[objId] = collectTime;
                cout << "Fossil Collecting " << collectTime << endl;
                mySimManager->getStateManagerNew()->fossilCollect(object,
                                                                  collectTime, threadId);
                mySimManager->getOutputManagerNew()->fossilCollect(object,
                                                                   collectTime, threadId);
                mySimManager->getEventSetManagerNew()->fossilCollect(object,
                                                                     collectTime, threadId);
                 debug::debugout << "Fossil Collecting Obj " << objId
                 << " at time " << collectTime << " now at " << intCurTime << endl;
                 
            }
        }
        fossilPeriod[objId] = 0;
    }
}

bool ThreadedOptFossilCollManager::checkFault(SimulationObject* object) {
    bool isFault = true;
    unsigned int objId = object->getObjectID()->getSimulationObjectID();

    if (lastCollectTimes[objId] == -1) {
        // Then restore the proper state.
        vector<State*>* states =
            checkpointedStates.find(lastRestoreTime)->second;

        object->getState()->copyState((*states)[objId]);

        // Remove the older states from this object's queue.
        unsigned int threadid = *((unsigned int*) pthread_getspecific(threadKey));
        mySimManager->getStateManagerNew()->ofcPurge(objId, threadid);

        isFault = false;
    }

    return isFault;
}

void ThreadedOptFossilCollManager::registerWithCommunicationManager() {
    ASSERT(myCommManager != NULL);
    myCommManager->registerMessageType(
        RestoreCkptMessage::getRestoreCkptMessageType(), this);
}
void ThreadedOptFossilCollManager::configure(SimulationConfiguration& configuration) {
    registerWithCommunicationManager();
}

int ThreadedOptFossilCollManager::getLeastCollectTime() {
    int leastCollectTime = lastCollectTimes[0];
    for (int i = 0; i < mySimManager->getNumberOfSimulationObjects(); i++) {
        if (lastCollectTimes[i] < leastCollectTime)
        { leastCollectTime = lastCollectTimes[i]; }
    }
    return leastCollectTime;
}

void ThreadedOptFossilCollManager::getOfcFlagLock(int threadId, const string syncMech ) {
    ofcFlagLock->setLock(threadId,syncMech);
    ASSERT(ofcFlagLock->hasLock(threadId,syncMech));	    
}

void ThreadedOptFossilCollManager::releaseOfcFlagLock(int threadId, const string syncMech){
    ASSERT(ofcFlagLock->hasLock(threadId,syncMech));
    ofcFlagLock->releaseLock(threadId,syncMech);
}

void ThreadedOptFossilCollManager::getOfcCheckPointLock(int threadId, const string syncMech ) {
    ofcFlagLock->setLock(threadId,syncMech);
    ASSERT(ofcFlagLock->hasLock(threadId,syncMech));	    
}

void ThreadedOptFossilCollManager::releaseOfcCheckPointLock(int threadId, const string syncMech){
    ASSERT(ofcFlagLock->hasLock(threadId,syncMech));
    ofcFlagLock->releaseLock(threadId,syncMech);
}

void ThreadedOptFossilCollManager:: deleteWarpedOFCFiles(string dirPath){
  DIR* ofcDir = NULL; 
  struct dirent* ent = NULL;
  ofcDir = opendir(dirPath.c_str());
  if(NULL == ofcDir){
    cerr<<"Failed to locate the directory in which OFC checkpoint files are !"<<endl;
  } 
  while(NULL != (ent=readdir(ofcDir))){
    if(ent->d_type == DT_REG ){ // regular file DT_REG = 8
     char* cptFileName = ent->d_name;
     string fileName = dirPath + cptFileName;
     if (0!=remove(fileName.c_str()))
      /*The application doesn't go through this branch when it runs on the Beowulf cluster with multiple nodes*/
      printf("File %s has been deleted : Redundant Deletion.\n", ent->d_name ); 
    }
  }
  closedir(ofcDir);
  ofcDir = NULL; 
}
