#ifndef LOCAL_EVENT_MESSAGE_H_
#define LOCAL_EVENT_MESSAGE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "LocalKernelMessage.h"
//#include "EventMessage.h"
#include "Event.h"

class LocalEventMessage : public LocalKernelMessage
{
private:
	const Event *myEvent;
public:
	LocalEventMessage(const Event *event, local_kernel_message_type messageType, SimulationObject *object)
		: myEvent(event), LocalKernelMessage(messageType, object, event->getReceiveTime().clone()) {}

	 ~LocalEventMessage() {}

	const Event* getEvent()
	{
		return myEvent;
	}

	/*EventMessage* getMessage()
	{
		return myMessage;
	}*/
};

#endif /* LOCAL_EVENT_MESSAGE_H_ */
