#ifndef WARPED_MAIN_H
#define WARPED_MAIN_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/ArgumentParser.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

class VTime;
class Application;
class Simulation;
class SimulationConfiguration;

/**
   This class implements the main function for warped.

   The intention of this class is that an application does something like
   the following to bootstrap itself:
 
   #include "MyApplication.h"
  
   int main( int argc, char *arv ){
     WarpedMain wm;
     return wm.main( new MyApplication(), argc, argv );
   }
   
*/
class WarpedMain {
public:
  /**
     Constructor to be called by user's main.
  */
  WarpedMain( Application *application );
  /**
     Default Deconstructor
  */
  ~WarpedMain();
  /**
     This is equivalent to "int main( int argc, char **argv )" for a warped
     app.
  */
  int main( int argc, char **argv );

  /**
     Our call to register serializable types with the system.  Anyone
     expecting the serializable types to function needs to call this method
     first.
  */
  static void registerKernelDeserializers();

  SimulationConfiguration *readConfiguration( const string &configurationFileName,
					      const vector<string> &argumentVector );
  
  /**
     This does everything up until the moment that the simulation is about
     to start.  The SimulationManager that is returned can be used
  */
  void initializeSimulation( const vector<string> &args );

  /**
     Tells the simulation to run until the absolute time passed in.
  */
  void simulate( const VTime &simulateUntil );

  /**
     If the simulation is being run in small time steps, this method
     provides a mechanism for determining if the simulation has completed.
  */
  bool simulationComplete();

  /**
     Cleans up the simulation.
  */
  void finalize();

  /**
     Returns the simulation time that we have advanced to.
  */
  const VTime &getCommittedTime();
  /**
     Returns the time of the next event that we will execute.
  */
  const VTime &getNextEventTime();

private:
  /**
     This function decides if we're master or slave and does the right
     thing depending on which we are.  Returns the argument list that we
     should use to finish initializing.
  */
  vector< vector<string> > initMasterOrSlave( const vector<string> &argumentVector );

  /**
     Initializes us if we're a slave process.
  */
  vector< vector<string> > doSlaveInit( const vector<string> &argumentVector );
  /**
     Initializes us if we're a master process.
  */
  vector<string> doMasterInit( const vector<string> &argumentVector );

  /**
     The way MPICH works, we have to communicate slave arguments via a
     file.  This is because when we called MPI_Init, it starts the remote
     process without the arguments we passed.
  */
  void writeSlaveArguments( const vector<string> &masterArgs );


  /**
     Builds a vector of strings from a standard "C"-style argument list.
  */
  vector<string> buildArgumentVector( int, char ** );

  static ArgumentParser::ArgRecord *getArgumentList( WarpedMain &setMyVariables );

  void displayParameters( string executableName );
  /**
     This method returns the slave arguments that the master wrote out for
     us if we're a slave.
  */
  vector< vector<string> > readSlaveArguments( const vector<string> &commandLineArguments );
  bool checkConfigFile( string configFileName );

  // variables used to catching warnings or errors
  int errors;
  int warnings;
  string configurationFileName;
  bool debugFlag;
  string simulateUntil;
  
  /** this is the file that will be read by "slave" processes */
  const string slaveArgumentFile;

  Application *myApplication;
  Simulation *mySimulation;

  /** This method checks to see if we're a slave based on the command line
      arguments.  (A hack to support MPICH.)  If we find the magic argument
      we erase it. */
  bool amSlave( const vector<string> &args ) const;

};

#endif
