// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "Application.h"
#include "WarpedMain.h"
#include "SimulationConfiguration.h"
#include "Simulation.h"
#include "Spinner.h"
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdio>
#include <algorithm>

// We need these to register their deserializers
#include "CirculateInitializationMessage.h"
#include "DeserializerManager.h"
#include "EventMessage.h"
#include "GVTUpdateMessage.h"
#include "InitializationMessage.h"
#include "UsefulWorkMessage.h"
#include "IntVTime.h"
#include "MatternGVTMessage.h"
#include "NegativeEventMessage.h"
#include "NegativeEvent.h"
#include "RestoreCkptMessage.h"
#include "StartMessage.h"
#include "TerminateToken.h"
#include "WarpedMain.h"

using std::string;
using std::fstream;
using std::ofstream;
using std::ifstream;
using std::istringstream;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

// maximum length of command line arguments
#define MAX_COMMAND_LINE_LENGTH 256

ArgumentParser::ArgRecord *
WarpedMain::getArgumentList( WarpedMain &main ) {
  static ArgumentParser::ArgRecord args[] = {
    {"-configuration", "specify configuration file", &main.configurationFileName,
     ArgumentParser::STRING, false},
    {"-simulateUntil", "specify a simulation end time", &main.simulateUntil,
     ArgumentParser::STRING, false},
    {"-debug", "display debug messages", &main.debugFlag, 
     ArgumentParser::BOOLEAN, false},
    {"", ""}
  };
    
  return args;
};

WarpedMain::WarpedMain( Application *initApplication ) :
  errors( 0 ),
  warnings( 0 ),
  configurationFileName( "" ),
  debugFlag( false ),
  simulateUntil( "" ),
  myApplication( initApplication ),
  mySimulation( 0 ){}

WarpedMain::~WarpedMain()
{
	delete mySimulation;
	delete myApplication;
}

void
WarpedMain::displayParameters( string executableName ){
  ArgumentParser ap( getArgumentList( *this ) );
  ap.printUsage( executableName, cerr );
}

bool
WarpedMain::checkConfigFile( string configFileName ){
  string choice;
  if( configFileName == "" ){
    cerr << "A Simulation Configuration File has not been specified" << endl;
    cerr << "Shall I create a default configuration: [y/n]: ";
    cin >> choice;
    // we are going to uppercase whatever the choice is ...
    string upChoice = choice;
    std::transform(upChoice.begin(), upChoice.end(), upChoice.begin(),(int(*)(int)) std::toupper);
            
    if( upChoice == "Y" ){
      cerr << "Creating default configuration file: simulation.conf"
	   << endl;
      cerr << "This has not been implemented yet ..." << endl;
      exit(-1);
    }
  }
  return true;
}

void
WarpedMain::registerKernelDeserializers(){
  CirculateInitializationMessage::registerDeserializer();
  EventMessage::registerDeserializer();
  GVTUpdateMessage::registerDeserializer();
  InitializationMessage::registerDeserializer();
  UsefulWorkMessage::registerDeserializer();
  IntVTime::registerDeserializer();
  MatternGVTMessage::registerDeserializer();
  NegativeEvent::registerDeserializer();
  NegativeEventMessage::registerDeserializer();
  RestoreCkptMessage::registerDeserializer();
  StartMessage::registerDeserializer();
  TerminateToken::registerDeserializer();
}

vector<string>
WarpedMain::buildArgumentVector( int argc, char **argv ){
  vector<string> retval;
  for( int i = 0; i < argc; i++ ){
    retval.push_back( string( argv[i] ) );
  }
  return retval;
}

SimulationConfiguration *
WarpedMain::readConfiguration( const string &configurationFileName,
			       const vector<string> &argumentVector ){
  SimulationConfiguration *configuration = 0;
  if( configurationFileName != "" ){
    if( checkConfigFile( configurationFileName ) == false ){
      cerr << "Can't read configuration file " << configurationFileName << endl;
      exit( -1 );
    }
    configuration = SimulationConfiguration::parseConfiguration( configurationFileName,
								 argumentVector );
    if( configuration == 0 ){
      cerr << "There was a problem parsing configuration " << configurationFileName
	   << ", exiting." << endl;
      exit( -1 );
    } else {
      cerr << "Using configuration file: " << configurationFileName << endl;
    }
  }
  return configuration;
}

void
WarpedMain::initializeSimulation( vector<string> &commandLineArgs ){
  registerKernelDeserializers();
  myApplication->registerDeserializers();

  ArgumentParser ap( getArgumentList( *this ));
  ap.checkArgs( commandLineArgs, false );

  if( debugFlag == true ){
    utils::enableDebug();
    utils::debug << "Debug output enabled with -debug" << endl;
  }

  SimulationConfiguration *configuration =
          readConfiguration( configurationFileName, commandLineArgs );

  if( configuration != 0 ){
    Spinner::spinIfRequested( "SpinBeforeConfiguration", *configuration );
  }
  // We have to let the application initialize before we can do much else.
  myApplication->initialize( commandLineArgs );

  // else, configuration is NULL, and we'll run with a default configuration
  mySimulation = Simulation::instance( configuration, myApplication );
  mySimulation->initialize();


  delete configuration;
}

void
WarpedMain::simulate( const VTime &simulateUntil ){
  mySimulation->simulate( simulateUntil );
}

bool
WarpedMain::simulationComplete( ){
  return mySimulation->simulationComplete();
}

void
WarpedMain::finalize(){
  mySimulation->finalize();
}

const VTime &
WarpedMain::getCommittedTime(){
  return mySimulation->getCommittedTime();
}

const VTime &
WarpedMain::getNextEventTime(){
  return mySimulation->getNextEventTime();
}


int 
WarpedMain::main( int argc, char **argv ){
  vector<string> args = buildArgumentVector( argc, argv );
  initializeSimulation( args );
    
  if (simulateUntil == "") {
    simulate( myApplication->getPositiveInfinity() );
  }
  else {
    simulate( myApplication->getTime(simulateUntil));
  }

  finalize();

  return errors;
}
