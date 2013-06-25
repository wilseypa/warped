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

enum eventType {ARRIVAL, DEPARTURE, STATECHANGE,DEFAULT};		

class EpidemicEvent: public DefaultEvent {
public:

	/* constructor usded by application */
	EpidemicEvent(const VTime &initSendTime,
				const VTime &initRecvTime,
				SimulationObject *initSender,
				SimulationObject *initReceiver)	: 
		DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
		person(NULL),randSeed(0),
		pid(0),currEventType(DEFAULT){
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

		EpidemicEvent *event = new EpidemicEvent(*sendTime, *receiveTime, sender,receiver,eventId);	
		
		event->setPerson(instance->getUnsigned(),instance->getDouble(),instance->getString());
		event->setRandSeed(instance->getUnsigned());
		event->setPid(instance->getUnsigned());
		event->setEventType(instance->getInt());
		
		delete sendTime;
		delete receiveTime;
		return event;
	}
	
	void serialize( SerializedInstance *addTo ) const {
		Event::serialize(addTo);
		addTo->addUnsigned(person->pid);
		addTo->addDouble(person->susceptibility);
		addTo->addString(person->infectionState);
		addTo->addUnsigned(randSeed);
		addTo->addUnsigned(pid);
		int typeID = 0;
		if (currEventType == ARRIVAL){
			typeID = 1;
			addTo->addInt(typeID);
		}
		else if (currEventType == DEPARTURE){
			typeID = 2;
			addTo->addInt(typeID);
		}
		else if (currEventType == STATECHANGE){
			typeID = 3;
			addTo->addInt(typeID);
		}
		else {  
			addTo->addInt(typeID);
		}
	}
	
	bool eventCompare(const Event* event){
		EpidemicEvent* thisEvent = (EpidemicEvent*) event;
		return (compareEvents(this,event)&&
				randSeed == thisEvent->getRandSeed()&&
				pid == thisEvent->getPid()&&
				currEventType == getEventType()&&
				person == getPerson());
	}

	
	static const string &getEpidemicEventDataType(){
		static string epidemicEventDataType = "EpidemicEvent";
    	return epidemicEventDataType;
	}
	
	const string &getDataType() const {
    	return getEpidemicEventDataType();
  	}

	void setPerson(unsigned int personid, double susValue, string stateValue ){
		person->pid = personid;
		person->susceptibility = susValue;
		person->infectionState = stateValue;
	}

	void setRandSeed(unsigned int seed){
		randSeed = seed;
	}

	void setPid(unsigned personid){
		pid = personid;
	}

	void setEventType(int thisType){
		if(1==thisType)
			currEventType = ARRIVAL;
		if(2==thisType)
			currEventType = DEPARTURE;
		if(3==thisType)
			currEventType = STATECHANGE;
		if(0==thisType)
			currEventType = DEFAULT;
	}
	
	Person* getPerson(){return person;}
	unsigned int getRandSeed(){ return randSeed; }
	unsigned int getPid() { return pid;}
	eventType getEventType(){return currEventType;}

private:

EpidemicEvent(const VTime &initSendTime,
              const VTime &initRecvTime,
			  const ObjectID &initSender,
			  const ObjectID &initReceiver,
			  const unsigned int eventIdVal) :
			DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver,eventIdVal ),
			person(NULL),randSeed(0),
			pid(0),currEventType(DEFAULT){
			}

	// data for arrival event
	Person *person;
	unsigned int randSeed;
	// data for departure and state change event
	unsigned int pid;
	//specify the event type
	eventType currEventType;
};

#endif

