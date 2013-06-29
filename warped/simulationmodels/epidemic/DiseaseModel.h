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

#ifndef DISEASE_MODEL_H
#define DISEASE_MODEL_H

#include "Person.h"
#include <cmath>
#include "RandomNumGen.h"

#define PROB_MULTIPLIER 100

using namespace std;

class DiseaseModel {

public:

	/* Constructor */
	DiseaseModel(	float transmissibility,
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
					float probUIU ) :
		transmissibility(transmissibility),
		latentDwellTime(latentDwellTime),
		incubatingDwellTime(incubatingDwellTime),
		infectiousDwellTime(infectiousDwellTime),
		asymptDwellTime(asymptDwellTime),
		latentInfectivity(latentInfectivity),
		incubatingInfectivity(incubatingInfectivity),
		infectiousInfectivity(infectiousInfectivity),
		asymptInfectivity(asymptInfectivity),
		probULU(probULU),
		probULV(probULV),
		probURV(probURV),
		probUIV(probUIV),
		probUIU(probUIU) {

		randNumGen.seedRandNumGen();
	}

	/* Destructor */
	~DiseaseModel() {}

	/* Probabilistic Timed Transition System */
	void diseasePTTS( Person *person ) {
		/* the state time part need furthure implementation */
		string iState = person->infectionState;
		if ("latent" == iState){
			latentDwellTime;
			person->infectionState = "infectious";	
		}
		if ("incubating" == iState){
			incubatingDwellTime;		
			person->infectionState = "asympt";	
		}
		if ("infectious" == iState){
			infectiousDwellTime;
			person->infectionState = "recovered";	
		}
		if ("asympt" == iState){
			asymptDwellTime;
			person->infectionState = "recovered";	
		}
	}

	/* Reaction function */
	void diseaseReaction( map <unsigned int, Person *> *personMap ) {

		unsigned int latentNum = 0, incubatingNum = 0, 
						infectiousNum = 0, asymptNum = 0, uninfectedNum = 0;
		vector< Person* > uninfectedVec;

		for( map<unsigned int, Person*>::iterator mapIter = personMap->begin(); 
												mapIter != personMap->end(); mapIter++) {

			string infectionState = (mapIter->second)->infectionState;
			if ("uninfected" == infectionState) {
				uninfectedNum++;
				uninfectedVec.push_back(mapIter->second);
			} else if ("latent" == infectionState) {
				latentNum++;
			} else if ("incubating" == infectionState) {
				incubatingNum++;
			} else if ("infectious" == infectionState) {
				infectiousNum++;
			} else if ("asympt" == infectionState) {
				asymptNum++;
			} else {
				ASSERT("recovered" == infectionState);
			}
		}

		/* If some persons remain uninfected */
		if ( uninfectedNum ) {
			for( vector <Person*>::iterator vecIter = uninfectedVec.begin(); 
												vecIter != uninfectedVec.end(); vecIter++) {
				double susceptibility = (*vecIter)->susceptibility;
				double suscepMultTrans = (double) susceptibility * transmissibility;
				double probLatent = 1.0, probIncubating = 1.0, probInfectious = 1.0, 
													probAsympt = 1.0, diseaseProb = 1.0;
				if (latentNum) {
					probLatent -= (double) suscepMultTrans * latentInfectivity;
					probLatent = pow( probLatent, (double) latentNum );
				}
				if (incubatingNum) {
					probIncubating -= (double) suscepMultTrans * incubatingInfectivity;
					probIncubating = pow( probIncubating, (double) incubatingNum );
				}
				if (infectiousNum) {
					probInfectious -= (double) suscepMultTrans * infectiousInfectivity;
					probInfectious = pow( probInfectious, (double) infectiousNum);
				}
				if (asymptNum) {
					probAsympt -= (double) suscepMultTrans * asymptInfectivity;
					probAsympt = pow( probAsympt, (double) asymptNum);
				}

				/* Incomplete : Formula will change with introduction of time */
				diseaseProb -= (probLatent * probIncubating * probInfectious * probAsympt);

				/* Decide whether the person gets infected */
				unsigned int diseaseNum = (unsigned int) diseaseProb * PROB_MULTIPLIER;
				if( diseaseNum > randNumGen.genRandNum(PROB_MULTIPLIER) ) {

					unsigned int randNum = randNumGen.genRandNum(PROB_MULTIPLIER);

					/* Check whether the person is vaccinated */
					if( (*vecIter)->isVaccinated ) {

						unsigned int ulvNum = (unsigned int) probULV * PROB_MULTIPLIER;
						unsigned int urvPlusUlvNum = (unsigned int) (probURV + probULV) * PROB_MULTIPLIER;
						if( ulvNum > randNum ) {
							(*vecIter)->infectionState = "latent";
						} else if( urvPlusUlvNum > randNum ) {
							(*vecIter)->infectionState = "recovered";
						} else {
							(*vecIter)->infectionState = "incubating";
						}

					} else {

						unsigned int uluNum = (unsigned int) probULU * PROB_MULTIPLIER;
						if( uluNum > randNum ) {
							(*vecIter)->infectionState = "latent";
						} else {
							(*vecIter)->infectionState = "incubating";
						}
					}
				}
			}
		}
	}

private:

	/* Transmissibility */
	float transmissibility;

	/* Disease dwell times */
	unsigned int latentDwellTime, incubatingDwellTime, 
					infectiousDwellTime, asymptDwellTime;

	/* Disease state infectivity */
	float latentInfectivity, incubatingInfectivity, 
					infectiousInfectivity, asymptInfectivity;

	/* Disease state transition probabilities */
	float probULU, probULV, probURV, probUIV, probUIU;

	/* Random Number Generator */
	RandomNumGen randNumGen;
};

#endif
