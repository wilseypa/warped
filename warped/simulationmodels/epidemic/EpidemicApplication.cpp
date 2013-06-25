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

#include "EpidemicApplication.h"
#include "LocationObject.h"
#include "EpidemicPartitioner.h"
#include "Person.h"
#include <warped/PartitionInfo.h>
#include <warped/DeserializerManager.h>
#include <utils/ArgumentParser.h>

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;


EpidemicApplication::EpidemicApplication(): inputFileName( "" ), numObjects( 0 ) { }

int EpidemicApplication::initialize( vector<string> &arguments ){

	getArgumentParser().checkArgs( arguments );

	if( inputFileName.empty() ) {
		cerr << "A epidemicSim configuration file must be specified using -simulate" << endl;
		abort();
	}
	return 0;
}

int EpidemicApplication::getNumberOfSimulationObjects(int mgrId) const {
	return numObjects;
}

const PartitionInfo *EpidemicApplication::getPartitionInfo(
					unsigned int numberOfProcessorsAvailable ) {

	EpidemicPartitioner *myPartitioner = new EpidemicPartitioner();
	int numRegions = 0, numLocations = 0, numPersons = 0;
	unsigned int pid = 0;
	double susceptibility = 0.0;
	float transmissibility = 0.0;
	string locationName = "", infectionState = "";

	Person *person;
	vector <SimulationObject*> *locObjs;
	vector <Person *> *personVec;

	ifstream configFile;
	configFile.open( inputFileName.c_str() );
	if(configFile.fail()) {
		cerr << "Could not open file: '" << inputFileName << "'" << endl;
		cerr << "Terminating simulation." << endl;
		abort();
	}
	configFile >> transmissibility >> numRegions;

	/* If needed (to limit the LPs of a region to a specific processor */
	//if(numRegions > numberOfProcessorsAvailable) {
	//	cerr << "Not enough processors alotted for all the regions." << endl;
	//	abort();
	//}

	/* For each region in the simulation, initialize the locations */
	for( int regIndex = 0; regIndex < numRegions; regIndex++ ) {

		locObjs = new vector<SimulationObject*>;  
		numLocations = 0;
		configFile >> numLocations;

		for( int locIndex = 0; locIndex < numLocations; locIndex++ ) {

			personVec = new vector <Person *>;

			configFile >> locationName >> numPersons;

			/* Read each person's details */
			for(int perIndex = 0; perIndex < numPersons; perIndex++) {

				configFile >> pid >> susceptibility >> infectionState;
				person = new Person( pid, susceptibility, infectionState );
				personVec->push_back( person );
			}

			LocationObject *locObject = new LocationObject( locationName,
															transmissibility,
															personVec);
			locObjs->push_back(locObject);
		}
		numObjects += numLocations;

		/* Add the group of objects to the partition information */
		myPartitioner->addObjectGroup(locObjs);
	}

	/* Perform the actual partitioning of groups */
	const PartitionInfo *retval = myPartitioner->partition( NULL, numberOfProcessorsAvailable );

	configFile.close();
	return retval;
}

int EpidemicApplication::finalize() {
	return 0;
}

void EpidemicApplication::registerDeserializers() {
	DeserializerManager::instance()->registerDeserializer(  EpidemicEvent::getEpidemicEventDataType(), 
															&EpidemicEvent::deserialize );
}

ArgumentParser & EpidemicApplication::getArgumentParser() {

	static ArgumentParser::ArgRecord args[] = {
		{ "-simulate", "input file name", &inputFileName, ArgumentParser::STRING, false },
		{ "", "", 0 }
	};

	static ArgumentParser *myArgParser = new ArgumentParser( args );
	return *myArgParser;
}

