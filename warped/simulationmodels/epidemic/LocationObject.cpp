// Copyright (c) The University of Cincinnati.
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT. UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.
 
#include "LocationObject.h"
#include "LocationState.h"
#include "IntVTime.h"

#include <vector>
#include <map>

using namespace std;

LocationObject::LocationObject( string locationName,
								float transmissibility,
								unsigned int latentDwellTime,
								unsigned int incubatingDwellTime,
								unsigned int infectiousDwellTime,
								unsigned int asymptDwellTime,
								float probULU,
								float probULV,
								float probURV,
								float probUIV,
								float probUIU,
								vector <Person *> *personVec,
								unsigned int travelTimeToHub) : 
	locationName(locationName),
	personVec(personVec),
	travelTimeToHub(travelTimeToHub) {

	diseaseModel = new DiseaseModel(	transmissibility, latentDwellTime, 
										incubatingDwellTime, infectiousDwellTime, asymptDwellTime,
										probULU, probULV, probURV, probUIV, probUIU   );
}

LocationObject::~LocationObject() {

	deallocateState(getState());

	for( vector<Person *>::iterator vecIter = personVec->begin();
							vecIter != personVec->end(); vecIter++ ) {
		delete *vecIter;
	}
	delete personVec;
	delete diseaseModel;
}

void LocationObject::initialize() {
	LocationState *myState = dynamic_cast<LocationState*>(getState());
	myState->populateLocation(personVec);
}

void LocationObject::finalize() {}

void LocationObject::executeProcess() {

	LocationState *myState = static_cast<LocationState*>(getState());
	IntVTime sendTime = static_cast<const IntVTime&> (getSimulationTime());
	EpidemicEvent *recvEvent = NULL;
	SimulationObject *receiver = NULL;

	while(haveMoreEvents() == true) {

		recvEvent = (EpidemicEvent *) getEvent();
		if ( recvEvent != NULL ) {

		}
	}
}

State* LocationObject::allocateState() {
	return (new LocationState());
}

void LocationObject::deallocateState( const State *state ) {
	delete state;
}

void LocationObject::reclaimEvent(const Event *event) {
	delete event;
}

