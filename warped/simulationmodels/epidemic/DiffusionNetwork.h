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
	DiffusionNetwork( RandomNumGen *randNumGen ) : 
		randNumGen(randNumGen) {
	}

	/* Destructor */
	~DiffusionNetwork() {}

	/* Choose a location */
	const string &getLocationName( map <string, unsigned int> *locationMap ) {

		/* Fill this up to return a location of choice */
		/* It can also choose none, in which case "" will be returned */
	}

	/* Get travel time for the chosen location */
	unsigned int travelTimeToChosenLoc (	map <string, unsigned int> *locationMap,
											unsigned int travelTimeToHub,
											string locationChoice			) {
		/* Travel time = travel time from hub to location + travel time from home loc to hub */
	}

private:

	/* Random class variable */
	RandomNumGen *randNumGen;
};

#endif
