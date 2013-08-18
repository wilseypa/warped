
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

WarpedMain::WarpedMain(Application* application, string configurationFileName,
                       string simulateUntil,  bool debug) :
  errors(0),
  warnings(0),
  configurationFileName(configurationFileName),
  debugFlag(debug),
  simulateUntil(simulateUntil),
  myApplication(application),
  mySimulation(0) {}

WarpedMain::~WarpedMain() {
	delete mySimulation;
	delete myApplication;
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

SimulationConfiguration*
WarpedMain::readConfiguration(const string& configurationFileName,
                              const vector<string>& argumentVector) {
  SimulationConfiguration* configuration = 0;
  if (!configurationFileName.empty()) {
    configuration = SimulationConfiguration::parseConfiguration(configurationFileName,
                    argumentVector);
    if (configuration == 0) {
      cerr << "There was a problem parsing configuration " << configurationFileName
           << ", exiting." << endl;
      exit(-1);
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

  if( debugFlag == true ){
    debug::enableDebug();
    debug::debugout << "Debug output enabled." << endl;
  }

  SimulationConfiguration *configuration =
          readConfiguration( configurationFileName, commandLineArgs );

  if( configuration != 0 ){
    Spinner::spinIfRequested( "SpinBeforeConfiguration", *configuration );
  }

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
WarpedMain::main(int argc, char **argv) {
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
