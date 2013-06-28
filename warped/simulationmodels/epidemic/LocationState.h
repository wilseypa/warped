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
#include "Person.h"

#include <vector>
#include <map>

using namespace std;

class LocationState : public State {

public:

	/* Default Constructor */
	LocationState() {}

	/* Destructor */
	~LocationState() {
		map <unsigned int, Person *>::iterator mapIter;
		for(mapIter = personMap.begin(); mapIter != personMap.end(); mapIter++) {
			delete (mapIter->second);
		}
	}

	/* Copy the state */
	void copyState(const State* toCopy) {
		Person *person, *tempPerson;
		ASSERT(toCopy != NULL);
		const LocationState *copy = static_cast<const LocationState*>(toCopy);

		map <unsigned int, Person *> tempMap = copy->personMap;
		map <unsigned int, Person *>::iterator mapIter;

		for( mapIter = tempMap.begin(); mapIter != tempMap.end(); mapIter++ ) {
			tempPerson = mapIter->second;
			person = new Person( 	tempPerson->pid,
									tempPerson->susceptibility,
									tempPerson->isVaccinated,
									tempPerson->infectionState 	);
			personMap.insert( std::pair <unsigned int, Person *> (mapIter->first, person) );
		}
	}

	/* Get the state size */
	unsigned int getStateSize() const { return sizeof(LocationState); }

	void populateLocation( vector <Person *> *personVec ) {

		vector <Person *>::iterator vecIter;
		for( vecIter = personVec->begin(); vecIter != personVec->end(); vecIter++ ) {
			Person *person = *vecIter;
			personMap.insert( std::pair<unsigned int, Person *>(person->pid, person) );
		}
	}

	/* Fetch the Person map address */
	map <unsigned int, Person *> *getPersonMap() {
		return &personMap;
	}

private:

	/* Person map */
	map <unsigned int, Person *> personMap;
};

#endif

