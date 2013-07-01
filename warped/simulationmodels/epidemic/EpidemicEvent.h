// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

#ifndef EPIDEMIC_EVENT_H
#define EPIDEMIC_EVENT_H

#include "DefaultEvent.h"
#include "IntVTime.h"
#include "SerializedInstance.h"
#include "Person.h"

class EpidemicEvent: public DefaultEvent {

public:

	/* Constructor */
	EpidemicEvent(  const VTime &initSendTime,
					const VTime &initRecvTime,
					SimulationObject *initSender,
					SimulationObject *initReceiver,
					Person *person ) : 
			DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
			pid(0), susceptibility(0.0), vaccinationStatus(""), 
			infectionState(""), timeSpentInCurrState(0) {

		if( person ) {
			pid = person->pid;
			susceptibility = person->susceptibility;
			vaccinationStatus = person->vaccinationStatus;
			infectionState = person->infectionState;
			timeSpentInCurrState = initRecvTime.getApproximateIntTime() - person->lastStateChangeTime;
		}
	}

	/* Destructor */
	~EpidemicEvent() {}

	static Serializable* deserialize( SerializedInstance *instance ) {

		VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
		VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
		unsigned int senderSimManID = instance->getUnsigned();
		unsigned int senderSimObjID = instance->getUnsigned();
		unsigned int receiverSimManID = instance->getUnsigned();
		unsigned int receiverSimObjID = instance->getUnsigned();
		unsigned int eventId          = instance->getUnsigned();

		ObjectID sender(senderSimObjID, senderSimManID);
		ObjectID receiver(receiverSimObjID, receiverSimManID);

		EpidemicEvent *event = new EpidemicEvent(*sendTime, *receiveTime, sender, receiver, eventId);	

		event->setPID ( instance->getUnsigned() );
		event->setSusceptibility ( instance->getDouble() );
		event->setVaccinationStatus (instance->getString() );
		event->setInfectionState ( instance->getString() );
		event->setTimeSpentInCurrState ( instance->getInt() );

		delete sendTime;
		delete receiveTime;

		return event;
	}

	void serialize( SerializedInstance *addTo ) const {
		Event::serialize(addTo);
		addTo->addUnsigned(pid);
		addTo->addDouble(susceptibility);
		addTo->addString(vaccinationStatus);
		addTo->addString(infectionState);
		addTo->addInt( timeSpentInCurrState );
	}

	bool eventCompare( const Event* event ) {

		EpidemicEvent *thisEvent = (EpidemicEvent *) event;

		return (	compareEvents ( this, event ) && 
					( pid == thisEvent->getPID() ) &&
					( susceptibility == thisEvent->getSusceptibility() ) &&
					( vaccinationStatus == thisEvent->getVaccinationStatus() ) &&
					( infectionState == thisEvent->getInfectionState() ) &&
					( timeSpentInCurrState == thisEvent->getTimeSpentInCurrState() )   );
	}

	static const string &getEpidemicEventDataType(){
		static string epidemicEventDataType = "EpidemicEvent";
		return epidemicEventDataType;
	}

	const string &getDataType() const { return getEpidemicEventDataType(); }

	unsigned int getEventSize() const { return sizeof(EpidemicEvent); }

	void setPID( unsigned int personId ) { pid = personId; }

	unsigned int getPID() { return pid; }

	void setSusceptibility( double susValue ) { susceptibility = susValue; }

	double getSusceptibility() { return susceptibility; }

	void setVaccinationStatus( const string statusValue ) { vaccinationStatus = statusValue; }

	const string &getVaccinationStatus() { return vaccinationStatus; }

	void setInfectionState( const string stateValue ) { infectionState = stateValue; }

	const string &getInfectionState() { return infectionState; }

	void setTimeSpentInCurrState( int timeSpent ) { 
		timeSpentInCurrState = timeSpent;
	}

	int getTimeSpentInCurrState() { return timeSpentInCurrState; }

private:

	EpidemicEvent(  const VTime &initSendTime,
					const VTime &initRecvTime,
					const ObjectID &initSender,
					const ObjectID &initReceiver,
					const unsigned int eventId  ) :
			DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver, eventId ),
			pid(0), susceptibility(0), vaccinationStatus(""), 
			infectionState(""), timeSpentInCurrState(0) {
	}

	/* Person ID */
	unsigned int pid;

	/* Person's susceptibility */
	double susceptibility;

	/* Vaccination status */
	string vaccinationStatus;

	/* Person's infection state */
	string infectionState;

	/* Time spent in the current state */
	int timeSpentInCurrState;
};

#endif

