#ifndef LOCAL_FOSSIL_COLLECT_MESSAGE_H_
#define LOCAL_FOSSIL_COLLECT_MESSAGE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "LocalKernelMessage.h"
#include "VTime.h"
#include "SimulationObject.h"

class LocalFossilCollectMessage : public LocalKernelMessage
{
private:
	const VTime *minTime;
	const VTime *gvtTime;
public:
	LocalFossilCollectMessage(const VTime *minTime,
								const VTime *gvtTime,
								local_kernel_message_type messageType,
								SimulationObject *object)
		: LocalKernelMessage(messageType, object),
		  minTime(minTime),
		  gvtTime(gvtTime) {}

	 ~LocalFossilCollectMessage()
	 {
		 delete gvtTime;
		 delete minTime;
	 }

	const VTime* getMinTime()
	{
		return minTime;
	}

	const VTime* getGVTTime()
	{
		return gvtTime;
	}
};

#endif /* LOCAL_FOSSIL_COLLECT_MESSAGE_H_ */
