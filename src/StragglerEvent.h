#ifndef STRAGGLEREVENT_H_
#define STRAGGLEREVENT_H_


#include "warped.h"
#include "Event.h"
#include "EventId.h"
#include <vector>

class NegativeEvent;
using namespace std;

class StragglerEvent: public Event {
public:
	StragglerEvent(const Event* receivedEvent, bool stragglerType) :
		positiveEvent(receivedEvent), stragglerType(stragglerType) {
	}
	StragglerEvent(const Event* receivedEvent, bool stragglerType,
			const vector<const NegativeEvent *> &events) :
		positiveEvent(receivedEvent), stragglerType(stragglerType),
				eventsToCancel(events) {
	}
	virtual ~StragglerEvent() {
	}

	inline const ObjectID & getSender() const {
		return positiveEvent->getSender();
	}

	inline const ObjectID & getReceiver() const {
		return positiveEvent->getReceiver();
	}

	inline const VTime & getSendTime() const {
		return positiveEvent->getSendTime();
	}

	inline const VTime & getReceiveTime() const {
		return positiveEvent->getReceiveTime();
	}

	inline const EventId & getEventId() const {
		return positiveEvent->getEventId();
	}

	inline unsigned int getEventSize() const {
		return sizeof(StragglerEvent);
	}

	const string & getDataType() const {
		return getStragglerEventDataType();
	}

	static const string & getStragglerEventDataType() {
		static string stragglerEventDataType = "StragglerEvent";
		return stragglerEventDataType;
	}

	friend std::ostream & operator <<(std::ostream & os, const StragglerEvent & event);
	bool eventCompare(const Event*);
	const Event *getPositiveEvent() const;
	bool getStragglerType() const;
	void setStragglerType(bool stragglerType);
    const vector<const NegativeEvent*> getEventsToCancel() const;
private:
    const EventId *id;
    const Event *positiveEvent;
    bool stragglerType;
    const vector<const NegativeEvent *> eventsToCancel;
};
#endif /* STRAGGLEREVENT_H_ */
