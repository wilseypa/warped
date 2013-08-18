#ifndef DIFFUSION_NETWORK_H
#define DIFFUSION_NETWORK_H

#include "RandomNumGen.h"
#include <map>

class DiffusionNetwork {

public:

	/* Constructor */
	DiffusionNetwork( RandomNumGen *randNumGen, unsigned int travelTimeToHub ) : 
		randNumGen(randNumGen),
		travelTimeToHub(travelTimeToHub) {}

	/* Destructor */
	~DiffusionNetwork() {}

	/* To choose or not to choose a random location, that is the question :) */
	const string getLocationName() {

		string locationName = "";
		unsigned int locationNum = travelTimeMap.size();

		/* Check if there is atleast one location in travelTimeMap */
		if ( locationNum ) {
			unsigned int locationID = randNumGen->genRandNum(locationNum);	

			map<string, unsigned int>::iterator mapIter = travelTimeMap.begin();
			for(unsigned int count = 0; count < locationID ; count++ ) {
				mapIter++;
			}
			locationName = mapIter->first;
		}
		return locationName;
	}

	/* Get travel time for the chosen location */
	unsigned int travelTimeToChosenLoc( string locationChoice ) {
		return (travelTimeMap[locationChoice] + travelTimeToHub);
	}

	/* To choose or not to choose a random person, that is the question :) */
	Person *getPerson( map <unsigned int, Person *> *personMap ) {

		Person *person = NULL;
		unsigned int personCount = personMap->size();

		/* Check if there is atleast one person present at location */
		if ( personCount ) {
			unsigned int randomNum = randNumGen->genRandNum(personCount);	

			map<unsigned int, Person *>::iterator mapIter = personMap->begin();
			for(unsigned int count = 0; count < randomNum ; count++ ) {
				mapIter++;
			}
			person = mapIter->second;
		}
		return person;
	}

	/* Populate the travel cost details */
	void populateTravelCost( map <string, unsigned int> *travelMap, string locationName ) {
		travelTimeMap.insert( travelMap->begin(), travelMap->end() );
		travelTimeMap.erase(locationName);
	}

private:

	/* Random class variable */
	RandomNumGen *randNumGen;

	/* Travel time from Home location to Hub */
	unsigned int travelTimeToHub;

	/* Location travel time from Hub */
	map <string, unsigned int> travelTimeMap;
};

#endif
