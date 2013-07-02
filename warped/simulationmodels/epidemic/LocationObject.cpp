// Copyright (c) The University of Cincinnati.
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT. UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.
 
#include "LocationObject.h"
#include "LocationState.h"
#include "IntVTime.h"

#include <vector>
#include <map>

#define BINARY 2

using namespace std;

LocationObject::LocationObject( string locationName,
								float transmissibility,
								unsigned int latentDwellTime,
								unsigned int incubatingDwellTime,
								unsigned int infectiousDwellTime,
								unsigned int asymptDwellTime,
								float latentInfectivity,
								float incubatingInfectivity,
								float infectiousInfectivity,
								float asymptInfectivity,
								float probULU,
								float probULV,
								float probURV,
								float probUIV,
								float probUIU,
								unsigned int locStateRefreshInterval,
								unsigned int locDiffusionTrigInterval,
								vector <Person *> *personVec,
								unsigned int travelTimeToHub) : 
		locationName(locationName),
		personVec(personVec),
		locStateRefreshInterval(locStateRefreshInterval),
		locDiffusionTrigInterval(locDiffusionTrigInterval) {

	/* Create and seed the random number class */
	randNumGen = new RandomNumGen();
	randNumGen->seedRandNumGen();

	/* Create the disease model */
	diseaseModel = new DiseaseModel(	transmissibility, latentDwellTime, 
										incubatingDwellTime, infectiousDwellTime, 
										asymptDwellTime, latentInfectivity, 
										incubatingInfectivity, infectiousInfectivity, 
										asymptInfectivity, probULU, probULV, probURV, 
										probUIV, probUIU, randNumGen  );

	/* Create the diffusion network */
	diffusionNetwork = new DiffusionNetwork( randNumGen, travelTimeToHub );
}

LocationObject::~LocationObject() {

	deallocateState(getState());

	for( vector<Person *>::iterator vecIter = personVec->begin();
							vecIter != personVec->end(); vecIter++ ) {
		delete *vecIter;
	}

	delete personVec;
	delete randNumGen;
	delete diseaseModel;
	delete diffusionNetwork;
}

void LocationObject::initialize() {
	LocationState *myState = dynamic_cast<LocationState*>(getState());
	myState->populateLocation(personVec);

	/* Create and send the initial events */
	IntVTime currentTime = static_cast<const IntVTime&> (getSimulationTime());
	refreshLocStateEvent(currentTime);
	triggerDiffusionEvent(currentTime);
}

void LocationObject::finalize() {}

void LocationObject::executeProcess() {

	LocationState *myState = static_cast<LocationState*>(getState());
	IntVTime currentTime = static_cast<const IntVTime&> (getSimulationTime());
	EpidemicEvent *recvEvent = NULL;
	SimulationObject *receiver = NULL;

	while(haveMoreEvents() == true) {

		recvEvent = (EpidemicEvent *) getEvent();
		if ( recvEvent != NULL ) {

			/* Check if the event was sent by the same location */
			if( recvEvent->getSender() == *(this->getObjectID()) ) {

				if( DISEASE == recvEvent->getDiseaseOrDiffusion() ) {
					diseaseModel->diseaseReaction(	myState->getPersonMap(),
													currentTime.getApproximateIntTime() );
					refreshLocStateEvent(currentTime);

				} else if( DIFFUSION == recvEvent->getDiseaseOrDiffusion() ){
					migrateLocationEvent(currentTime, myState);
					triggerDiffusionEvent(currentTime);

				} else {
					cerr << "Invalid event at " << locationName << endl;
					abort();
				}
			} else {
				int timeSpentInCurrState = 
						currentTime.getApproximateIntTime() - 
											recvEvent->getTimeSpentInCurrState();
				ASSERT(timeSpentInCurrState >= 0);
				Person *person = new Person(recvEvent->getPID(),
											recvEvent->getSusceptibility(),
											recvEvent->getVaccinationStatus(),
											recvEvent->getInfectionState(),
											currentTime.getApproximateIntTime(),
											timeSpentInCurrState);
				(myState->getPersonMap())->insert( 
								std::pair <unsigned int, Person *> (recvEvent->getPID(), person) );
			}
		}
	}
}

State* LocationObject::allocateState() {
	return (new LocationState());
}

void LocationObject::deallocateState( const State *state ) {
	delete state;
}

void LocationObject::reclaimEvent(const Event *event) {
	delete event;
}

void LocationObject::refreshLocStateEvent( IntVTime currentTime ) {

	EpidemicEvent *refreshEvent = new EpidemicEvent(	currentTime,
														currentTime + (int)locStateRefreshInterval,
														this, this, NULL, DISEASE  );
	this->receiveEvent(refreshEvent);
}

void LocationObject::triggerDiffusionEvent( IntVTime currentTime ) {

	EpidemicEvent *diffusionEvent = new EpidemicEvent(	currentTime,
														currentTime + (int)locDiffusionTrigInterval,
														this, this, NULL, DIFFUSION  );
	this->receiveEvent(diffusionEvent);
}

void LocationObject::migrateLocationEvent(	IntVTime currentTime,
											LocationState *locationState  ) {

	/* Randomly decide whether to migrate any person */
	if( randNumGen->genRandNum(BINARY) ) {

		/* Decide a random location */
		string selectedLocation = diffusionNetwork->getLocationName();
		if( "" != selectedLocation ) {

			int travelTime = (int) ( diffusionNetwork->travelTimeToChosenLoc(selectedLocation) );
			SimulationObject *receiver = getObjectHandle(selectedLocation);

			/* Decide a random person */
			Person *person = diffusionNetwork->getPerson( locationState->getPersonMap() );

			if(person) {
				EpidemicEvent *migrateLocEvent = new EpidemicEvent(	currentTime,
																	currentTime + travelTime,
																	this,
																	receiver,
																	person,
																	DIFFUSION );
				receiver->receiveEvent(migrateLocEvent);
				locationState->deletePersonFromLocation(person);
			}
		}
	}
}


