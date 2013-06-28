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
	}

	/* Destructor */
	~DiseaseModel() {}

	/* Probabilistic Timed Transition System */
	void diseasePTTS( /* args have to be decided */ ) {
	}

	/* Reaction function */
	void diseaseReaction( map <unsigned int, Person *> *personMap ) {
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
};

#endif
