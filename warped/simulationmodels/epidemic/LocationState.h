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

#ifndef LOCATION_STATE_H
#define LOCATION_STATE_H

#include "State.h"
#include "IntVTime.h"

#include <map>

using namespace std;

class LocationState : public State {

public:

	/* Default Constructor */
	LocationState() {};

	/* Destructor */
	~LocationState() {};

	/* Copy the state */
	void copyState(const State* toCopy) {
		ASSERT(toCopy != NULL);
		const LocationState *copy = static_cast<const LocationState*>(toCopy);

		map <unsigned int, string> tempMap = copy->infectionState;
		map <unsigned int, string>::iterator mapIter;
		for( mapIter = tempMap.begin(); mapIter != tempMap.end(); mapIter++ ) {
			infectionState.insert( std::pair <unsigned int, string> (mapIter->first, mapIter->second) );
		}
	};

	/* Get the state size */
	unsigned int getStateSize() const { return sizeof(LocationState); }

private:

	/* Map person ID to Infecton state */
	map <unsigned int, string> infectionState;
};

#endif

