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
#include "PersonState.h"

enum eventType {arrival, departure, statechage};		

class EpidemicEvent: public DefaultEvent {

public:

	/* Default Constructor */
	EpidemicEvent();

	/* Destructor */
	~EpidemicEvent();
	
	unsigned int getEventSize() const { return sizeof(EpidemicEvent); }

    static Serializable *deserialize( SerializeInstance *instance){
	}

	void serialize( SerializedInstance *addTo ) const {
	}

	bool eventCompare(const Event* event){
	}

private:
	// data for arrival event
	PersonState* personArrival;
	unsigned int randSeed;
	// data for departure and state change event
	unsigned int pid;
	//specify the event type
	eventType currEventType;
};

#endif

