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

#ifndef LOCATION_OBJECT_H
#define LOCATION_OBJECT_H

#include "SimulationObject.h"
#include "LocationState.h"
#include "Person.h"
#include "EpidemicEvent.h"
#include <vector>
#include <map>

using namespace std;

/* LocationObject Class */

class LocationObject : public SimulationObject {

public:

	/* Default Constructor */
	LocationObject( string locationName,
					float transmissibility,
					vector <Person *> *personVec );

	/* Destructor */
	~LocationObject();

	/* Initialize the simulation object */
	void initialize();

	/* Finalize the simulation object before termination */
	void finalize();

	/* Execute the scheduled event */
	void executeProcess();

	/* Allocate a new state */
	State* allocateState();

	/* Deallocate a state */
	void deallocateState(const State *state);

	/* Reclaim an event */
	void reclaimEvent(const Event *event);

	/* Accessor for objectName */
	const string &getName() const {return locationName;}

private:

	/* Location name */
	string locationName;

	/* Transmissibility */
	float transmissibility;

	/* Initial population */
	vector <Person *> *personVec;
};

#endif
