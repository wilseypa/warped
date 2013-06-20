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

#ifndef LOCATIONOBJECT_H
#define LOCATIONOBJECT_H

#include "SimulationObject.h"
#include "PersonState.h"

/* LocationObject Class */

class LocationObject : public SimulationObject {

public:

	/* Default Constructor */
	LocationObject();

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

	/* Report an error */
	void reportError(const string&, SEVERITY);

	/* Accessor for objectName */
	const string &getName() const {return locationName;}

private:

	/* Location name */
	string locationName;

	/* Occupant set of persons at this location */
	vector <PersonState*> occSet;

	/* Map from ID to person  */
	map <unsigned int, PersonState*> occMap;

	/* Departed set of persons at this location */
	vector <PersonState*> depSet;

	/* Map from ID to departed person */
	map <unsigned int, PersonState*> depMap;

};

#endif

