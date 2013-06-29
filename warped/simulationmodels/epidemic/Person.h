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

#ifndef PERSON_H
#define PERSON_H

#include "IntVTime.h"

#define INVALID_PID 0

using namespace std;

class Person {

public:

	/* Constructor */
	Person( unsigned int pid,
			double susceptibility,
			string vaccinationStatus,
			string infectionState,
			IntVTime arrivalTimeAtLoc,
			IntVTime lastStateChangeTime ) :
		pid(pid),
		susceptibility(susceptibility),
		vaccinationStatus(vaccinationStatus),
		infectionState(infectionState),
		arrivalTimeAtLoc(arrivalTimeAtLoc),
		lastStateChangeTime(lastStateChangeTime) {}

	/* Destructor */
	~Person() {}

	/* PID */
	unsigned int pid;

	/* Susceptibility */
	double susceptibility;

	/* Vaccination status */
	string vaccinationStatus;

	/* Infection state */
	string infectionState;

	/* Time when the person arrived at the location */
	IntVTime arrivalTimeAtLoc;

	/* Time when the last infection state change occured */
	IntVTime lastStateChangeTime;
};

#endif
