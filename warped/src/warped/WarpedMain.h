#ifndef WARPED_MAIN_H
#define WARPED_MAIN_H


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
    WarpedMain(Application* application, string configurationFileName, 
               string simulateUntil = "",  bool debug = false);
  /**
     Default Deconstructor
  */
  ~WarpedMain();
  
  /**
     This is equivalent to "int main( int argc, char **argv )" for a warped
     app.
  */
  int main(int argc, char **argv);

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
  void initializeSimulation( vector<string> &args );

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
     Builds a vector of strings from a standard "C"-style argument list.
  */
  vector<string> buildArgumentVector( int, char ** );
  
  // variables used to catching warnings or errors
  int errors;
  int warnings;
  string configurationFileName;
  bool debugFlag;
  string simulateUntil;

  Application *myApplication;
  Simulation *mySimulation;
};

#endif
