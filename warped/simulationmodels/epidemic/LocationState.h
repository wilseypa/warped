#ifndef LOCATION_STATE_H
#define LOCATION_STATE_H

#include "State.h"
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
									tempPerson->vaccinationStatus,
									tempPerson->infectionState, 
									tempPerson->arrivalTimeAtLoc,
									tempPerson->lastStateChangeTime );
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

	void deletePersonFromLocation( Person *person ) {

		if(person) {
			personMap.erase( personMap.find(person->pid) );
		}
	}

private:

	/* Person map */
	map <unsigned int, Person *> personMap;
};

#endif

