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
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
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

vector<SimulationObject *> *EpidemicApplication::getSimulationObjects() {

	vector<SimulationObject *> *retval = new vector<SimulationObject *>;
	string name;         /* Name of the location */
	int    numLocations; /* Number of locations  */

	ifstream configFile;
	configFile.open( inputFileName.c_str() );
	if(configFile.fail()) {
		cerr << "Could not open file: '" << inputFileName << "'" << endl;
		cerr << "Terminating simulation." << endl;
		abort();
	}

	/* Read in the number of locations */
	configFile >> numLocations;
	numObjects = numLocations;

	/* Setup the locations */
	for( int index = 0; index < numLocations; index++ ) {
		configFile >> name;
		//retval->push_back( new LocationObject(name) );
	}

	configFile.close();
	return retval;
}

const PartitionInfo *EpidemicApplication::getPartitionInfo(
					unsigned int numberOfProcessorsAvailable ) {

	Partitioner *myPartitioner = new RoundRobinPartitioner();
	return  myPartitioner->partition( getSimulationObjects(), numberOfProcessorsAvailable );
}

int EpidemicApplication::finalize() {

	return 0;
}

void EpidemicApplication::registerDeserializers() {
}

ArgumentParser & EpidemicApplication::getArgumentParser() {

	static ArgumentParser::ArgRecord args[] = {
		{ "-simulate", "input file name", &inputFileName, ArgumentParser::STRING, false },
		{ "", "", 0 }
	};

	static ArgumentParser *myArgParser = new ArgumentParser( args );
	return *myArgParser;
}

