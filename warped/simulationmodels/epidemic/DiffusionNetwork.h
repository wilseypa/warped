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
	const string &getLocationName() {

		/* Fill this up to return a location of choice */
		/* It can also choose none, in which case "" will be returned */
		string nullString = "";
		string &locationName = nullString;
		unsigned int locationNum = travelTimeMap.size();
		if ( locationNum ) { // there is at least one location in travelTimeMap
			randNumGen = new RandomNumGen();
			randNumGen->seedRandNumGen();
			unsigned int locationID = randNumGen->genRandNum(locationNum);	
			map<string, unsigned int>::iterator mapIter = travelTimeMap.begin();
			for(int count = 0; count < locationID ; count++ )
				mapIter++;
			locationName = mapIter->first;
			return locationName;
		}
		else{
			return locationName;
		}
	}

	/* Get travel time for the chosen location */
	unsigned int travelTimeToChosenLoc( string locationChoice ) {

		/* Travel time = travel time from hub to location + travel time from home loc to hub */
		unsigned int travelTime = travelTimeMap[locationChoice] + travelTimeToHub;
		return travelTime; 	
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
