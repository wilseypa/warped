#ifndef WARPED_MAIN_H
#define WARPED_MAIN_H


#include <string>                       // for string

class Application;
class Simulation;
class SimulationConfiguration;
class VTime;

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
    WarpedMain(Application* application, std::string configurationFileName,
               std::string simulateUntil = "",  bool debug = false);
    /**
       Default Deconstructor
    */
    ~WarpedMain();

    /**
       This is equivalent to "int main( int argc, char **argv )" for a warped
       app.
    */
    int main(int argc, char** argv);

    /**
       Our call to register serializable types with the system.  Anyone
       expecting the serializable types to function needs to call this method
       first.
    */
    static void registerKernelDeserializers();

    /**
       This does everything up until the moment that the simulation is about
       to start.  The SimulationManager that is returned can be used
    */
    void initializeSimulation();

    /**
       Tells the simulation to run until the absolute time passed in.
    */
    void simulate(const VTime& simulateUntil);

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
    const VTime& getCommittedTime();
    /**
       Returns the time of the next event that we will execute.
    */
    const VTime& getNextEventTime();

private:
    std::string configurationFileName;
    bool debugFlag;
    std::string simulateUntil;

    Application* myApplication;
    Simulation* mySimulation;
};

#endif
