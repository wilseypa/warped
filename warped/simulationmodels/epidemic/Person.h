#ifndef PERSON_H
#define PERSON_H

#define INVALID_PID 0

using namespace std;

class Person {

public:

	/* Constructor */
	Person( unsigned int pid,
			double susceptibility,
			string vaccinationStatus,
			string infectionState,
			int arrivalTimeAtLoc,
			int lastStateChangeTime ) :
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
	int arrivalTimeAtLoc;

	/* Time when the last infection state change occured */
	int lastStateChangeTime;
};

#endif
