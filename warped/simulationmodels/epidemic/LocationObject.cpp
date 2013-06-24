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
 
#include "LocationObject.h"
#include "LocationState.h"
#include "IntVTime.h"

#include <vector>
#include <map>

using namespace std;

LocationObject::LocationObject( string locationName,
								float transmissibility,
								vector <Person *> *personVec) : 
	locationName(locationName),
	transmissibility(transmissibility),
	personVec(personVec) {
}

LocationObject::~LocationObject() {

	deallocateState(getState());

	for( vector<Person *>::iterator vecIter = personVec->begin();
							vecIter != personVec->end(); vecIter++ ) {
		delete *vecIter;
	}
	delete personVec;
}

void LocationObject::initialize() {}

void LocationObject::finalize() {}

void LocationObject::executeProcess() {}

State* LocationObject::allocateState() {
	return (new LocationState());
}

void LocationObject::deallocateState( const State *state ) {
	delete state;
}

void LocationObject::reclaimEvent(const Event *event) {
	delete event;
}

