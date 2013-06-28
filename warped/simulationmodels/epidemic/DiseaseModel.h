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
	void diseasePTTS( /* args have to be decided */ ){
	}

	/* Reaction function */
	map<Person*,double> diseaseReaction( map <unsigned int, Person *> *personMap ){
    	int latentNum = 0;
    	int incubatingNum = 0;
    	int asymptNum = 0;
    	int infectiousNum = 0;
    	int uninfectedNum = 0;
   		vector<Person*> uninfectedSet;
    	map<Person*,double> getInfectedProbability;

    	for(map<unsigned int, Person*>::iterator it = personMap->begin(); it!=personMap->end(); it++){
        	if ("latent" == (it->second)->infectionState)
            	latentNum++;
        	if ("incubating" == (it->second)->infectionState)
            	incubatingNum++;
        	if ("asympt" == (it->second)->infectionState)
            	asymptNum++;
        	if ("infectious" == (it->second)->infectionState)
            	infectiousNum++;
        	if ("uninfected" == (it->second)->infectionState){
            	uninfectedNum++;
            	uninfectedSet.push_back(it->second);
        	}
    	}

    	if(0==uninfectedNum) // all people in the locatoin are infected or recovery
        	return getInfectedProbability;
    	else{
        	for(vector<Person*>::iterator it=uninfectedSet.begin(); it!=uninfectedSet.end(); it++){
            	double si = (*it)->susceptibility;
            	double pLatent;
            	double pIncubating;
            	double pInfectious;
            	double pAsympt;
            	if (0!=latentNum){
                	pLatent = 1 - si*transmissibility*latentInfectivity;
                	pLatent = pow(pLatent,latentNum);
            	}
            	else
                	pLatent = 1.0;
            	if (0!=incubatingNum){
                	pIncubating = 1 - si*transmissibility*incubatingInfectivity;
                	pIncubating = pow(pIncubating,incubatingNum);
            	}
           		else
                	pIncubating = 1.0;
            	if(0!=infectiousNum){
                	pInfectious = 1 - si*transmissibility*infectiousInfectivity;
                	pInfectious = pow(pInfectious,infectiousNum);                                    	}
				else
                	pInfectious = 1.0;
            	if(0!=asymptNum){
                	pAsympt = 1 - si*transmissibility*asymptInfectivity;
                	pAsympt = pow(pAsympt,asymptNum);
            	}
            	else
                	pAsympt = 1.0;
            	double multiply = pLatent*pIncubating*pInfectious*pAsympt;
            	double time = 2.0;
            	double pi = 1 - pow(multiply,time);
            	getInfectedProbability.insert(pair<Person*,double>((*it),pi));
        	}
        return getInfectedProbability;
    }
}

private:

	/* Transmissibility */
	float transmissibility;

	/* Disease dwell times */
	unsigned int latentDwellTime, incubatingDwellTime, 
					infectiousDwellTime, asymptDwellTime;

	/* Disease state infectivity */
	double latentInfectivity, incubatingInfectivity, 
					infectiousInfectivity, asymptInfectivity;

	/* Disease state transition probabilities */
	float probULU, probULV, probURV, probUIV, probUIU;
};

#endif
