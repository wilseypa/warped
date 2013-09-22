#ifndef LOCALNEGATIVEEVENTMESSAGE_H_
#define LOCALNEGATIVEEVENTMESSAGE_H_


#include "LocalKernelMessage.h"
#include "NegativeEventMessage.h"
#include "NegativeEvent.h"
#include "Event.h"
#include <vector>

class LocalNegativeEventMessage : public LocalKernelMessage
{
private:
	vector<const NegativeEvent *> *negativeEvents;
public:
	LocalNegativeEventMessage(const vector<const NegativeEvent *> &negEvents, local_kernel_message_type messageType, SimulationObject *object)
		: negativeEvents(new vector<const NegativeEvent *>(negEvents.begin(), negEvents.end())),
		  LocalKernelMessage(messageType, object, negEvents[0]->getReceiveTime().clone()) {}

	~LocalNegativeEventMessage()
	{
		delete negativeEvents;
	}

	const vector<const NegativeEvent*> &getNegativeEvents()
	{
		return *negativeEvents;
	}
};

#endif /* LOCALNEGATIVEEVENTMESSAGE_H_ */
