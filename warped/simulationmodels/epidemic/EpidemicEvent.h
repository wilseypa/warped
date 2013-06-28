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
					Person *person,
					unsigned int randSeed  ) : 
			DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
			randSeed(randSeed), pid(0), susceptibility(0.0), infectionState("") {

		if( person ) {
			pid = person->pid;
			susceptibility = person->susceptibility;
			infectionState = person->infectionState;
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

		event->setRandSeed( instance->getUnsigned() );
		event->setPID ( instance->getUnsigned() );
		event->setSusceptibility ( instance->getDouble() );
		event->setInfectionState ( instance->getString() );

		delete sendTime;
		delete receiveTime;

		return event;
	}

	void serialize( SerializedInstance *addTo ) const {
		Event::serialize(addTo);
		addTo->addUnsigned(randSeed);
		addTo->addUnsigned(pid);
		addTo->addDouble(susceptibility);
		addTo->addString(infectionState);
	}

	bool eventCompare( const Event* event ) {

		EpidemicEvent *thisEvent = (EpidemicEvent *) event;

		return (	compareEvents ( this, event ) && 
					( randSeed == thisEvent->getRandSeed() ) && 
					( pid == thisEvent->getPID() ) &&
					( susceptibility == thisEvent->getSusceptibility() ) &&
					( infectionState == thisEvent->getInfectionState() )    );
	}

	static const string &getEpidemicEventDataType(){
		static string epidemicEventDataType = "EpidemicEvent";
		return epidemicEventDataType;
	}

	const string &getDataType() const { return getEpidemicEventDataType(); }

	unsigned int getEventSize() const { return sizeof(EpidemicEvent); }

	void setRandSeed(unsigned int seed) { randSeed = seed; }

	unsigned int getRandSeed() { return randSeed; }

	void setPID( unsigned int personId ) { pid = personId; }

	unsigned int getPID() { return pid; }

	void setSusceptibility( double susValue ) { susceptibility = susValue; }

	double getSusceptibility() { return susceptibility; }

	void setInfectionState( const string stateValue ) { infectionState = stateValue; }

	const string &getInfectionState() { return infectionState; }

private:

	EpidemicEvent(  const VTime &initSendTime,
					const VTime &initRecvTime,
					const ObjectID &initSender,
					const ObjectID &initReceiver,
					const unsigned int eventId  ) :
			DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver, eventId ),
			randSeed(0), pid(0), susceptibility(0), infectionState("") {
	}

	/* Random seed */
	unsigned int randSeed;

	/* Person ID */
	unsigned int pid;

	/* Person's susceptibility */
	double susceptibility;

	/* Person's infection state */
	string infectionState;
};

#endif

