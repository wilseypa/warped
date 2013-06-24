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
#include "IntVTime.h"

#include <vector>
#include <map>

using namespace std;

LocationObject::LocationObject( string locationName,
								float transmissibility,
								vector <unsigned int> *pidVec,
								vector <float> *suscepVec,
								vector <string> *infectVec) : 
	locationName(locationName), transmissibility(transmissibility) {

	vector <unsigned int>::iterator vecPIDIter;
	vector <float>::iterator vecSusIter;
	vector <string>::iterator vecInfectIter;

	for(vecPIDIter = pidVec->begin(), vecSusIter = suscepVec->begin(), vecInfectIter = infectVec->begin();
				vecPIDIter != pidVec->end(), vecSusIter != suscepVec->end(), vecInfectIter != infectVec->end();
						vecPIDIter++, vecSusIter++, vecInfectIter++) {

		suscepMap.insert( std::pair<unsigned int, float> (*vecPIDIter, *vecSusIter) );
		infectionMap.insert( std::pair<unsigned int, string> (*vecPIDIter, *vecInfectIter) );
	}
}

LocationObject::~LocationObject() {}

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

