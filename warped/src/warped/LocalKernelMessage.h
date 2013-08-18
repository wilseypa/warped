#ifndef LOCAL_KERNEL_MESSAGE_H_
#define LOCAL_KERNEL_MESSAGE_H_


#include "SimulationObject.h"
#include "ObjectID.h"
using std::endl;
using std::cout;
enum local_kernel_message_type
{
	LKM_EVENT,
	LKM_NEGATIVE_EVENT,
	LKM_OUTPUT_CLEANUP,
	LKM_FOSSIL_COLLECT_STATE,
	LKM_FOSSIL_COLLECT_OUTPUT,
	LKM_FOSSIL_COLLECT_EVENTSET,
};

class LocalKernelMessage
{
private:
	local_kernel_message_type myMessageType;
	const ObjectID *myObjectID;
	const VTime *receiveTime;
public:
	LocalKernelMessage(local_kernel_message_type messageType, SimulationObject *object)
		: myMessageType(messageType), myObjectID(object->getObjectID()), receiveTime(NULL) {}
	LocalKernelMessage(local_kernel_message_type messageType, SimulationObject *object, const VTime *receiveTime)
		: myMessageType(messageType), myObjectID(object->getObjectID()), receiveTime(receiveTime) {}

	virtual ~LocalKernelMessage()
	{
		delete receiveTime;
	}

	const ObjectID &getObjectID() {
		return *myObjectID;
	}

	const VTime *getReceiveTime() {
		return receiveTime;
	}

	const local_kernel_message_type getMessageType() {
		return myMessageType;
	}
};

#endif /* RECEIVEDQUEUEMESSAGE_H_ */
