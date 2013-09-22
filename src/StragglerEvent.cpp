
#include "StragglerEvent.h"
#include "Serializable.h"
#include "SerializedInstance.h"
#include "ObjectID.h"
#include "EventId.h"
#include "DeserializerManager.h"

/*void StragglerEvent::registerDeserializer() {
	DeserializerManager::instance()->registerDeserializer(
			getStragglerEventDataType(), &StragglerEvent::deserialize);
}*/

std::ostream&
operator<<(std::ostream& os, const StragglerEvent& event) {
	os << "sender: " << event.getSender() << " receiver: "
			<< event.getReceiver() << " sendTime: " << event.getSendTime()
			<< " receiveTime: " << event.getReceiveTime() << " eventId: "
			<< event.getEventId();
	return (os);
}

bool StragglerEvent::eventCompare(const Event *a) {
	bool retval = true;

	if (a->getSender() != this->getSender() || a->getEventId()
			!= this->getEventId()) {
		retval = false;
	}

	return retval;
}

const Event *StragglerEvent::getPositiveEvent() const
{
    return positiveEvent;
}

bool StragglerEvent::getStragglerType() const
{
    return stragglerType;
}

void StragglerEvent::setStragglerType(bool stragglerType)
{
    this->stragglerType = stragglerType;
}

const vector<const NegativeEvent*> StragglerEvent::getEventsToCancel() const
{
    return eventsToCancel;
}




