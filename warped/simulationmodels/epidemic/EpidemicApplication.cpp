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
#include <warped/PartitionInfo.h>
#include <warped/DeserializerManager.h>
#include <utils/ArgumentParser.h>

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using std::string;


EpidemicApplication::EpidemicApplication(): inputFileName( "" ), numObjects( 0 ) { }

int EpidemicApplication::initialize( vector<string> &arguments ){

	getArgumentParser().checkArgs( arguments );

	if( inputFileName.empty() ) {
		std::cerr << "A epidemicSim configuration file must be specified using -simulate" << std::endl;
		abort();
	}
	return 0;
}

int EpidemicApplication::getNumberOfSimulationObjects(int mgrId) const {

  return numObjects;
}

const PartitionInfo *EpidemicApplication::getPartitionInfo(
					unsigned int numberOfProcessorsAvailable ) {
  return NULL;
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

