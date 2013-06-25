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

enum eventType {arrival, departure, statechage,defualt};		

class EpidemicEvent: public DefaultEvent {
public:

	/* constructor usded by application */
	EpidemicEvent(const VTime &initSendTime,
				const VTime &initRecvTime,
				SimulationObject *initSender,
				SimulationObject *initReceiver)	: 
		DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
		person(NULL),randSeed(0),
		pid(0){
		}

	/* Destructor */
	~EpidemicEvent(){
		delete person;
	}
	
	unsigned int getEventSize() const { return sizeof(EpidemicEvent); }

    static Serializable* deserialize( SerializedInstance *instance ){
		VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
		VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
		unsigned int senderSimManID = instance->getUnsigned();
		unsigned int senderSimObjID = instance->getUnsigned();
		unsigned int receiverSimManID = instance->getUnsigned();
		unsigned int receiverSimObjID = instance->getUnsigned();
		unsigned int eventId = instance->getUnsigned();
		
		ObjectID sender(senderSimObjID, senderSimManID);
		ObjectID receiver(receiverSimObjID, receiverSimManID);
			
	}

	void serialize( SerializedInstance *addTo ) const {
		Event::serialize(addTo);
		addTo->addUnsigned(randSeed);
		addTo->addUnsigned(pid);
	}
	
	bool eventCompare(const Event* event){
	}

	static const string &getEpidemicEventDataType(){
		static string epidemicEventDataType = "EpidemicEvent";
    	return epidemicEventDataType;
	}

private:
	// data for arrival event
	Person *person;
	unsigned int randSeed;
	// data for departure and state change event
	unsigned int pid;
	//specify the event type
	eventType currEventType;
};

#endif

