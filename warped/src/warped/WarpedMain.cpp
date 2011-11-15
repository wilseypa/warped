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
#include "CFRollbackVectorMessage.h"
#include "CFRollbackAverageMessage.h"
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
  slaveArgumentFile( "slave-args" ),
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

vector< vector<string> >
WarpedMain::readSlaveArguments( const vector<string> &commandLineArgs ){
  //becuase utils::debug is defined by a command line argument and we are reading
  //the command line arguments now its not possible to use utils::debug to display
  //our error messages
  cerr << "About to read \"" << slaveArgumentFile << "\"" << endl;
  ifstream infile(slaveArgumentFile.c_str());
  
  if(!infile){
    perror( ("Slave error - unable to open file " + slaveArgumentFile+ ":" ).c_str() );
    exit(-1);
  }
  string argument;
  vector<string> vstring;
  vector< vector<string> > slaveArgs(2, vstring);
  //this line reads the file to restore the simulation arguments
  while(infile >> argument){
    //cerr << " arg:\"" << argument << "\"" << endl;
    slaveArgs[0].push_back( argument );
  }
  //This line adds in all the MPI command line arguments
  slaveArgs[1].insert( slaveArgs[1].begin(), commandLineArgs.begin(), commandLineArgs.end() );
  /*
  for ( int i = 0; i<slaveArgs[0].size(); i++){
     //cerr << "SimArg[" << i << "]: " << slaveArgs[0][i] << endl;
  }
  for ( int i = 0; i<slaveArgs[1].size(); i++){
     //cerr << "MPIArg[" << i << "]: " << slaveArgs[1][i] << endl;
  }*/
  infile.close();
  return slaveArgs;
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
  CFRollbackVectorMessage::registerDeserializer();
  CFRollbackAverageMessage::registerDeserializer();
  IntVTime::registerDeserializer();
  MatternGVTMessage::registerDeserializer();
  NegativeEvent::registerDeserializer();
  NegativeEventMessage::registerDeserializer();
  RestoreCkptMessage::registerDeserializer();
  StartMessage::registerDeserializer();
  TerminateToken::registerDeserializer();
}

vector< vector<string> >
WarpedMain::doSlaveInit( const vector<string> &commandLineArgs ){
  cerr << "doSlaveInit called" << endl;

  // if i am a slave process, then i have to read the command line
  // parameters from a file instead of reading it from argc and
  // args as MPI hasn't sent us the simulation command line
  // parameters yet. So we are forced to read this from slaveArgumentFile.
  // the master process writes slaveArgumentFile to its run directory;
  // you may have to copy it to the directory where the slave is spawned from;
  // i.e. your home directory
  return readSlaveArguments( commandLineArgs );
}

void
WarpedMain::writeSlaveArguments( const vector<string> &masterArgs ){
  fstream outFile;
  outFile.open(slaveArgumentFile.c_str(), ios::out );
  if(!outFile){
      cerr << "Error trying to write " << slaveArgumentFile << ":" << endl;
      exit(-1);
  }
      
  for( unsigned int count = 1; count < masterArgs.size(); count++ ){
    outFile << masterArgs[count] << " ";
  }
  outFile << endl;
  outFile.close();
}

vector<string>
WarpedMain::doMasterInit( const vector<string> &masterArgs ){
  cerr << "doMasterInit called" << endl;
  // if i am the master, i should create the slaveArgumentFile file so
  // that user doesn't have to do this. The reason I need to do
  // this is because only I have complete access to the
  // simulation's command line parameters (at this point; before a
  // call to physicalInit).
  writeSlaveArguments( masterArgs );
  return masterArgs;
}

vector<string>
WarpedMain::buildArgumentVector( int argc, char **argv ){
  vector<string> retval;
  for( int i = 0; i < argc; i++ ){
    retval.push_back( string( argv[i] ) );
  }
  return retval;
}

bool
WarpedMain::amSlave( const vector<string> &args ) const {
  bool retval = false;
  for( vector<string>::const_iterator i = args.begin();
       i < args.end(); i++ ){
    //MPI_Init adds in extra arguments and -p4amslave is one of them
    //These will need to be hidden from the simulation application
    //because it will not understand what they are for
    if( *i == "-p4amslave" ){
      retval = true;
    }
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

vector< vector<string> >
WarpedMain::initMasterOrSlave( const vector<string> &args ){
   //argumentsToUse[0] = Simulation arguments
   //argumentsToUse[1] = MPI arguments
  vector<string> vstring;
  vector< vector<string> > argumentsToUse(2, vstring);
  // We have to do some special stuff here due to the fact that we haven't
  // read our configuration yet and don't even know if we're running
  // parallel.  Furthermore, we won't have our parameters for some time if
  // we happen to be an MPI slave process.
  if( amSlave( args ) ){
    //If we are a slave process both the simulation and the MPI process
    //are going to have arguments
    argumentsToUse = doSlaveInit( args );
  }
  else {
    //If this is the master process their are not MPI arguments
    argumentsToUse[0]= doMasterInit( args );
    argumentsToUse[1] = argumentsToUse[0];
  }

  ArgumentParser ap( getArgumentList( *this ));
  ap.checkArgs( argumentsToUse[0], false );

  return argumentsToUse;
}

void
WarpedMain::initializeSimulation( const vector<string> &commandLineArgs ){
  registerKernelDeserializers();
  myApplication->registerDeserializers();

  //Get commandLineArguments if this is a slave process from MPI this will
  //determine the extra agruments
  vector< vector<string> > masterOrSlaveArgs = initMasterOrSlave( commandLineArgs );
  
  if( debugFlag == true ){
    utils::enableDebug();
    utils::debug << "Debug output enabled with -debug" << endl;
  }

  SimulationConfiguration *configuration =
          readConfiguration( configurationFileName, masterOrSlaveArgs[1] );

  if( configuration != 0 ){
    Spinner::spinIfRequested( "SpinBeforeConfiguration", *configuration );
  }
  // We have to let the application initialize before we can do much else.
  myApplication->initialize( masterOrSlaveArgs[0] );

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
