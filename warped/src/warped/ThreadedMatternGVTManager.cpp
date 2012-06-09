// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedMatternGVTManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "ThreadedLazyOutputManager.h"
#include "MatternGVTMessage.h"
#include "GVTUpdateMessage.h"
#include "ThreadedTimeWarpMultiSetSchedulingManager.h"

ThreadedMatternGVTManager::ThreadedMatternGVTManager(ThreadedTimeWarpSimulationManager *simMgr,
		unsigned int period) :
		mySimulationManager(simMgr), MatternGVTManager(simMgr, period) {
}
ThreadedMatternGVTManager::ThreadedMatternGVTManager(ThreadedTimeWarpSimulationManager *simMgr,
		unsigned int period, bool objectRecordDefined) :
		mySimulationManager(simMgr), MatternGVTManager(simMgr, period,
				objectRecordDefined) {
}
/// Destructor
ThreadedMatternGVTManager::~ThreadedMatternGVTManager() {
}

/*const VTime *ThreadedMatternGVTManager::getEarliestEventTime(
		const VTime *lowEventTime) {
	if (mySimulationManager->getOutputMgrType() == LAZYMGR
			|| mySimulationManager->getOutputMgrType() == ADAPTIVEMGR) {
		ThreadedLazyOutputManager *lmgr =
				dynamic_cast<ThreadedLazyOutputManager*>(mySimulationManager->getOutputManagerNew());
		// GVT calculation is done by manager thread, hence passing 0 as threadId
		const VTime *lazyMinTime = &lmgr->getLazyQMinTime(0);
		lowEventTime = &MIN_FUNC(*lazyMinTime, *lowEventTime);
	}
	return lowEventTime;
}*/

void ThreadedMatternGVTManager::receiveKernelMessage(KernelMessage *msg) {
	ASSERT(msg != NULL);
	if (dynamic_cast<MatternGVTMessage *>(msg) != 0) {
		const MatternGVTMessage *gVTMessage =
				dynamic_cast<MatternGVTMessage *>(msg);
		const int count = gVTMessage->getNumMessagesInTransit();

		if (mySimulationManager->getSimulationManagerID() == 0) {
			// Initiator has received the control message.
			// Check to see if the count is zero and this is at least the second
			// round of the token. Continue until the count is 0 and all messages
			// in transit are accounted for.
			if (objectRecord->getTokenIterationNumber() > 1
					&& (objectRecord->getNumberOfWhiteMessages() + count == 0)) {

				// Need to remember the old gvt to compare it to the new.
				const VTime &oldGVT = getGVT();

				// Determine GVT.
				setGVT(
						MIN_FUNC(gVTMessage->getLastScheduledEventTime(),
								gVTMessage->getMinimumTimeStamp()));
				ASSERT(getGVT() >= oldGVT);

				// Send out the GVT update to the other simulation managers and reset to white.
				objectRecord->setTokenIterationNumber(0);
				objectRecord->setNumberOfWhiteMessages(0);
				objectRecord->setColor(WHITE);
				sendGVTUpdate();

				// End of this gvt calculation cycle.
				gVTTokenPending = false;

				// Only output the value and fossil collect when it actually changes.
				if (getGVT() > oldGVT) {
					// Fossil collect now with the new GVT.
					cout << "GVT = " << getGVT() << endl;
                    //mySimulationManager->resetGVTTokenPending();
					mySimulationManager->fossilCollect(getGVT());
				}
			} else {
				// Not yet ready to calculate gvt, send the token around again.
               // cout <<"NUmber of Transit messages "<<count <<endl;
               // cout <<"NUmber of White messages "<<objectRecord->getNumberOfWhiteMessages()<<endl;
                objectRecord->setNumberOfWhiteMessages(
                        objectRecord->getNumberOfWhiteMessages() + count);
				mySimulationManager->setGVTTokenPending();
				mySimulationManager->initiateLocalGVT();
				GVTMessageLastScheduledEventTime =
						gVTMessage->getLastScheduledEventTime().clone();
				GVTMessageMinimumTimeStamp =
						gVTMessage->getMinimumTimeStamp().clone();
                objectRecord->setNumberOfWhiteMessages(0);

			}
		} else {
			// The gvt token has been received by another simulation manager.
			// [a] Set color of this sim mgr to RED; set tMin = positive infinity.
			// [b] Pass on the token to processor(i mod n) + 1.
			if (objectRecord->getColor() == WHITE) {
				objectRecord->resetMinTimeStamp(
						mySimulationManager->getPositiveInfinity());
				objectRecord->setColor(RED);
			}

			// Add the the local white message count to the simulation's white message total.
			objectRecord->setNumberOfWhiteMessages(
					objectRecord->getNumberOfWhiteMessages() + count);
			mySimulationManager->setGVTTokenPending();
			mySimulationManager->initiateLocalGVT();
			GVTMessageLastScheduledEventTime =
					gVTMessage->getLastScheduledEventTime().clone();
			GVTMessageMinimumTimeStamp =
					gVTMessage->getMinimumTimeStamp().clone();
		}
	} else if (dynamic_cast<GVTUpdateMessage *>(msg) != 0) {
		const GVTUpdateMessage *gVTMessage =
				dynamic_cast<GVTUpdateMessage *>(msg);

		const VTime &oldGVT = getGVT();
		setGVT(gVTMessage->getNewGVT());
        //mySimulationManager->resetGVTTokenPending();
		ASSERT(getGVT() >= oldGVT);

		// Only fossil collect if the value has increased.
		if (getGVT() > oldGVT) {
			// Fossil collect now with the new GVT.
			mySimulationManager->fossilCollect(getGVT());
		}

		// VERY IMPORTANT NOTE!!
		// The white message count is not reset here because by the time this
		// simulation manager gets the update token, it may have already received some
		// white messages from another simulation manager that already switched back
		// to white. These need to be taken into account for the next GVT calculation.
		objectRecord->setTokenIterationNumber(0);
		objectRecord->setColor(WHITE);
	} else {
		cerr << "MatternGVTManager::receiveKernelMessage() received"
				<< " unknown (" << msg->getDataType() << ") message type"
				<< endl;
		cerr << "Aborting simulation ..." << endl;
		abort();
	}
	// We are done with this kernel message.
	delete msg;
}
void ThreadedMatternGVTManager::sendPendingGVTToken() {
	const VTime *lowEventTime =&(myScheduler->getLastEventScheduledTime());
	//cout << "Sent Pending Token" << endl;
	sendGVTToken(
			MIN_FUNC(*GVTMessageLastScheduledEventTime, *lowEventTime),
			MIN_FUNC(*GVTMessageMinimumTimeStamp,
					*objectRecord->getMinTimeStamp()));
	delete GVTMessageLastScheduledEventTime;
	delete GVTMessageMinimumTimeStamp;
	// This is reset to record the number of messages received since the last round.
	objectRecord->setNumberOfWhiteMessages(0);

}
